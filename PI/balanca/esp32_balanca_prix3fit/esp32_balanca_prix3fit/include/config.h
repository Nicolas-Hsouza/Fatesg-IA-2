// =============================================================================
// config.h — Configurações centralizadas do projeto
//
// ATENÇÃO: Em produção, NÃO versionar este arquivo no Git.
// Adicione "include/config.h" ao seu .gitignore.
// Use variáveis de ambiente do PlatformIO para substituição em CI/CD.
// =============================================================================
#pragma once

// -----------------------------------------------------------------------------
// Credenciais Wi-Fi
// -----------------------------------------------------------------------------
#define WIFI_SSID       "iot_fatesg"
#define WIFI_PASSWORD   "truma2iot_fatesg"

// Tempo máximo aguardando conexão Wi-Fi (ms)
#define WIFI_CONNECT_TIMEOUT_MS     15000UL
// Intervalo de tentativa de reconexão após falha (ms)
#define WIFI_RECONNECT_INTERVAL_MS  10000UL
// Intervalo de heartbeat para verificar conexão (ms)
#define WIFI_HEARTBEAT_INTERVAL_MS  30000UL

// -----------------------------------------------------------------------------
// MongoDB Atlas — Data API HTTP endpoint
//
// Como obter a URL:
//   1. Acesse cloud.mongodb.com → seu cluster → Data API
//   2. Copie o endpoint no formato abaixo
//   3. Em "API Access List", adicione o IP público do ESP32 ou use 0.0.0.0/0
// -----------------------------------------------------------------------------
#define MONGO_API_URL    "https://data.mongodb-api.com/app/data-XXXXX/endpoint/data/v1/action/insertOne"
#define MONGO_API_KEY    "SUA_API_KEY_AQUI"
#define MONGO_DATA_SOURCE "Cluster0"
#define MONGO_DATABASE   "industrial"
#define MONGO_COLLECTION "pesagens"

// Timeout das requisições HTTP (ms)
#define HTTP_TIMEOUT_MS    10000UL
// Máximo de retentativas em caso de falha
#define HTTP_MAX_RETRIES   3
// Base do backoff exponencial (ms) — 1s, 2s, 4s
#define HTTP_RETRY_BASE_MS 1000UL

// -----------------------------------------------------------------------------
// Identificação do dispositivo
// -----------------------------------------------------------------------------
#define DEVICE_ID        "ESP32_BALANCA_01"
#define DEVICE_LOCATION  "LINHA_PRODUCAO_A"

// -----------------------------------------------------------------------------
// Serial RS232 — UART2 da ESP32
// -----------------------------------------------------------------------------
// GPIO16 = RX2 (recebe dados da balança via MAX232)
// GPIO17 = TX2 (envia dados para a balança, se necessário)
#define SCALE_UART_NUM   UART_NUM_2     // equivale a Serial2 no framework Arduino
#define SCALE_BAUD_RATE  9600
#define SCALE_RX_PIN     16
#define SCALE_TX_PIN     17

// Timeout de leitura serial: se não chegar STX em X ms, descarta buffer
#define SCALE_READ_TIMEOUT_MS   2000UL
// Intervalo mínimo entre leituras distintas (debounce) — evita enviar o mesmo peso 100x
#define SCALE_DEBOUNCE_MS       500UL
// Variação mínima de peso para considerar nova leitura (kg) — filtra ruído
#define SCALE_MIN_DELTA_KG      0.010f
// Tamanho máximo do buffer de recepção serial
#define SCALE_BUFFER_SIZE       64
// Máximo de bytes descartados antes de forçar flush completo do buffer
#define SCALE_MAX_GARBAGE_BYTES 200

// -----------------------------------------------------------------------------
// Watchdog lógico
// -----------------------------------------------------------------------------
// Se o loop principal não der sinal de vida em X ms, o watchdog lógico reseta
#define WATCHDOG_TIMEOUT_MS  30000UL

// -----------------------------------------------------------------------------
// Fila offline (cache local quando MongoDB indisponível)
// -----------------------------------------------------------------------------
// Máximo de leituras enfileiradas na NVS enquanto sem conectividade
#define OFFLINE_QUEUE_MAX_SIZE  5

// -----------------------------------------------------------------------------
// Debug / logs
// A macro LOG_TAG_ é prefixo para todos os prints deste projeto.
// Definir DEBUG_VERBOSE para saída mais detalhada.
// -----------------------------------------------------------------------------
// #define DEBUG_VERBOSE   // Descomente para logs verbosos em desenvolvimento
