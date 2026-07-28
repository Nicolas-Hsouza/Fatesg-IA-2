# 📊 ESP32 + Balança Prix 3 Fit + MongoDB Atlas

Sistema IoT de captura de peso em tempo real com armazenamento em nuvem.

## ✨ Características

- ✅ **Leitura RS232** da Balança Prix 3 Fit via UART2 (GPIO16/17)
- ✅ **Conexão Wi-Fi** com reconexão automática e backoff exponencial
- ✅ **Envio HTTPS** para MongoDB Atlas via Data API
- ✅ **Fila Offline** — armazena até 5 pesos localmente quando sem Wi-Fi
- ✅ **Retry Automático** com backoff (1s → 2s → 4s)
- ✅ **Watchdog Lógico** — reseta se travar
- ✅ **Debounce** — ignora leituras repetidas e ruído

## 🚀 Quick Start (5 minutos)

### 1. Crie um cluster MongoDB

Abra [cloud.mongodb.com](https://cloud.mongodb.com) e:
- Registre com Google/GitHub
- Crie cluster M0 (gratuito)
- Habilite Data API em `Tools` → `HTTPS Endpoint`
- Gere chave em `API Keys`
- Autorize IP em `Security` → `Network Access` → `0.0.0.0/0`

**Copie e salve:**
- URL do endpoint (termina com `/endpoint/data/v1`)
- Chave API

### 2. Configure o ESP32

Abra `config.h` e substitua:

```cpp
#define WIFI_SSID       "Seu_WiFi"
#define WIFI_PASSWORD   "sua_senha"

// URL que você copiou + "/action/insertOne"
#define MONGO_API_URL    "https://data.mongodb-api.com/app/data-XXXXX/endpoint/data/v1/action/insertOne"

// Chave que você copiou
#define MONGO_API_KEY    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
```

### 3. Compile e envie

```bash
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

### 4. Teste

- Coloque peso na balança
- Verifique logs: `[Mongo] HTTP 201` = sucesso ✅
- Veja dados em: [cloud.mongodb.com](https://cloud.mongodb.com) → Collections → industrial.pesagens

## 📚 Documentação

| Arquivo | Descrição |
|---------|-----------|
| **[QUICK_START.md](./QUICK_START.md)** | 📖 Guia visual passo a passo (RECOMENDADO PARA INICIANTES) |
| **[MONGODB_SETUP.md](./MONGODB_SETUP.md)** | 🛠️ Setup detalhado do MongoDB + troubleshooting |
| **config.h** | ⚙️ Configurações do projeto (Wi-Fi, MongoDB, Serial) |
| **config_example.h** | 📋 Exemplo comentado com todos os parâmetros |

## 📁 Arquitetura do Código

```
┌─────────────────────────────────────────────────────┐
│                    main.cpp                         │
│           (Loop principal / Watchdog)               │
└──────────────────┬──────────────────────────────────┘
                   │
        ┌──────────┼──────────┐
        ▼          ▼          ▼
   ScaleManager  WifiManager  MongoService
   (RS232 RX)   (Wi-Fi STA)   (HTTPS POST)
   
   Protocolo:
   Balança → MAX232 → UART2 (GPIO16/17)
                        ↓
                   Parser serial
                   (STX...ETX)
                        ↓
                   ScaleReading
                   {weight_kg, status}
                        ↓
                   MongoService::send()
                        ↓
                   HTTP POST (JSON)
                        ↓
                   MongoDB Atlas (Cloud)
```

## 🔧 Configurações Importantes

### config.h (você precisa editar)

```cpp
// Wi-Fi
#define WIFI_SSID           "SUA_REDE"
#define WIFI_PASSWORD       "SUA_SENHA"

// MongoDB (vide QUICK_START.md para obter estes)
#define MONGO_API_URL       "https://data.mongodb-api.com/app/data-XXXXX/..."
#define MONGO_API_KEY       "eyJhbGc..."

// Timeouts
#define WIFI_CONNECT_TIMEOUT_MS     15000UL  // Aguarda conexão por 15s
#define HTTP_TIMEOUT_MS             10000UL  // Timeout HTTPS por 10s
#define SCALE_READ_TIMEOUT_MS       2000UL   // Timeout serial por 2s

// Filtros
#define SCALE_DEBOUNCE_MS           500UL    // Espera 500ms entre leituras
#define SCALE_MIN_DELTA_KG          0.010f   // Ignora mudanças < 10g
```

## 📊 Formato dos Dados no MongoDB

Cada peso enviado gera um documento como este:

```json
{
  "_id": ObjectId("..."),
  "device_id": "ESP32_BALANCA_01",
  "location": "LINHA_PRODUCAO_A",
  "peso": 1.350,
  "unidade": "kg",
  "status": "stable",
  "timestamp": "2024-12-21T15:30:45Z",
  "uptime_s": 125,
  "rssi_dbm": -50,
  "fw_version": "1.0.0"
}
```

## 🔌 Pinagem ESP32

```
ESP32 DevKit V1
┌─────────────────────────────────────────┐
│  Balança (MAX232) → UART2               │
│  GND → GND                              │
│  RX (MAX232 OUT) → GPIO16 (UART2 RX)   │
│  TX (MAX232 IN)  → GPIO17 (UART2 TX)   │
└─────────────────────────────────────────┘

Baud Rate: 9600 bps
Protocolo: 8N1 (8 bits, sem paridade, 1 stop bit)
```

## 📈 Recursos Utilizados

```
RAM: ~50 KB (sem fila offline) / ~80 KB (fila cheia)
Flash: ~300 KB (firmware)
Wi-Fi: 20-50 mA durante transmissão
Serial: 9600 bps (baixa velocidade, robusta)
HTTPS: TLS 1.2 + certificado CA DigiCert (embutido)
```

## ⚠️ Troubleshooting Rápido

| Sintoma | Verificar |
|---------|-----------|
| Sem logs no monitor | `pio device monitor -b 115200` |
| Wi-Fi não conecta | `config.h`: SSID/senha corretos? |
| HTTP 401/403 | MongoDB: chave API expirou? IP autorizado? |
| Timeout HTTPS | Wi-Fi conectado? MongoDB em pé? |
| Sem peso na balança | Verifique UART2 (GPIO16/17) e conexão MAX232 |

👉 **Veja [MONGODB_SETUP.md](./MONGODB_SETUP.md)** para troubleshooting completo

## 🧪 Testar Localmente (Python)

```bash
pip3 install pymongo
python3 test_mongodb.py
```

Isso lista os últimos 5 pesos e estatísticas.

## 📦 Dependências (PlatformIO)

```ini
platform  = espressif32
board     = esp32dev
framework = arduino
lib_deps  = bblanchon/ArduinoJson @ ^7.2.0
```

Já inclusos no ESP32 Arduino Core:
- WiFiClientSecure (HTTPS)
- HTTPClient (POST/GET)
- HardwareSerial (UART)

## 🔐 Segurança

- ✅ Certificado CA validado (DigiCert Global Root G2)
- ✅ HTTPS TLS 1.2
- ✅ API Key criptografada em trânsito
- ⚠️ **NÃO commite** `config.h` com credenciais no Git
- ⚠️ Use `.gitignore`: `config.h`

## 📋 Checklist de Deploy

- [ ] MongoDB Atlas cluster criado
- [ ] Data API habilitada e testada
- [ ] `config.h` preenchido com valores reais
- [ ] Wi-Fi testado no ESP32
- [ ] Serial da balança verificada (9600 bps)
- [ ] Primeiro peso enviado com sucesso (HTTP 201)
- [ ] Documentos visíveis em cloud.mongodb.com
- [ ] Fila offline testada (desconectar Wi-Fi e testar)

## 🚀 Próximos Passos

1. **Múltiplos ESP32**: Mude `DEVICE_ID` para cada unidade
2. **Dashboard**: Integre com Grafana, Tableau ou seu próprio frontend
3. **Alertas**: Configure triggers no MongoDB ou na sua aplicação
4. **OTA Updates**: Implemente atualização via Wi-Fi
5. **Armazenamento Local**: Use NVS (Preferences) para cache mais robusto

## 📞 Suporte

- Documentação: [QUICK_START.md](./QUICK_START.md) e [MONGODB_SETUP.md](./MONGODB_SETUP.md)
- Logs: `pio device monitor -b 115200`
- MongoDB: [cloud.mongodb.com](https://cloud.mongodb.com) → Support

---

**Versão:** 1.0.0  
**Última atualização:** 2024-12-21  
**Status:** ✅ Produção
