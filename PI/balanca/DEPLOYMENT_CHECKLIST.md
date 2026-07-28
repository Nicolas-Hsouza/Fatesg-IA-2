# ✅ Checklist Final — Pronto para Enviar para o ESP32

## 📋 Antes de Começar

- Ler **QUICK_START.md** completamente
- Entender a arquitetura em **ARCHITECTURE.txt**
- Ter acesso a cloud.mongodb.com
- ESP32 conectado via USB
- PlatformIO CLI instalado (`pio --version`)

---

## 🌐 Configuração MongoDB Atlas

- Criar conta em cloud.mongodb.com (com Google/GitHub)
- Criar um projeto (ou usar existente)
- Criar cluster M0 (gratuito)
- Aguardar cluster ficar pronto (~5 min)
- Criar banco de dados: **industrial**
- Criar coleção: **pesagens**
- Habilitar Data API em Tools → HTTPS Endpoint
- Copiar URL da Data API (termina com `/endpoint/data/v1`)
- Criar API Key em API Keys
- Copiar a chave API (começa com `eyJhbGc...`)
- Adicionar IP em Security → Network Access → `0.0.0.0/0`

---

## ⚙️ Configuração do ESP32

- Abrir `config.h` no editor
- Preencher `WIFI_SSID` com seu SSID (ex: "Meu_WiFi_5G")
- Preencher `WIFI_PASSWORD` com sua senha
- Preencher `MONGO_API_URL` com URL copiada + `/action/insertOne`
- Preencher `MONGO_API_KEY` com a chave copiada
- Verificar que `MONGO_DATABASE` = "industrial"
- Verificar que `MONGO_COLLECTION` = "pesagens"
- (Opcional) Mudar `DEVICE_ID` para um nome único (se múltiplos ESP32)

---

## 🔧 Verificação Rápida

```bash
bash verify_setup.sh
```

Todos devem estar OK

---

## 🚀 Compilação e Upload

```bash
# Limpar build anterior (recomendado)
pio run -e esp32dev -t clean

# Compilar e enviar para ESP32
pio run -e esp32dev -t upload
```

Verificar:
- Compilação SEM erros
- Upload completo (mensagem "Leaving... Hard resetting via RTS pin")
- Nenhuma falha

---

## 🔍 Teste nos Logs

```bash
# Monitorar em tempo real
pio device monitor -b 115200
```

Você deve ver (ordem esperada):

```
============================================
  ESP32 + Balança Prix 3 Fit + MongoDB
  Firmware v1.0.0
  Device ID: ESP32_BALANCA_01
  Chip: ESP32 | Rev: 1
  Flash: 4096 KB | Heap: XXXX bytes
============================================

[Main] Inicializando ScaleManager (UART2)...
[Main] Inicializando WifiManager...
[Main] Inicializando MongoService...
[Main] Setup concluído. Entrando no loop principal.
[Main] Aguardando dados da balança...

[WiFi] Módulo Wi-Fi inicializado. SSID: Seu_WiFi_5G
[WiFi] Conectando a 'Seu_WiFi_5G'...

(após ~5s)

[WiFi] ============== CONECTADO ==============
[WiFi]   SSID:    Seu_WiFi_5G
[WiFi]   IP:      192.168.X.X
[WiFi]   Gateway: 192.168.X.1
[WiFi]   RSSI:    -XX dBm
[WiFi]   MAC:     AA:BB:CC:DD:EE:FF

[Scale] UART2 inicializada: RX=GPIO16 TX=GPIO17 @ 9600 bps
[Scale] Aguardando dados da balança Prix 3 Fit...

[Mongo] Serviço inicializado. Endpoint: https://data.mongodb-api.com/app/data-...
```

---

## 📊 Teste de Funcionamento

- Colocar um peso na balança Prix 3 Fit
- Peso estabilizar
- Verificar logs para:
  ```
  [Scale] Peso estável: X.XXX kg | Enviando...
  [Mongo] HTTP 201 | Resposta: { "insertedId": "..." }
  [Mongo] Documento enviado com sucesso. Peso: X.XXX kg | HTTP 201
  ```

**Se não viu:** Verifique troubleshooting em [MONGODB_SETUP.md](./MONGODB_SETUP.md)

---

## 🌐 Verificação em cloud.mongodb.com

- Logado em cloud.mongodb.com
- Navegou para Databases → Collections → industrial → pesagens
- Clicou em "Filter" e pressionou Play (sem filtro)
- Viu meu documento com:
  - `peso`: seu valor (ex: 1.350)
  - `status`: "stable"
  - `device_id`: "ESP32_BALANCA_01"
  - `timestamp`: data/hora recente
  - `rssi_dbm`: valor negativo (ex: -50)

**Se não viu:** 
- Aguardar 30s após enviar (fila offline pode estar em flush)
- Verificar status HTTP nos logs (deve ser 201)
- Se HTTP 401/403, verifique [MONGODB_SETUP.md](./MONGODB_SETUP.md)

---

## 🧪 Teste Python (Opcional)

```bash
pip3 install pymongo
python3 test_mongodb.py
```

Deve conectar e listar últimos 5 documentos com estatísticas.

---

## 🔐 Segurança (Antes de Produção)

- NÃO commitar `config.h` com credenciais no Git
- Adicionar `config.h` ao `.gitignore`
- Testar reconexão Wi-Fi (reiniciar roteador)
- Testar fila offline (desligar Wi-Fi por 2 min, ligar de novo)
- Verificar que dados offline foram sincronizados

---

## 📈 Teste de Estresse (Opcional)

- Colocar vários pesos consecutivos na balança
- Verificar que cada peso gerou um documento no MongoDB
- Contadores em `[=== DIAGNÓSTICO ===]` aumentam corretamente:
  - `Pesos enviados`
  - `Erros de envio` (deve permanecer baixo)
  - `Fila offline` (deve estar vazia se Wi-Fi Ok)

---

## ✅ Status Final

Se TODOS os pontos acima foram concluídos:

### 🎉 Parabéns! Seu Sistema Está Pronto!

**Próximos passos:**

1. **Monitoramento contínuo**: Execute `pio device monitor` conforme necessário
2. **Dashboard**: Crie um com Grafana/Tableau apontando para MongoDB
3. **Alertas**: Configure alertas se peso sair do esperado
4. **Múltiplos ESP32**: Duplique o projeto, mude `DEVICE_ID` para cada um
5. **Backup**: Configure backup automático no MongoDB Atlas

---

## 🆘 Se Algo Falhar

| Problema | Ação |
|----------|------|
| **Logs não aparecem** | Execute `pio device monitor -b 115200` |
| **HTTP 401** | Regenere API Key em cloud.mongodb.com |
| **HTTP 403** | Adicione `0.0.0.0/0` em Security → Network Access |
| **Wi-Fi não conecta** | Verifique SSID/senha em `config.h` |
| **Sem peso na balança** | Verifique conexão MAX232 nos GPIO16/17 |
| **JSON inválido** | Verifique aspas/escapes em `config.h` |

Mais ajuda: [MONGODB_SETUP.md](./MONGODB_SETUP.md) → Troubleshooting

---

**Data de Conclusão**: ________________  
**Testado por**: ________________  
**Versão do Firmware**: 1.0.0  
**Status**: ✅ Pronto para Produção
