// =============================================================================
// WifiManager.cpp
//
// Implementação da máquina de estados Wi-Fi com reconexão automática.
// Todas as operações são não-bloqueantes (millis-based).
// =============================================================================
#include "WifiManager.h"

#define LOG_WIFI   "[WiFi] "
#define LOG_WIFI_E "[WiFi][ERRO] "
#define LOG_WIFI_W "[WiFi][WARN] "

// Backoff máximo para reconexão (2 minutos)
static constexpr uint32_t MAX_BACKOFF_MS = 120000UL;

// =============================================================================
// begin() — inicializa o módulo Wi-Fi
// =============================================================================
void WifiManager::begin() {
    // Garante que o módulo Wi-Fi está em modo station (cliente), não AP
    WiFi.mode(WIFI_STA);

    // Desabilita reconexão automática do SDK — o WifiManager gerencia isso
    WiFi.setAutoReconnect(false);

    // Configura hostname do dispositivo na rede
    WiFi.setHostname(DEVICE_ID);

    Serial.printf(LOG_WIFI "Módulo Wi-Fi inicializado. SSID: %s\n", WIFI_SSID);

    // Inicia primeira conexão
    _startConnection();
}

// =============================================================================
// update() — chamar a cada loop()
// =============================================================================
void WifiManager::update() {
    switch (_state) {
    case WiFiState::DISCONNECTED:
        _startConnection();
        break;

    case WiFiState::CONNECTING:
        _handleConnecting();
        break;

    case WiFiState::CONNECTED:
        _handleConnected();
        break;

    case WiFiState::RECONNECTING:
        _handleReconnecting();
        break;
    }
}

// =============================================================================
// isConnected() — retorna true se tiver IP válido
// =============================================================================
bool WifiManager::isConnected() const {
    return (WiFi.status() == WL_CONNECTED) &&
           (_state == WiFiState::CONNECTED);
}

// =============================================================================
// getRSSI()
// =============================================================================
int WifiManager::getRSSI() const {
    if (!isConnected()) return 0;
    return WiFi.RSSI();
}

// =============================================================================
// getIP()
// =============================================================================
String WifiManager::getIP() const {
    if (!isConnected()) return "0.0.0.0";
    return WiFi.localIP().toString();
}

// =============================================================================
// _startConnection() — dispara tentativa de conexão
// =============================================================================
void WifiManager::_startConnection() {
    Serial.printf(LOG_WIFI "Conectando a '%s'...\n", WIFI_SSID);

    // WiFi.begin() é assíncrono — retorna imediatamente
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    _connectStartMs = millis();
    _state = WiFiState::CONNECTING;
}

// =============================================================================
// _handleConnecting() — aguarda conexão ou timeout
// =============================================================================
void WifiManager::_handleConnecting() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        _state = WiFiState::CONNECTED;
        _reconnectAttempts = 0;
        _reconnectDelayMs = 5000; // Reseta backoff

        _printStatus();

        // Dispara callback de conexão
        if (_onConnect) _onConnect();
        _wasConnected = true;

        return;
    }

    // Verifica timeout
    if ((millis() - _connectStartMs) > WIFI_CONNECT_TIMEOUT_MS) {
        Serial.printf(LOG_WIFI_W "Timeout de conexão após %lu ms. Status: %d\n",
                      WIFI_CONNECT_TIMEOUT_MS, (int)status);
        WiFi.disconnect(true);
        _state = WiFiState::RECONNECTING;
        _reconnectAttempts++;
        _connectStartMs = millis(); // Reusa para controle do backoff
    }

    // Status WL_NO_SSID_AVAIL ou WL_CONNECT_FAILED: não tenta imediatamente
    if (status == WL_NO_SSID_AVAIL) {
        Serial.println(LOG_WIFI_W "SSID não encontrado. Aguardando...");
    }
}

// =============================================================================
// _handleConnected() — verifica manutenção da conexão e heartbeat
// =============================================================================
void WifiManager::_handleConnected() {
    // Verifica se ainda está conectado
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println(LOG_WIFI_W "Conexão perdida! Iniciando reconexão...");
        _state = WiFiState::RECONNECTING;
        _reconnectAttempts = 0;
        _connectStartMs = millis();

        // Dispara callback de desconexão
        if (_wasConnected && _onDisconnect) _onDisconnect();
        _wasConnected = false;
        return;
    }

    // Heartbeat periódico: verifica sinal e loga status
    uint32_t now = millis();
    if ((now - _lastHeartbeatMs) >= WIFI_HEARTBEAT_INTERVAL_MS) {
        _lastHeartbeatMs = now;
        Serial.printf(LOG_WIFI "Heartbeat OK | IP: %s | RSSI: %d dBm\n",
                      WiFi.localIP().toString().c_str(),
                      WiFi.RSSI());
    }
}

// =============================================================================
// _handleReconnecting() — aplica backoff exponencial e tenta reconexão
// =============================================================================
void WifiManager::_handleReconnecting() {
    uint32_t delay = _calcBackoffMs();

    if ((millis() - _connectStartMs) >= delay) {
        _reconnectCount++;
        Serial.printf(LOG_WIFI "Tentativa de reconexão #%u (backoff: %lu ms)...\n",
                      _reconnectCount, delay);
        _startConnection();
    }
}

// =============================================================================
// _calcBackoffMs() — backoff exponencial com jitter
//   Tentativa 0: 5s, 1: 10s, 2: 20s, 3: 40s, 4+: 120s (máx)
// =============================================================================
uint32_t WifiManager::_calcBackoffMs() const {
    uint32_t delay = 5000UL;
    for (uint8_t i = 0; i < _reconnectAttempts && delay < MAX_BACKOFF_MS; i++) {
        delay *= 2;
    }
    return min(delay, MAX_BACKOFF_MS);
}

// =============================================================================
// _printStatus() — loga informações de conexão
// =============================================================================
void WifiManager::_printStatus() const {
    Serial.println(LOG_WIFI "=== CONECTADO ===");
    Serial.printf(LOG_WIFI "  SSID:    %s\n", WiFi.SSID().c_str());
    Serial.printf(LOG_WIFI "  IP:      %s\n", WiFi.localIP().toString().c_str());
    Serial.printf(LOG_WIFI "  Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf(LOG_WIFI "  RSSI:    %d dBm\n", WiFi.RSSI());
    Serial.printf(LOG_WIFI "  MAC:     %s\n", WiFi.macAddress().c_str());
}
