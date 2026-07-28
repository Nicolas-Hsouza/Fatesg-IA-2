// =============================================================================
// config.h — Configurações centralizadas do projeto
//
// ATENÇÃO: Em produção, NÃO versionar este arquivo no Git.
// Adicione "config.h" ao seu .gitignore.
// Use variáveis de ambiente do PlatformIO para substituição em CI/CD.
//
// PRIMEIRO PASSO: Siga as instruções em MONGODB_SETUP.md
// =============================================================================
#pragma once

// =============================================================================
// SEÇÃO 1: CREDENCIAIS WI-FI — CONFIGURE AQUI
// =============================================================================
#define WIFI_SSID       "SUA_REDE_AQUI"
#define WIFI_PASSWORD   "SUA_SENHA_AQUI"

// Tempo máximo aguardando conexão Wi-Fi (ms)
#define WIFI_CONNECT_TIMEOUT_MS     15000UL
// Intervalo de tentativa de reconexão após falha (ms)
#define WIFI_RECONNECT_INTERVAL_MS  10000UL
// Intervalo de heartbeat para verificar conexão (ms)
#define WIFI_HEARTBEAT_INTERVAL_MS  30000UL

// =============================================================================
// SEÇÃO 2: CREDENCIAIS MONGODB ATLAS — CONFIGURE AQUI
// =============================================================================
// Instruções detalhadas: veja MONGODB_SETUP.md
//
// Passos:
//   1. Crie conta em cloud.mongodb.com
//   2. Crie cluster M0 (gratuito)
//   3. Crie banco "industrial" e coleção "pesagens"
//   4. Habilite Data API em Tools → HTTPS Endpoint
//   5. Copie a URL base (terminada em /endpoint/data/v1)
//   6. Gere chave em API Keys
//   7. Adicione IP em Security → Network Access (0.0.0.0/0 para teste)
//   8. Cole URL + "/action/insertOne" e chave abaixo
//
// Exemplos:
//   MONGO_API_URL:
//     https://data.mongodb-api.com/app/data-6234567890abcdef01234567/endpoint/data/v1/action/insertOne
//   MONGO_API_KEY:
//     eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiI2MjM0NTY3ODkwYWJjZGVm...
//
#define MONGO_API_URL    "https://data.mongodb-api.com/app/data-XXXXX/endpoint/data/v1/action/insertOne"
#define MONGO_API_KEY    "SUA_API_KEY_AQUI"

// Estes já estão corretos (não altere)
#define MONGO_DATA_SOURCE "Cluster0"
#define MONGO_DATABASE   "industrial"
#define MONGO_COLLECTION "pesagens"

// Timeout das requisições HTTP (ms)
#define HTTP_TIMEOUT_MS    10000UL
// Máximo de retentativas em caso de falha
#define HTTP_MAX_RETRIES   3
// Base do backoff exponencial (ms) — 1s, 2s, 4s, 8s...
#define HTTP_RETRY_BASE_MS 1000UL

// =============================================================================
// SEÇÃO 3: IDENTIFICAÇÃO DO DISPOSITIVO
// =============================================================================
// Útil se você tem múltiplos ESP32 em produção
#define DEVICE_ID        "ESP32_BALANCA_01"
#define DEVICE_LOCATION  "LINHA_PRODUCAO_A"

// =============================================================================
// SEÇÃO 4: COMUNICAÇÃO COM BALANÇA (Serial RS232)
// =============================================================================
// ESP32 UART2: RX=GPIO16, TX=GPIO17 (conectar ao MAX232)
#define SCALE_UART_NUM   UART_NUM_2
#define SCALE_BAUD_RATE  9600           // Balança Prix 3 Fit padrão
#define SCALE_RX_PIN     16              // GPIO16 = RX2
#define SCALE_TX_PIN     17              // GPIO17 = TX2

// Timeout de leitura serial (ms)
// Se não chegar STX em X ms, descarta buffer e reinicia
#define SCALE_READ_TIMEOUT_MS   2000UL

// Debounce temporal (ms)
// Intervalo mínimo entre duas leituras distintas
// Evita enviar "1.350 kg" 50 vezes se a balança ficar estável
#define SCALE_DEBOUNCE_MS       500UL

// Variação mínima de peso (kg) para considerar nova leitura
// Se o peso muda menos que isso, é ignorado (filtra ruído analógico)
#define SCALE_MIN_DELTA_KG      0.010f

// Tamanho do buffer de recepção serial (bytes)
#define SCALE_BUFFER_SIZE       64

// Máximo de bytes inválidos antes de forçar limpeza do buffer
#define SCALE_MAX_GARBAGE_BYTES 200

// =============================================================================
// SEÇÃO 5: WATCHDOG LÓGICO (Proteção contra travamentos)
// =============================================================================
// Se o loop principal não der sinal de vida em X ms, o ESP32 reseta
// Evita que o dispositivo fique preso esperando Wi-Fi/balança
#define WATCHDOG_TIMEOUT_MS  30000UL

// =============================================================================
// SEÇÃO 6: FILA OFFLINE (Cache local sem Wi-Fi)
// =============================================================================
// Quando sem Wi-Fi, armazena até N leituras localmente em RAM
// Quando reconecta ao Wi-Fi, envia tudo para MongoDB
// Max = 5 leituras (≈ 80 bytes de RAM, ≈ 2-3 minutos de dados)
#define OFFLINE_QUEUE_MAX_SIZE  5

// =============================================================================
// SEÇÃO 7: DEBUG / LOGS (Desenvolvimento)
// =============================================================================
// Descomente para ativar logs verbosos
// Produção: mantenha comentado para economizar memória
// #define DEBUG_VERBOSE
