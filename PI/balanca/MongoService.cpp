// =============================================================================
// MongoService.cpp
//
// Envio de dados para MongoDB Atlas via HTTPS + Data API.
//
// Payload enviado (exemplo real):
// {
//   "dataSource": "Cluster0",
//   "database":   "industrial",
//   "collection": "pesagens",
//   "document": {
//     "device_id":    "ESP32_BALANCA_01",
//     "location":     "LINHA_PRODUCAO_A",
//     "peso":         1.350,
//     "unidade":      "kg",
//     "status":       "stable",
//     "timestamp":    "2026-05-21T10:30:00Z",
//     "uptime_s":     3600,
//     "rssi_dbm":     -65
//   }
// }
//
// Resposta esperada do MongoDB Data API (HTTP 201):
// { "insertedId": "6650aaf1234567890abcdef0" }
// =============================================================================
#include "MongoService.h"
#include <WiFi.h>
#include <time.h>

#define LOG_MONGO   "[Mongo] "
#define LOG_MONGO_E "[Mongo][ERRO] "
#define LOG_MONGO_W "[Mongo][WARN] "

// ---------------------------------------------------------------------------
// Certificado CA raiz para o MongoDB Atlas (DigiCert Global Root G2)
// Necessário para WiFiClientSecure validar o certificado TLS do servidor.
// Válido até 2038 — atualizar se expirar.
// ---------------------------------------------------------------------------
static const char MONGODB_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo2IwYDAdBgNV
HQ4EFgQUTiJUIBiV5uNu5g/6+rkS7QYXjzkwHwYDVR0jBBgwFoAUTiJUIBiV5uNu
5g/6+rkS7QYXjzkwDwYDVR0TAQH/BAUwAwEB/zALBgNVHQ8EBAMCAYYwDQYJKoZI
hvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY1Yl9PMWLSn/pvtsrF9+wX3N3KjIx
VFpCuZDFrOixTBQ8N0QMcS8B6oQUm1xhLOcqDVYbCcIo+G9nHqmjXAI+/34N/No
VmkG8F5sTCHicbLEz3jHMLzJNAUxnQ5mPUcl4MYiSHM1L3GUO5mHWNQmDjJjAHN
T2d6GxlHpRU7c1DXKB7/gNEiFlyVBLr8+yIKRaAzSkxd5sSKZY0BLknHBkpAJ4y
NNobJjdlhqP4cPRYRIlVEKFLSwHmfxAiJPWbfXNJRhsaQ23V0bNE9Xb09bNv9u11
YHDQBxCx5rl28P+JFxh+x5gT7YieTmY=
-----END CERTIFICATE-----
)EOF";

// =============================================================================
// begin()
// =============================================================================
void MongoService::begin() {
    memset(_queue, 0, sizeof(_queue));

    // Configura NTP para timestamps precisos
    // Fuso horário UTC (offset 0) — ajuste se quiser horário local
    configTime(0, 0, "pool.ntp.org", "time.google.com");

    Serial.println(LOG_MONGO "Serviço inicializado. Endpoint: " MONGO_API_URL);
}

// =============================================================================
// send() — chamado pelo main quando há nova leitura
// =============================================================================
void MongoService::send(const ScaleReading& reading, bool wifi_connected) {
    if (!wifi_connected) {
        // Sem Wi-Fi: enfileira para envio posterior
        if (_enqueue(reading)) {
            Serial.printf(LOG_MONGO_W "Offline. Leitura %.3f kg enfileirada (%u/%d).\n",
                          reading.weight_kg, _queueSize(), OFFLINE_QUEUE_MAX_SIZE);
        } else {
            Serial.println(LOG_MONGO_E "Fila offline CHEIA. Leitura descartada!");
        }
        return;
    }

    if (_state != MongoState::IDLE) {
        // Serviço ocupado com outro envio — enfileira
        _enqueue(reading);
        return;
    }

    // Envia imediatamente
    _currentReading = reading;
    _retryCount = 0;
    _state = MongoState::SENDING;

    int httpCode = _doHttpPost(_currentReading);

    if (httpCode == 201 || httpCode == 200) {
        _sentCount++;
        _state = MongoState::IDLE;
        Serial.printf(LOG_MONGO "Documento enviado com sucesso. Peso: %.3f kg | HTTP %d\n",
                      _currentReading.weight_kg, httpCode);
    } else {
        _errorCount++;
        _retryCount = 1;
        _retryStartMs = millis();
        _retryWaitMs = _calcRetryDelay();
        _state = MongoState::RETRY;
        Serial.printf(LOG_MONGO_E "Falha no envio (HTTP %d). Retry em %lu ms.\n",
                      httpCode, _retryWaitMs);
    }
}

// =============================================================================
// update() — processa retry e flush da fila offline
// =============================================================================
void MongoService::update(bool wifi_connected) {
    // ---- Estado RETRY -------------------------------------------------------
    if (_state == MongoState::RETRY) {
        if (!wifi_connected) {
            // Wi-Fi caiu durante o retry — enfileira e desiste
            _enqueue(_currentReading);
            _state = MongoState::IDLE;
            return;
        }

        if ((millis() - _retryStartMs) >= _retryWaitMs) {
            if (_retryCount >= HTTP_MAX_RETRIES) {
                Serial.printf(LOG_MONGO_E "Máximo de retries (%d) atingido. Descartando.\n",
                              HTTP_MAX_RETRIES);
                _errorCount++;
                _state = MongoState::IDLE;
                return;
            }

            Serial.printf(LOG_MONGO "Retry #%u para peso %.3f kg...\n",
                          _retryCount, _currentReading.weight_kg);

            int httpCode = _doHttpPost(_currentReading);

            if (httpCode == 201 || httpCode == 200) {
                _sentCount++;
                _state = MongoState::IDLE;
                Serial.printf(LOG_MONGO "Retry bem-sucedido (HTTP %d).\n", httpCode);
            } else {
                _retryCount++;
                _retryStartMs = millis();
                _retryWaitMs = _calcRetryDelay();
                Serial.printf(LOG_MONGO_E "Retry falhou (HTTP %d). Próximo em %lu ms.\n",
                              httpCode, _retryWaitMs);
            }
        }
        return;
    }

    // ---- Flush da fila offline quando Wi-Fi retorna -------------------------
    if (_state == MongoState::IDLE && wifi_connected && !_queueEmpty()) {
        ScaleReading r{};
        if (_dequeue(r)) {
            Serial.printf(LOG_MONGO "Flush offline: enviando peso %.3f kg...\n", r.weight_kg);
            send(r, true);
        }
    }
}

// =============================================================================
// _doHttpPost() — executa a requisição HTTPS
// =============================================================================
int MongoService::_doHttpPost(const ScaleReading& reading) {
    WiFiClientSecure client;

    // Usa o certificado CA para validar o TLS (IMPORTANTE para segurança)
    client.setCACert(MONGODB_CA_CERT);

    // Alternativa para ambientes de teste: desabilita verificação de certificado
    // client.setInsecure(); // NÃO usar em produção!

    HTTPClient http;
    http.setTimeout(HTTP_TIMEOUT_MS);

    if (!http.begin(client, MONGO_API_URL)) {
        Serial.println(LOG_MONGO_E "Falha ao inicializar HTTPClient.");
        return -1;
    }

    // Headers obrigatórios para a MongoDB Data API
    http.addHeader("Content-Type",  "application/json");
    http.addHeader("api-key",       MONGO_API_KEY);
    http.addHeader("Accept",        "application/json");

    String payload = _buildPayload(reading);

#ifdef DEBUG_VERBOSE
    Serial.printf(LOG_MONGO "Payload: %s\n", payload.c_str());
#endif

    int httpCode = http.POST(payload);

    if (httpCode > 0) {
        String response = http.getString();
        Serial.printf(LOG_MONGO "HTTP %d | Resposta: %s\n", httpCode, response.c_str());
    } else {
        Serial.printf(LOG_MONGO_E "Erro de rede: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return httpCode;
}

// =============================================================================
// _buildPayload() — monta o JSON para MongoDB Data API
// =============================================================================
String MongoService::_buildPayload(const ScaleReading& reading) {
    // JsonDocument com tamanho estimado para este payload (~512 bytes)
    JsonDocument doc;

    doc["dataSource"] = MONGO_DATA_SOURCE;
    doc["database"]   = MONGO_DATABASE;
    doc["collection"] = MONGO_COLLECTION;

    JsonObject document = doc["document"].to<JsonObject>();
    document["device_id"]  = DEVICE_ID;
    document["location"]   = DEVICE_LOCATION;
    document["peso"]       = serialized(String(reading.weight_kg, 3)); // 3 casas decimais
    document["unidade"]    = "kg";
    document["status"]     = reading.is_stable ? "stable" : "unstable";
    document["timestamp"]  = _getTimestamp(reading.timestamp_ms);
    document["uptime_s"]   = (int)(millis() / 1000);
    document["rssi_dbm"]   = WiFi.RSSI();
    document["fw_version"] = "1.0.0";

    // Campo para diagnóstico — remove em produção se quiser reduzir tamanho
#ifdef DEBUG_VERBOSE
    document["raw_packet"] = reading.raw;
#endif

    String output;
    serializeJson(doc, output);
    return output;
}

// =============================================================================
// _getTimestamp() — retorna ISO 8601 via NTP ou timestamp relativo
// =============================================================================
String MongoService::_getTimestamp(uint32_t reading_ms) {
    struct tm timeinfo;

    // Tenta obter hora via NTP (disponível após configTime + conexão Wi-Fi)
    if (getLocalTime(&timeinfo, 0)) {
        char buf[30];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
        return String(buf);
    }

    // Fallback: timestamp relativo em segundos desde boot
    // MongoDB não aceita isso como Date, mas é melhor que nada para diagnóstico
    uint32_t secs = reading_ms / 1000;
    char buf[32];
    snprintf(buf, sizeof(buf), "BOOT+%lus", (unsigned long)secs);
    return String(buf);
}

// =============================================================================
// Gerenciamento da fila offline (FIFO simples em array fixo)
// =============================================================================
bool MongoService::_enqueue(const ScaleReading& reading) {
    if (_queueFull()) return false;

    _queue[_queueTail] = OfflineItem{
        reading.weight_kg,
        reading.is_stable,
        reading.timestamp_ms,
        true
    };
    _queueTail = (_queueTail + 1) % OFFLINE_QUEUE_MAX_SIZE;
    return true;
}

bool MongoService::_dequeue(ScaleReading& out) {
    if (_queueEmpty()) return false;

    OfflineItem& item = _queue[_queueHead];
    out.weight_kg    = item.weight_kg;
    out.is_stable    = item.is_stable;
    out.timestamp_ms = item.timestamp_ms;
    out.is_valid     = true;

    item.used = false;
    _queueHead = (_queueHead + 1) % OFFLINE_QUEUE_MAX_SIZE;
    return true;
}

uint8_t MongoService::_queueSize() const {
    return (_queueTail - _queueHead + OFFLINE_QUEUE_MAX_SIZE) % OFFLINE_QUEUE_MAX_SIZE;
}

bool MongoService::_queueFull() const {
    return _queueSize() >= OFFLINE_QUEUE_MAX_SIZE - 1;
}

bool MongoService::_queueEmpty() const {
    return _queueHead == _queueTail;
}

// =============================================================================
// _calcRetryDelay() — backoff exponencial: 1s, 2s, 4s (máx)
// =============================================================================
uint32_t MongoService::_calcRetryDelay() const {
    uint32_t delay = HTTP_RETRY_BASE_MS;
    for (uint8_t i = 1; i < _retryCount; i++) {
        delay *= 2;
        if (delay > 30000UL) { delay = 30000UL; break; }
    }
    return delay;
}
