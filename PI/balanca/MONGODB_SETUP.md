# Guia: Configurar MongoDB Atlas para o Projeto ESP32

## Passo 1: Criar Conta MongoDB Atlas

1. Acesse [cloud.mongodb.com](https://cloud.mongodb.com)
2. Clique em **"Register"** ou faça login se já tiver conta
3. Complete o cadastro com seu email
4. Escolha **"Serverless"** ou **"M0 Free"** (cluster gratuito)

## Passo 2: Criar o Banco e Coleção

Após criar o cluster, acesse o painel:

1. Clique em **"+ Create Database"** (ou "Collections" se o cluster já existe)
2. Crie um banco com:
   - **Database name**: `industrial`
   - **Collection name**: `pesagens`

## Passo 3: Habilitar MongoDB Data API

1. No menu lateral, vá para **"Tools"** → **"Data API"** (ou procure por "HTTPS Endpoint")
2. Clique em **"Enable HTTPS Endpoint"**
3. Será gerada uma URL base como:
   ```
   https://data.mongodb-api.com/app/data-XXXXXXXXXXXXXXXXXXXXXXXX/endpoint/data/v1
   ```
4. **Copie a URL completa** (você usará dela em poucos passos)

## Passo 4: Gerar Chave API

1. Ainda em **"Data API"**, procure por **"API Keys"** ou **"Authentication"**
2. Clique em **"Create API Key"** (ou **"+ New Key"**)
3. Copie a chave gerada (será algo como `eyJhbGc...`)

## Passo 5: Adicionar IP à Lista de Acesso

1. No menu lateral, clique em **"Security"** → **"Network Access"**
2. Clique em **"+ Add IP Address"**
3. Escolha uma opção:
   - **Teste rápido**: digite `0.0.0.0/0` (aceita qualquer IP - menos seguro)
   - **Produção**: adicione o IP público do seu ESP32 ou roteador

## Passo 6: Configurar o Projeto

Abra o arquivo `config.h` do seu projeto e substitua:

```cpp
// Copie a URL base da Data API e adicione "/action/insertOne" no final
#define MONGO_API_URL    "https://data.mongodb-api.com/app/data-XXXXXXXXXXXXXXXXXXXXXXXX/endpoint/data/v1/action/insertOne"

// Cole a chave API aqui
#define MONGO_API_KEY    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."

// Estes já estão corretos, não altere
#define MONGO_DATA_SOURCE "Cluster0"
#define MONGO_DATABASE   "industrial"
#define MONGO_COLLECTION "pesagens"

// Configure seu Wi-Fi
#define WIFI_SSID       "NOME_DA_REDE"
#define WIFI_PASSWORD   "SENHA_WIFI"
```

## Passo 7: Compilar e Enviar

```bash
# Compilar e enviar para o ESP32
pio run -e esp32dev -t upload

# Monitorar logs
pio device monitor -b 115200
```

## Passo 8: Testar

1. Coloque um peso na balança Prix 3 Fit
2. Nos logs do monitor serial, você deve ver:
   ```
   [Mongo] Documento enviado com sucesso. Peso: 1.350 kg | HTTP 201
   ```
3. Acesse seu painel MongoDB e vá para **Collections** → **industrial.pesagens**
4. Você verá um documento como:
   ```json
   {
     "_id": "...",
     "device_id": "ESP32_BALANCA_01",
     "location": "LINHA_PRODUCAO_A",
     "peso": 1.35,
     "unidade": "kg",
     "status": "stable",
     "timestamp": "2024-12-21T10:30:00Z",
     "uptime_s": 3600,
     "rssi_dbm": -65,
     "fw_version": "1.0.0"
   }
   ```

## Troubleshooting

| Problema | Causa | Solução |
|----------|-------|---------|
| **HTTP 400** | URL ou chave inválida | Verifique em `config.h` |
| **HTTP 401** | Chave API expirou ou está errada | Regenere a chave no MongoDB |
| **HTTP 403** | IP não autorizado | Adicione IP em "Network Access" |
| **Timeout** | Sem conexão Wi-Fi | Verifique SSID/senha em `config.h` |
| **HTTP -1** (erro de rede) | Certificado SSL inválido | Geralmente não aplicável (CA bundle está embutido) |

## Documentação Oficial

- [MongoDB Data API](https://www.mongodb.com/docs/atlas/app-services/data-api/)
- [ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/)
- [Seu projeto - config.h](./config.h)
