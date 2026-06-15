// =============================================================================
// WifiManager.h
//
// Responsabilidade: gerenciamento completo do ciclo de vida Wi-Fi.
// Implementa máquina de estados com reconexão automática e heartbeat.
//
// Funcionalidades:
//   - Conexão inicial com timeout configurável
//   - Detecção automática de desconexão
//   - Reconexão com backoff exponencial (evita sobrecarga do AP)
//   - Heartbeat periódico (ping ao gateway ou verificação de IP)
//   - Callbacks opcionais para eventos de conexão/desconexão
//   - Não-bloqueante: usa millis() para todos os timeouts
// =============================================================================
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

// ---------------------------------------------------------------------------
// Estados da máquina de estados Wi-Fi
// ---------------------------------------------------------------------------
enum class WiFiState : uint8_t {
    DISCONNECTED,   // Sem conexão — aguardando iniciar tentativa
    CONNECTING,     // Tentativa em andamento (aguardando IP)
    CONNECTED,      // Conectado e com IP válido
    RECONNECTING    // Tentativa de reconexão após queda
};

// ---------------------------------------------------------------------------
// Callback de evento (tipo função void)
// ---------------------------------------------------------------------------
using WiFiEventCallback = void (*)();

// ---------------------------------------------------------------------------
// WifiManager
// ---------------------------------------------------------------------------
class WifiManager {
public:
    WifiManager() = default;

    // -------------------------------------------------------------------------
    // Inicia o módulo Wi-Fi e dispara a primeira tentativa de conexão.
    // Deve ser chamado em setup().
    // -------------------------------------------------------------------------
    void begin();

    // -------------------------------------------------------------------------
    // Atualiza a máquina de estados. Chamar a cada iteração do loop().
    // NÃO bloqueante.
    // -------------------------------------------------------------------------
    void update();

    // -------------------------------------------------------------------------
    // Retorna true se estiver com IP válido neste instante
    // -------------------------------------------------------------------------
    bool isConnected() const;

    // -------------------------------------------------------------------------
    // Retorna o estado atual da máquina para debug
    // -------------------------------------------------------------------------
    WiFiState getState() const { return _state; }

    // -------------------------------------------------------------------------
    // Retorna o RSSI (força do sinal) em dBm. 0 se desconectado.
    // -------------------------------------------------------------------------
    int getRSSI() const;

    // -------------------------------------------------------------------------
    // Retorna IP local como String ("0.0.0.0" se desconectado)
    // -------------------------------------------------------------------------
    String getIP() const;

    // -------------------------------------------------------------------------
    // Registra callback chamado quando conexão é estabelecida (opcional)
    // -------------------------------------------------------------------------
    void onConnect(WiFiEventCallback cb) { _onConnect = cb; }

    // -------------------------------------------------------------------------
    // Registra callback chamado quando conexão é perdida (opcional)
    // -------------------------------------------------------------------------
    void onDisconnect(WiFiEventCallback cb) { _onDisconnect = cb; }

    // -------------------------------------------------------------------------
    // Estatísticas
    // -------------------------------------------------------------------------
    uint32_t getReconnectCount() const { return _reconnectCount; }

private:
    WiFiState _state = WiFiState::DISCONNECTED;

    // Controle de tempo
    uint32_t _connectStartMs    = 0;
    uint32_t _lastHeartbeatMs   = 0;
    uint32_t _reconnectDelayMs  = 5000;  // Começa com 5s, dobra a cada falha

    // Tentativas de reconexão (para cálculo de backoff)
    uint8_t  _reconnectAttempts = 0;
    uint32_t _reconnectCount    = 0;

    // Callbacks de evento
    WiFiEventCallback _onConnect    = nullptr;
    WiFiEventCallback _onDisconnect = nullptr;

    // Flag para detectar transição de estado (disparo de callbacks)
    bool _wasConnected = false;

    // -------------------------------------------------------------------------
    // Métodos privados
    // -------------------------------------------------------------------------
    void _startConnection();
    void _handleConnecting();
    void _handleConnected();
    void _handleReconnecting();
    uint32_t _calcBackoffMs() const;
    void _printStatus() const;
};
