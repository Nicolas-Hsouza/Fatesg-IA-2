// =============================================================================
// ScaleManager.cpp
//
// Implementação completa do parser serial para Balança Prix 3 Fit.
// Protocolo Toledo Continuous Output via RS232 → MAX232 → UART2 da ESP32.
//
// Formato de pacote da Prix 3 Fit:
//   Byte  0:    STX (0x02)
//   Byte  1:    Status 1 — 'S' (stable) ou 'U' (unstable/motion)
//   Byte  2:    Status 2 — 'S' (stable) ou 'D' (dynamic)
//   Byte  3:    Sinal — '+' ou '-'
//   Bytes 4–13: Peso com ponto decimal (ex: "  1.350  " ou "000010.500")
//   Bytes 14–15: Unidade ("kg" ou "lb")
//   Byte  16:   ETX (0x03)
//   Byte  17:   CR (0x0D)
//   Byte  18:   LF (0x0A)
//
// Nota: a balança pode enviar pacotes com formatação ligeiramente diferente
// dependendo da versão de firmware. O parser é tolerante a espaços extras.
// =============================================================================
#include "ScaleManager.h"

// Caracteres de controle do protocolo Toledo
static constexpr uint8_t PROTO_STX = 0x02;  // Start of Text
static constexpr uint8_t PROTO_ETX = 0x03;  // End of Text
static constexpr uint8_t PROTO_CR  = 0x0D;  // Carriage Return
static constexpr uint8_t PROTO_LF  = 0x0A;  // Line Feed

// Prefixos de log para o monitor serial
#define LOG_SCALE   "[Scale] "
#define LOG_SCALE_E "[Scale][ERRO] "
#define LOG_SCALE_W "[Scale][WARN] "

// =============================================================================
// Construtor
// =============================================================================
ScaleManager::ScaleManager(HardwareSerial& serial)
    : _serial(serial)
{
    memset(_buf, 0, sizeof(_buf));
    memset(&_lastReading, 0, sizeof(_lastReading));
    memset(&_pendingReading, 0, sizeof(_pendingReading));
}

// =============================================================================
// begin() — inicializa UART2
// =============================================================================
void ScaleManager::begin() {
    // HardwareSerial.begin(baud, mode, rx_pin, tx_pin)
    // SERIAL_8N1 = 8 bits de dados, sem paridade, 1 stop bit
    _serial.begin(SCALE_BAUD_RATE, SERIAL_8N1, SCALE_RX_PIN, SCALE_TX_PIN);

    // Limpa qualquer lixo no buffer de hardware após inicialização
    delay(100);
    while (_serial.available()) _serial.read();

    Serial.println(LOG_SCALE "UART2 inicializada: "
                   "RX=GPIO" + String(SCALE_RX_PIN) +
                   " TX=GPIO" + String(SCALE_TX_PIN) +
                   " @ " + String(SCALE_BAUD_RATE) + " bps");
    Serial.println(LOG_SCALE "Aguardando dados da balança Prix 3 Fit...");
}

// =============================================================================
// update() — chamar a cada loop(). Retorna true se leitura pronta.
// =============================================================================
bool ScaleManager::update() {
    // Verifica timeout de leitura parcial (evita travar em estado READING)
    _checkTimeout();

    // Se já temos uma leitura pronta pendente, retorna true imediatamente
    if (_state == ScaleState::READY) {
        return true;
    }

    // Processa todos os bytes disponíveis no buffer de hardware da UART
    // Isso é intencional: consumir tudo de uma vez é mais eficiente do que
    // processar um byte por chamada de loop()
    while (_serial.available() > 0) {
        uint8_t byte = (uint8_t)_serial.read();
        _processByte(byte);

        // Saímos do while se chegamos no estado READY ou se o buffer está vazio
        if (_state == ScaleState::READY) {
            return true;
        }
    }

    return false;
}

// =============================================================================
// getReading() — retorna leitura pronta e reseta estado para IDLE
// =============================================================================
ScaleReading ScaleManager::getReading() {
    ScaleReading r = _pendingReading;
    memset(&_pendingReading, 0, sizeof(_pendingReading));
    _state = ScaleState::IDLE;
    _bufLen = 0;
    _garbageCount = 0;
    return r;
}

// =============================================================================
// flush() — limpa buffer e reseta estado (recuperação de erro)
// =============================================================================
void ScaleManager::flush() {
    memset(_buf, 0, sizeof(_buf));
    _bufLen = 0;
    _state = ScaleState::IDLE;
    _garbageCount = 0;
    _readStartMs = 0;

    // Descarta também o buffer de hardware da UART
    while (_serial.available()) _serial.read();

    _stats.flush_count++;

    Serial.println(LOG_SCALE_W "Buffer limpo (flush). Reiniciando parser.");
}

// =============================================================================
// _processByte() — núcleo da máquina de estados
// =============================================================================
void ScaleManager::_processByte(uint8_t byte) {
    switch (_state) {

    // -------------------------------------------------------------------------
    case ScaleState::IDLE:
        if (byte == PROTO_STX) {
            // Início de pacote válido encontrado
            memset(_buf, 0, sizeof(_buf));
            _bufLen = 0;
            _buf[_bufLen++] = (char)byte;
            _readStartMs = millis();
            _garbageCount = 0;
            _state = ScaleState::READING;

#ifdef DEBUG_VERBOSE
            Serial.println(LOG_SCALE "STX recebido. Iniciando leitura de pacote.");
#endif
        } else {
            // Byte inválido antes do STX — conta como lixo
            _garbageCount++;
            if (_garbageCount >= SCALE_MAX_GARBAGE_BYTES) {
                Serial.printf(LOG_SCALE_W "Muitos bytes inválidos (%u). Forçando flush.\n",
                              _garbageCount);
                flush();
            }
        }
        break;

    // -------------------------------------------------------------------------
    case ScaleState::READING:
        if (byte == PROTO_ETX) {
            // Fim de pacote detectado
            _buf[_bufLen++] = (char)byte;
            _state = ScaleState::PARSING;

            // Processa imediatamente (ainda dentro do processamento do byte)
            ScaleReading newReading{};
            if (_parsePacket(newReading)) {
                if (_isNewReading(newReading)) {
                    _pendingReading = newReading;
                    _lastReading = newReading;
                    _lastReadingMs = millis();
                    _state = ScaleState::READY;
                    _stats.packets_received++;

                    Serial.printf(LOG_SCALE "Peso: %.3f kg | Status: %s\n",
                                  newReading.weight_kg,
                                  newReading.is_stable ? "estavel" : "instavel");
                } else {
                    // Leitura igual à anterior (debounce / leitura repetida)
#ifdef DEBUG_VERBOSE
                    Serial.println(LOG_SCALE "Leitura ignorada (debounce ou peso idêntico).");
#endif
                    _state = ScaleState::IDLE;
                    _bufLen = 0;
                }
            } else {
                // Pacote malformado
                _stats.packets_invalid++;
                Serial.printf(LOG_SCALE_E "Pacote inválido. Buffer: [%.*s]\n",
                              _bufLen, _buf);
                _state = ScaleState::ERROR;
            }
        } else if (byte == PROTO_CR || byte == PROTO_LF) {
            // CR e LF vêm depois do ETX — ignorar silenciosamente
            // (são consumidos sem sair do estado READING porque ETX já teria
            // feito a transição. Se chegarmos aqui, é que o ETX não veio.)
        } else if (byte == PROTO_STX) {
            // STX inesperado no meio do pacote: o pacote anterior estava incompleto
            Serial.println(LOG_SCALE_W "STX inesperado no meio do pacote. Reiniciando.");
            _stats.packets_invalid++;
            memset(_buf, 0, sizeof(_buf));
            _bufLen = 0;
            _buf[_bufLen++] = (char)byte;
            _readStartMs = millis();
            // Permanece em READING com o novo pacote
        } else if (_bufLen < SCALE_BUFFER_SIZE - 1) {
            // Byte normal de dados — acumula
            if (_isValidByte(byte)) {
                _buf[_bufLen++] = (char)byte;
            } else {
                // Byte inválido dentro de pacote: corrupção
                Serial.printf(LOG_SCALE_W "Byte inválido 0x%02X no meio do pacote.\n", byte);
                _stats.packets_invalid++;
                _state = ScaleState::ERROR;
            }
        } else {
            // Buffer overflow: pacote maior do que o esperado
            Serial.println(LOG_SCALE_E "Buffer overflow. Descartando pacote.");
            _stats.packets_invalid++;
            _state = ScaleState::ERROR;
        }
        break;

    // -------------------------------------------------------------------------
    case ScaleState::PARSING:
        // Este estado é transitório — o parse já ocorre dentro do READING.
        // Se chegamos aqui, é por race condition ou bug. Faz flush seguro.
        _state = ScaleState::ERROR;
        break;

    // -------------------------------------------------------------------------
    case ScaleState::ERROR:
        // No estado de erro, aguarda o próximo STX para recomeçar
        if (byte == PROTO_STX) {
            memset(_buf, 0, sizeof(_buf));
            _bufLen = 0;
            _buf[_bufLen++] = (char)byte;
            _readStartMs = millis();
            _garbageCount = 0;
            _state = ScaleState::READING;
        }
        // Qualquer outro byte é descartado silenciosamente
        break;

    // -------------------------------------------------------------------------
    case ScaleState::READY:
        // Não processa novos bytes enquanto o chamador não consumiu a leitura.
        // O buffer de hardware da UART pode acumular (até 128 bytes no ESP32).
        // Isso é OK: o próximo loop() vai drenar.
        break;
    }
}

// =============================================================================
// _parsePacket() — extrai peso e status do buffer acumulado
//
// Layout esperado no buffer após receber STX...dados...ETX:
// [0]  = STX (0x02)
// [1]  = status 1: 'S' (stable) ou 'U' (unstable)  ou espaço em alguns modelos
// [2]  = status 2: 'S' | 'D' | ' '
// [3]  = sinal: '+' ou '-'
// [4..13] = peso: "XXXXXXX.XXX" (10 chars, pode ter espaços à esquerda)
// [14..15] = unidade: "kg" (com possível espaço "lb")
// [16] = ETX (0x03)
//
// Nota: diferentes firmwares da Prix 3 podem variar 1-2 posições.
// O parser busca o campo numérico de forma flexível.
// =============================================================================
bool ScaleManager::_parsePacket(ScaleReading& out) {
    if (_bufLen < 10) {
        // Pacote curto demais
        return false;
    }

    // Verifica delimitadores
    if ((uint8_t)_buf[0] != PROTO_STX) return false;
    if ((uint8_t)_buf[_bufLen - 1] != PROTO_ETX) return false;

    // Trabalha com cópia sem STX e ETX
    char inner[SCALE_BUFFER_SIZE];
    int innerLen = _bufLen - 2; // Remove STX e ETX
    if (innerLen <= 0) return false;
    memcpy(inner, _buf + 1, innerLen);
    inner[innerLen] = '\0';

    // Extrai status de estabilidade (posição 0 e 1 do inner)
    // 'S' = stable, qualquer outro = unstable/motion
    bool stable = false;
    if (innerLen > 0) {
        char s1 = inner[0];
        char s2 = (innerLen > 1) ? inner[1] : ' ';
        // Prix 3 Fit: "SS" = estável, "SD" = instável, "US" = em movimento
        stable = (s1 == 'S');
        (void)s2; // Pode ser usado para lógica mais fina no futuro
    }

    // Extrai sinal (posição 2 do inner)
    bool negative = false;
    if (innerLen > 2) {
        negative = (inner[2] == '-');
    }

    // Busca campo numérico (começa na posição 3)
    // Procura por dígitos, ponto e espaços que formem o peso
    // Estratégia: varre do offset 3 até ETX procurando padrão "X.XXX"
    float weight = 0.0f;
    bool found_number = false;

    for (int start = 3; start < innerLen - 2 && !found_number; start++) {
        // Pula espaços
        int pos = start;
        while (pos < innerLen && inner[pos] == ' ') pos++;

        // Tenta converter a partir daqui
        char numStr[16];
        int numLen = 0;
        while (pos < innerLen && numLen < 15 &&
               (isdigit((uint8_t)inner[pos]) || inner[pos] == '.')) {
            numStr[numLen++] = inner[pos++];
        }
        numStr[numLen] = '\0';

        if (numLen >= 3 && strchr(numStr, '.') != nullptr) {
            // Parece um número de peso válido
            char* endptr = nullptr;
            float val = strtof(numStr, &endptr);
            if (endptr != numStr && val >= 0.0f && val < 9999.99f) {
                weight = negative ? -val : val;
                found_number = true;
            }
        }
    }

    if (!found_number) {
        return false;
    }

    // Extrai unidade (últimos 2 chars antes do ETX, ignorando espaços)
    // Para simplificação, assumimos "kg" como padrão da Prix 3 Fit no Brasil
    // A detecção dinâmica de unidade pode ser adicionada aqui futuramente

    // Preenche o resultado
    out.weight_kg    = weight;
    out.is_stable    = stable;
    out.is_valid     = true;
    out.timestamp_ms = millis();
    memcpy(out.raw, _buf, _bufLen);
    out.raw[_bufLen] = '\0';

    return true;
}

// =============================================================================
// _isNewReading() — verifica se a leitura é diferente da última (debounce)
// =============================================================================
bool ScaleManager::_isNewReading(const ScaleReading& r) const {
    // Verifica intervalo mínimo de tempo (debounce temporal)
    uint32_t now = millis();
    if ((now - _lastReadingMs) < SCALE_DEBOUNCE_MS) {
        return false;
    }

    // Verifica variação mínima de peso (filtra ruído analógico)
    float delta = fabsf(r.weight_kg - _lastReading.weight_kg);
    if (_lastReading.is_valid && delta < SCALE_MIN_DELTA_KG) {
        return false;
    }

    // Primeira leitura: sempre aceita
    if (!_lastReading.is_valid) {
        return true;
    }

    return true;
}

// =============================================================================
// _parseWeightString() — converte string "XXXXXX.XXX" para float
// =============================================================================
bool ScaleManager::_parseWeightString(const char* str, float& weight_out) {
    if (str == nullptr || strlen(str) == 0) return false;

    // Remove espaços à esquerda
    while (*str == ' ') str++;

    char* endptr = nullptr;
    float val = strtof(str, &endptr);

    if (endptr == str) return false; // Nenhum número encontrado
    if (val < 0.0f || val > 9999.0f) return false; // Fora da faixa esperada

    weight_out = val;
    return true;
}

// =============================================================================
// _isValidByte() — verifica se o byte é aceitável dentro de um pacote
// =============================================================================
bool ScaleManager::_isValidByte(uint8_t byte) const {
    // Aceita: ASCII imprimível (0x20–0x7E), CR (0x0D), LF (0x0A)
    // Rejeita: bytes de controle inesperados, bytes > 0x7F (não-ASCII)
    return (byte >= 0x20 && byte <= 0x7E) ||
           byte == PROTO_CR ||
           byte == PROTO_LF;
}

// =============================================================================
// _checkTimeout() — verifica se a leitura atual está demorando demais
// =============================================================================
void ScaleManager::_checkTimeout() {
    if (_state != ScaleState::READING) return;
    if (_readStartMs == 0) return;

    if ((millis() - _readStartMs) > SCALE_READ_TIMEOUT_MS) {
        Serial.printf(LOG_SCALE_W "Timeout de leitura (%lu ms). Bytes no buffer: %u. Flush.\n",
                      SCALE_READ_TIMEOUT_MS, _bufLen);
        _stats.timeouts++;
        flush();
    }
}
