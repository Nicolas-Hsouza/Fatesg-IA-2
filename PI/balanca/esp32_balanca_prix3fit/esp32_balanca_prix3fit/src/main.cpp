// =============================================================================
// main.cpp
//
// Firmware principal — ESP32 + Balança Prix 3 Fit + MongoDB Atlas
// Projeto: Integração industrial RS232 → Wi-Fi → MongoDB
//
// Arquitetura:
//   - ScaleManager: leitura RS232 via máquina de estados (UART2)
//   - WifiManager:  gestão Wi-Fi com reconexão automática
//   - MongoService: envio HTTPS para MongoDB Atlas Data API
//
// Loop principal:
//   - NÃO usa delay() (exceto no setup para estabilização)
//   - Todas as operações temporais via millis()
//   - Watchdog lógico por software (30s sem atividade = restart)
//
// Monitor serial: 115200 bps
// =============================================================================

#include <Arduino.h>
#include "config.h"
#include "ScaleManager.h"
#include "WifiManager.h"
#include "MongoService.h"

// =============================================================================
// Instâncias dos módulos
// Serial2 corresponde ao UART2 da ESP32 (RX=GPIO16, TX=GPIO17)
// =============================================================================
static ScaleManager scaleManager(Serial2);
static WifiManager  wifiManager;
static MongoService mongoService;

// =============================================================================
// Watchdog lógico por software
// O ESP-IDF tem watchdog de hardware (TWDT), mas também usamos um lógico
// para detectar travamentos na lógica da aplicação.
// =============================================================================
static uint32_t watchdogLastFedMs = 0;

inline void feedWatchdog() {
    watchdogLastFedMs = millis();
}

void checkWatchdog() {
    if ((millis() - watchdogLastFedMs) > WATCHDOG_TIMEOUT_MS) {
        Serial.println("[WDT] WATCHDOG DISPARADO! Sistema travado. Reiniciando...");
        Serial.flush();
        delay(500); // Garante que o log foi transmitido
        ESP.restart();
    }
}

// =============================================================================
// Callbacks de evento Wi-Fi
// =============================================================================
void onWifiConnected() {
    Serial.println("[Main] Wi-Fi conectado. Sistema operacional.");
}

void onWifiDisconnected() {
    Serial.println("[Main] Wi-Fi desconectado. Ativando fila offline.");
}

// =============================================================================
// Diagnóstico periódico de heap e estado do sistema
// =============================================================================
static uint32_t lastDiagMs = 0;
static constexpr uint32_t DIAG_INTERVAL_MS = 60000UL; // A cada 1 minuto

void printDiagnostics() {
    uint32_t now = millis();
    if ((now - lastDiagMs) < DIAG_INTERVAL_MS) return;
    lastDiagMs = now;

    uint32_t heap     = esp_get_free_heap_size();
    uint32_t minHeap  = esp_get_minimum_free_heap_size();
    uint32_t uptime_s = now / 1000;

    Serial.println("=== DIAGNÓSTICO ===");
    Serial.printf("  Uptime:        %lu s (%lu min)\n", uptime_s, uptime_s / 60);
    Serial.printf("  Heap livre:    %lu bytes\n", heap);
    Serial.printf("  Heap mínimo:   %lu bytes (histórico)\n", minHeap);
    Serial.printf("  WiFi estado:   %d | RSSI: %d dBm\n",
                  (int)wifiManager.getState(), wifiManager.getRSSI());
    Serial.printf("  Scale estado:  %d\n", (int)scaleManager.getState());
    Serial.printf("  Pesos enviados:    %lu\n", mongoService.getSentCount());
    Serial.printf("  Erros de envio:    %lu\n", mongoService.getErrorCount());
    Serial.printf("  Fila offline:      %u/%d\n",
                  mongoService.getQueueSize(), OFFLINE_QUEUE_MAX_SIZE);
    Serial.printf("  Pacotes balança:   %lu (inválidos: %lu)\n",
                  scaleManager.getPacketsReceived(), scaleManager.getPacketsInvalid());
    Serial.printf("  Reconexões WiFi:   %lu\n", wifiManager.getReconnectCount());

    // Alerta crítico de heap baixo
    if (heap < 20000) {
        Serial.printf("[WARN] Heap crítico: %lu bytes! Monitorar vazamento.\n", heap);
    }
    Serial.println("==================");
}

// =============================================================================
// setup()
// =============================================================================
void setup() {
    // Inicializa Serial de debug (USB) antes de tudo
    Serial.begin(115200);
    delay(500); // Aguarda estabilização do UART USB

    Serial.println("\n\n");
    Serial.println("============================================");
    Serial.println("  ESP32 + Balança Prix 3 Fit + MongoDB");
    Serial.println("  Firmware v1.0.0");
    Serial.printf ("  Device ID: %s\n", DEVICE_ID);
    Serial.printf ("  Chip: %s | Rev: %d\n",
                   ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf ("  Flash: %lu KB | Heap: %lu bytes\n",
                   ESP.getFlashChipSize() / 1024,
                   esp_get_free_heap_size());
    Serial.println("============================================\n");

    // ---- Registra callbacks Wi-Fi antes de inicializar ----------------------
    wifiManager.onConnect(onWifiConnected);
    wifiManager.onDisconnect(onWifiDisconnected);

    // ---- Inicializa módulos -------------------------------------------------
    Serial.println("[Main] Inicializando ScaleManager (UART2)...");
    scaleManager.begin();

    Serial.println("[Main] Inicializando WifiManager...");
    wifiManager.begin();

    Serial.println("[Main] Inicializando MongoService...");
    mongoService.begin();

    // ---- Inicia watchdog lógico --------------------------------------------
    feedWatchdog();

    Serial.println("[Main] Setup concluído. Entrando no loop principal.");
    Serial.println("[Main] Aguardando dados da balança...\n");
}

// =============================================================================
// loop() — NÃO bloqueante. Cada módulo processa sua fatia de tempo.
// =============================================================================
void loop() {
    // ---- Alimenta o watchdog lógico -----------------------------------------
    feedWatchdog();

    // ---- Verifica watchdog --------------------------------------------------
    checkWatchdog();

    // ---- Atualiza Wi-Fi (reconexão automática, heartbeat) -------------------
    wifiManager.update();

    // ---- Processa bytes da balança (máquina de estados) --------------------
    if (scaleManager.update()) {
        ScaleReading reading = scaleManager.getReading();

        // Só envia ao MongoDB se a leitura for estável (opcional — remova o
        // filtro se quiser também capturar leituras instáveis/em movimento)
        if (reading.is_stable) {
            Serial.printf("[Scale] Peso estável: %.3f kg | Enviando...\n",
                          reading.weight_kg);
            mongoService.send(reading, wifiManager.isConnected());
        } else {
            Serial.printf("[Scale] Peso instável: %.3f kg (ignorado)\n",
                          reading.weight_kg);
        }
    }

    // ---- Atualiza MongoService (retry, flush da fila offline) ---------------
    mongoService.update(wifiManager.isConnected());

    // ---- Diagnóstico periódico ---------------------------------------------
    printDiagnostics();

    // Yield explícito — cede CPU para tarefas do FreeRTOS (stack TCP/IP, etc.)
    // Importante: NÃO usar delay() aqui. yield() é não-bloqueante.
    yield();
}
