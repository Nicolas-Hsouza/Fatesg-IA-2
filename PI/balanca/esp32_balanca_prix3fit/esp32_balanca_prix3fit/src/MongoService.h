// =============================================================================
// MongoService.h
//
// Responsabilidade: envio de leituras de peso para o MongoDB Atlas via HTTPS.
// Usa a MongoDB Data API (HTTP endpoint), sem necessidade de driver nativo.
//
// Funcionalidades:
//   - POST HTTPS com certificado CA embutido
//   - Serialização JSON via ArduinoJson
//   - Retry com backoff exponencial
//   - Fila offline (até OFFLINE_QUEUE_MAX_SIZE itens na RAM)
//   - Flush automático da fila ao reconectar
//   - Logs detalhados de resposta HTTP
// =============================================================================
#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "ScaleManager.h"

// ---------------------------------------------------------------------------
// Estados da operação de envio
// ---------------------------------------------------------------------------
enum class MongoState : uint8_t {
    IDLE,        // Sem operação em andamento
    SENDING,     // Requisição HTTP em andamento
    RETRY,       // Aguardando próxima tentativa (backoff)
    OFFLINE      // Sem Wi-Fi — dados sendo enfileirados localmente
};

// ---------------------------------------------------------------------------
// Item da fila offline (peso + timestamp ISO8601)
// ---------------------------------------------------------------------------
struct OfflineItem {
    float    weight_kg;
    bool     is_stable;
    uint32_t timestamp_ms; // millis() no momento da leitura
    bool     used = false;
};

// ---------------------------------------------------------------------------
// MongoService
// ---------------------------------------------------------------------------
class MongoService {
public:
    MongoService() = default;

    // -------------------------------------------------------------------------
    // Inicializa o serviço. Deve ser chamado em setup().
    // -------------------------------------------------------------------------
    void begin();

    // -------------------------------------------------------------------------
    // Tenta enviar uma leitura. Se offline, enfileira localmente.
    // NÃO bloqueante — o envio pode ocorrer em futuras chamadas de update().
    // -------------------------------------------------------------------------
    void send(const ScaleReading& reading, bool wifi_connected);

    // -------------------------------------------------------------------------
    // Atualiza a máquina de estados. Chamar a cada loop().
    // Faz flush da fila offline se Wi-Fi estiver disponível.
    // -------------------------------------------------------------------------
    void update(bool wifi_connected);

    // -------------------------------------------------------------------------
    // Retorna o estado atual
    // -------------------------------------------------------------------------
    MongoState getState() const { return _state; }

    // -------------------------------------------------------------------------
    // Estatísticas de operação
    // -------------------------------------------------------------------------
    uint32_t getSentCount()   const { return _sentCount; }
    uint32_t getErrorCount()  const { return _errorCount; }
    uint32_t getQueueSize()   const { return _queueSize(); }

private:
    MongoState _state = MongoState::IDLE;

    // Retry
    uint8_t  _retryCount  = 0;
    uint32_t _retryWaitMs = 0;
    uint32_t _retryStartMs = 0;

    // Fila offline em memória (circular simples)
    OfflineItem _queue[OFFLINE_QUEUE_MAX_SIZE];
    uint8_t     _queueHead = 0;
    uint8_t     _queueTail = 0;

    // Leitura atual sendo enviada/retentada
    ScaleReading _currentReading{};

    // Estatísticas
    uint32_t _sentCount  = 0;
    uint32_t _errorCount = 0;

    // -------------------------------------------------------------------------
    // Métodos privados
    // -------------------------------------------------------------------------

    // Executa a requisição HTTP POST para o MongoDB Data API
    // Retorna código HTTP (200/201 = sucesso, negativo = erro de rede)
    int _doHttpPost(const ScaleReading& reading);

    // Monta o payload JSON para a Data API do MongoDB
    String _buildPayload(const ScaleReading& reading);

    // Retorna timestamp ISO 8601 a partir do millis() da leitura
    // Usa NTP se disponível, senão usa millis() relativo
    String _getTimestamp(uint32_t reading_ms);

    // Gerenciamento da fila offline
    bool     _enqueue(const ScaleReading& reading);
    bool     _dequeue(ScaleReading& out);
    uint8_t  _queueSize() const;
    bool     _queueFull()  const;
    bool     _queueEmpty() const;

    // Backoff exponencial: 1s → 2s → 4s (max HTTP_RETRY_BASE_MS * 2^retry)
    uint32_t _calcRetryDelay() const;
};
