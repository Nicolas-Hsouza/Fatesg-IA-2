# ESP32 + Balança Prix 3 Fit + MongoDB Atlas

Firmware industrial para integração de balança Toledo Prix 3 Fit com ESP32 via RS232/MAX232, enviando dados de pesagem para MongoDB Atlas.

---

## Estrutura do Projeto

```
esp32_balanca_prix3fit/
├── platformio.ini          ← Configuração de build e bibliotecas
├── include/
│   └── config.h            ← Credenciais e parâmetros (NÃO versionar!)
└── src/
    ├── main.cpp            ← Loop principal, watchdog, diagnóstico
    ├── ScaleManager.h/.cpp ← Parser RS232 com máquina de estados
    ├── WifiManager.h/.cpp  ← Wi-Fi com reconexão automática
    └── MongoService.h/.cpp ← HTTP POST para MongoDB Atlas Data API
```

---

## Instalação Passo a Passo

### 1. Instalar PlatformIO no VS Code

1. Instale o VS Code em https://code.visualstudio.com
2. Abra Extensions (Ctrl+Shift+X)
3. Busque "PlatformIO IDE" e instale
4. Reinicie o VS Code

### 2. Configurar Credenciais

Edite `include/config.h`:

```cpp
#define WIFI_SSID     "SUA_REDE"
#define WIFI_PASSWORD "SUA_SENHA"
#define MONGO_API_URL "https://data.mongodb-api.com/app/data-XXXXX/..."
#define MONGO_API_KEY "SUA_KEY"
```

### 3. Configurar MongoDB Atlas Data API

1. Acesse https://cloud.mongodb.com
2. No seu cluster: **Data API** → **Enable**
3. Crie uma chave de API com permissão de `insert`
4. Em **IP Access List**: adicione `0.0.0.0/0` para desenvolvimento
5. Copie a URL do endpoint para `MONGO_API_URL`

### 4. Compilar e Upload

```bash
# Via terminal PlatformIO
pio run -t upload

# Ou use o botão "→ Upload" na barra inferior do VS Code
```

### 5. Monitor Serial

```bash
pio device monitor --baud 115200 --filter colorize --filter time
```

---

## Conexão do Hardware

### MAX232 — Pinagem (DIP-16)

| Pino MAX232 | Função       | Conectar em           |
|-------------|--------------|----------------------|
| 1 (C1+)     | Cap+         | Cap 0.1µF para pino 3 |
| 2 (V+)      | +10V interno | Cap 1µF para GND      |
| 3 (C1-)     | Cap-         | Cap 0.1µF para pino 1 |
| 4 (C2+)     | Cap+         | Cap 0.1µF para pino 5 |
| 5 (C2-)     | Cap-         | Cap 0.1µF para pino 4 |
| 6 (V-)      | -10V interno | Cap 1µF para GND      |
| 11 (T1IN)   | TTL entrada  | GPIO17 (TX2) ESP32    |
| 12 (R1OUT)  | TTL saída    | GPIO16 (RX2) ESP32    |
| 13 (R1IN)   | RS232 entrada| Pino 2 (TXD) DB9      |
| 14 (T1OUT)  | RS232 saída  | Pino 3 (RXD) DB9      |
| 15 (GND)    | Terra        | GND comum             |
| 16 (VCC)    | Alimentação  | 5V (pino VIN do ESP32)|

### Conector DB9 — Balança Prix 3 Fit

| Pino DB9 | Sinal | Conectar em     |
|----------|-------|-----------------|
| 2        | TXD   | MAX232 pino 13  |
| 3        | RXD   | MAX232 pino 14  |
| 5        | GND   | GND comum       |
| 7        | RTS   | Curto com pino 8|
| 8        | CTS   | Curto com pino 7|

> **Nota:** Faça o loopback RTS↔CTS no DB9 se a balança exigir handshake de hardware.

---

## Testando a Comunicação

### Verificar se a balança está enviando dados

No monitor serial, você deve ver:

```
[Scale] UART2 inicializada: RX=GPIO16 TX=GPIO17 @ 9600 bps
[Scale] Aguardando dados da balança Prix 3 Fit...
[WiFi] Conectando a 'MINHA_REDE'...
[WiFi] === CONECTADO ===
[WiFi]   IP: 192.168.1.100
[Scale] Peso estável: 1.350 kg | Enviando...
[Mongo] Documento enviado com sucesso. Peso: 1.350 kg | HTTP 201
```

### Depurando ausência de dados da balança

1. **Sem nenhuma mensagem `[Scale]`**: A balança não está enviando ou a fiação está errada
   - Verifique tensão no pino 2 do DB9 (deve ter atividade serial em nível RS232: ±10V)
   - Verifique a saída TTL do MAX232 pino 12 (deve ter sinal 0-3.3V)

2. **Mensagens de bytes inválidos**: Baud rate errado
   - Tente 4800, 19200, ou 2400 em `SCALE_BAUD_RATE`

3. **Pacotes inválidos constantes**: Problema de GND
   - Verifique se GND da balança, MAX232 e ESP32 estão conectados

### Habilitar logs verbosos

Em `config.h`, descomente:
```cpp
#define DEBUG_VERBOSE
```

Isso mostra o conteúdo bruto de cada byte recebido.

---

## Payload Enviado ao MongoDB (exemplo real)

```json
{
  "dataSource": "Cluster0",
  "database": "industrial",
  "collection": "pesagens",
  "document": {
    "device_id":  "ESP32_BALANCA_01",
    "location":   "LINHA_PRODUCAO_A",
    "peso":       1.350,
    "unidade":    "kg",
    "status":     "stable",
    "timestamp":  "2026-05-21T10:30:00Z",
    "uptime_s":   3600,
    "rssi_dbm":   -65,
    "fw_version": "1.0.0"
  }
}
```

---

## Troubleshooting

| Sintoma | Causa provável | Solução |
|---------|----------------|---------|
| ESP32 reinicia em loop | Crash no setup / Wi-Fi não inicializa | Verifique credenciais em config.h |
| `[Mongo] HTTP -1` | Sem rede ou URL errada | Verifique MONGO_API_URL |
| `[Mongo] HTTP 401` | API Key inválida | Regenere a chave no Atlas |
| `[Scale] Timeout de leitura` | Balança não envio STX | Verifique baud rate e fiação |
| `[Scale] Bytes inválidos` | Baud rate errado ou GND flutuante | Teste outros baud rates |
| Heap decresce continuamente | Vazamento de memória | Verifique se HTTPClient.end() está sendo chamado |
| Watchdog dispara | Bloqueio em loop | Não adicione delay() > 100ms |

---

## Melhorias Futuras Recomendadas

### OTA Update
```cpp
// Em setup(), após Wi-Fi conectado:
ArduinoOTA.setHostname(DEVICE_ID);
ArduinoOTA.setPassword("senha_ota");
ArduinoOTA.begin();

// Em loop():
ArduinoOTA.handle();
```

### NVS para fila offline persistente
Substituir o array `OfflineItem _queue[]` por `Preferences` (NVS) para sobreviver a reinicializações.

### Dual-core: Serial em Core 1, HTTP em Core 0
```cpp
xTaskCreatePinnedToCore(scaleTask, "Scale", 4096, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(mongoTask, "Mongo", 8192, NULL, 1, NULL, 0);
```

### MQTT como alternativa ao HTTP
Para latência menor e menor overhead de conexão, use `PubSubClient` + MQTT Broker (Mosquitto / HiveMQ).

### Dashboard local
Servidor HTTP mínimo com `AsyncWebServer` para visualização em tempo real via navegador.
