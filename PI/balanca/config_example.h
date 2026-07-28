// =============================================================================
// config.h — EXEMPLO COM VALORES FICTÍCIOS
//
// INSTRUÇÕES:
// 1. Siga os passos em MONGODB_SETUP.md para criar conta e obter credenciais
// 2. Substitua os valores abaixo pelos seus valores REAIS
// 3. NÃO commite este arquivo no Git (adicione ao .gitignore)
// =============================================================================
#pragma once

// =============================================================================
// SEÇÃO 1: CREDENCIAIS WI-FI (OBRIGATÓRIO)
// =============================================================================
// Substitua pelos dados da sua rede Wi-Fi
#define WIFI_SSID       "SUA_REDE_AQUI"
#define WIFI_PASSWORD   "SUA_SENHA_AQUI"

// Tempo máximo aguardando conexão Wi-Fi (ms)
#define WIFI_CONNECT_TIMEOUT_MS     15000UL
// Intervalo de tentativa de reconexão após falha (ms)
#define WIFI_RECONNECT_INTERVAL_MS  10000UL
// Intervalo de heartbeat para verificar conexão (ms)
#define WIFI_HEARTBEAT_INTERVAL_MS  30000UL

// =============================================================================
// SEÇÃO 2: CREDENCIAIS MONGODB ATLAS (OBRIGATÓRIO)
// =============================================================================
// Siga os passos em MONGODB_SETUP.md para obter estes valores

// URL completa do endpoint Data API com /action/insertOne no final
// Exemplo real:
// https://data.mongodb-api.com/app/data-6234567890abcdef01234567/endpoint/data/v1/action/insertOne
#define MONGO_API_URL    "https://data.mongodb-api.com/app/data-XXXXXXXXXXXXXXXXXXXXXXXX/endpoint/data/v1/action/insertOne"

// Chave API gerada no MongoDB Atlas
// Exemplo: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiI...
#define MONGO_API_KEY    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."

// Estes valores já devem estar corretos (não altere)
#define MONGO_DATA_SOURCE "Cluster0"
#define MONGO_DATABASE   "industrial"
#define MONGO_COLLECTION "pesagens"

// Timeout das requisições HTTP (ms)
#define HTTP_TIMEOUT_MS    10000UL
// Máximo de retentativas em caso de falha
#define HTTP_MAX_RETRIES   3
// Base do backoff exponencial (ms) — 1s, 2s, 4s
#define HTTP_RETRY_BASE_MS 1000UL

// =============================================================================
// SEÇÃO 3: IDENTIFICAÇÃO DO DISPOSITIVO (OPCIONAL)
// =============================================================================
// Útil se você tem múltiplos ESP32 enviando dados para o mesmo banco
#define DEVICE_ID        "ESP32_BALANCA_01"
#define DEVICE_LOCATION  "LINHA_PRODUCAO_A"

// =============================================================================
// SEÇÃO 4: CONFIGURAÇÕES DA SERIAL (UART2 - Balança)
// =============================================================================
// Estes pinos e velocidades são específicos da sua hardware (não altere)
#define SCALE_UART_NUM   UART_NUM_2     // UART2 da ESP32
#define SCALE_BAUD_RATE  9600           // Velocidade da balança Prix 3 Fit
#define SCALE_RX_PIN     16              // GPIO16 = RX2
#define SCALE_TX_PIN     17              // GPIO17 = TX2

// Timeout de leitura serial (ms) — se não chegar dados, descarta buffer
#define SCALE_READ_TIMEOUT_MS   2000UL

// Intervalo mínimo entre leituras distintas (debounce em ms)
// Evita enviar peso 1.350 kg 50 vezes se a balança ficar estável
#define SCALE_DEBOUNCE_MS       500UL

// Variação mínima de peso para considerar nova leitura (em kg)
// Se o peso mudar menos de 0.010 kg, é ignorado (ruído analógico)
#define SCALE_MIN_DELTA_KG      0.010f

// Tamanho máximo do buffer de recepção serial (em bytes)
#define SCALE_BUFFER_SIZE       64

// Máximo de bytes inválidos consecutivos antes de forçar limpeza
#define SCALE_MAX_GARBAGE_BYTES 200

// =============================================================================
// SEÇÃO 5: WATCHDOG LÓGICO (SEGURANÇA)
// =============================================================================
// Se o loop principal não der sinal de vida em X ms, o ESP32 reseta
// Evita que o dispositivo fique travado esperando Wi-Fi/balança
#define WATCHDOG_TIMEOUT_MS  30000UL

// =============================================================================
// SEÇÃO 6: FILA OFFLINE (CACHE LOCAL)
// =============================================================================
// Quando sem Wi-Fi, o ESP32 armazena até N leituras localmente
// Quando reconecta, envia tudo para o MongoDB
// Máximo = 5 leituras (aproximadamente 2-3 minutos de dados)
#define OFFLINE_QUEUE_MAX_SIZE  5

// =============================================================================
// SEÇÃO 7: DEBUG / LOGS (OPCIONAL)
// =============================================================================
// Descomente a linha abaixo para ativar logs verbosos durante desenvolvimento
// Produção: mantenha comentado para economizar memória
// #define DEBUG_VERBOSE
