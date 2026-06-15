# 📦 Resumo: Banco MongoDB Conectado ao Seu Projeto ESP32

## ✅ O Que Foi Criado

Seu projeto agora tem **5 novos arquivos de documentação** + **1 script Python** para testar + **1 script Bash** para verificação:

### 📄 Documentação

| Arquivo | Propósito | Para Quem |
|---------|-----------|----------|
| **[QUICK_START.md](./QUICK_START.md)** | 👈 **COMECE AQUI** — Guia visual passo a passo com screenshots | Iniciantes |
| **[MONGODB_SETUP.md](./MONGODB_SETUP.md)** | Setup detalhado + troubleshooting completo | Todos |
| **[README_MONGODB.md](./README_MONGODB.md)** | Visão geral do projeto e arquitetura | Desenvolvedores |
| **[config_example.h](./config_example.h)** | Exemplo comentado de todas as configurações | Referência |

### 🔧 Ferramentas

| Arquivo | Função | Como usar |
|---------|--------|----------|
| **[config.h](./config.h)** | ⚙️ Configurações principais (EDITE AQUI) | Abra e preencha com seus dados |
| **[test_mongodb.py](./test_mongodb.py)** | 🧪 Testa conexão com MongoDB | `python3 test_mongodb.py` |
| **[verify_setup.sh](./verify_setup.sh)** | ✓ Verifica se tudo está pronto | `bash verify_setup.sh` |

---

## 🚀 Como Começar (3 Passos)

### 1️⃣ **Leia o Guia Rápido**

Abra e siga: [QUICK_START.md](./QUICK_START.md)

⏱️ Tempo: **5-10 minutos**

### 2️⃣ **Configure o Projeto**

Edite `config.h` com:
- Seu SSID Wi-Fi
- Sua senha Wi-Fi
- URL do MongoDB (que você copiou)
- Chave API (que você copiou)

⏱️ Tempo: **2 minutos**

### 3️⃣ **Envie para o ESP32**

```bash
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

⏱️ Tempo: **3 minutos**

---

## 🎯 O Código Já Está Pronto!

✅ **Você NÃO precisa modificar:**

- `MongoService.cpp/h` — Envio HTTPS + retry + fila offline
- `WifiManager.cpp/h` — Reconexão automática
- `ScaleManager.cpp/h` — Parser RS232
- `main.cpp` — Loop principal com watchdog

**Tudo funciona assim que você preencher as credenciais em `config.h`.**

---

## 📊 O Que Acontece Quando Você Envia

1. **ESP32 lê** peso da balança via Serial (UART2)
2. **Parser** valida o pacote (STX...ETX)
3. **Debounce** filtra leituras repetidas/ruído
4. **Envio** monta JSON e POST para MongoDB
5. **Retry** com backoff se falhar (1s, 2s, 4s)
6. **Fila Offline** armazena até 5 pesos se sem Wi-Fi
7. **Flush** automático quando reconecta
8. **MongoDB** recebe e armazena documento

---

## 🔐 Dados Enviados

Cada peso resulta em um documento no MongoDB:

```json
{
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

---

## 📋 Checklist de Setup

- [ ] Li o arquivo **QUICK_START.md**
- [ ] Criei conta em **cloud.mongodb.com**
- [ ] Criei cluster **M0** (gratuito)
- [ ] Habilitei **Data API** em Tools
- [ ] Criei banco **industrial** e coleção **pesagens**
- [ ] Copiei **URL** da Data API
- [ ] Copiei **Chave API**
- [ ] Autorizei **IP 0.0.0.0/0** em Network Access
- [ ] Preenchi **config.h** com URL e chave
- [ ] Preenchi **config.h** com SSID/senha Wi-Fi
- [ ] Executei `pio run -e esp32dev -t upload`
- [ ] Verifiquei logs: `[Mongo] HTTP 201` ✅

---

## ❓ Dúvidas?

| Dúvida | Resposta |
|--------|---------|
| **"Como obter a URL da Data API?"** | Veja passo 4 em [QUICK_START.md](./QUICK_START.md) |
| **"Como gerar a chave API?"** | Veja passo 5 em [QUICK_START.md](./QUICK_START.md) |
| **"Meu ESP32 não conecta ao Wi-Fi"** | Verifique SSID/senha em `config.h` |
| **"HTTP 403 ao enviar para MongoDB"** | Adicione IP em Security → Network Access |
| **"Quero múltiplos ESP32"** | Mude `DEVICE_ID` em `config.h` para cada um |
| **"Posso consultar dados com Python?"** | Sim! Use `test_mongodb.py` |

Mais dúvidas? Leia [MONGODB_SETUP.md](./MONGODB_SETUP.md) — seção **Troubleshooting**.

---

## 📚 Arquivos Importantes

```
seu_projeto/
├── config.h                  ← EDITE AQUI (credenciais)
├── main.cpp                  ← Não altere
├── MongoService.cpp/h        ← Não altere
├── WifiManager.cpp/h         ← Não altere
├── ScaleManager.cpp/h        ← Não altere
├── QUICK_START.md            ← 👈 COMECE AQUI
├── MONGODB_SETUP.md          ← Guia detalhado
├── README_MONGODB.md         ← Visão geral
├── config_example.h          ← Referência
├── test_mongodb.py           ← Teste com Python
└── verify_setup.sh           ← Verificação rápida
```

---

## 🎓 Aprendizado

Seu projeto agora implementa:

✅ **Comunicação Serial** — Parser de máquina de estados (STX/ETX)  
✅ **Wi-Fi** — Reconexão automática com backoff exponencial  
✅ **HTTPS** — Certificado TLS com CA bundle  
✅ **JSON** — Serialização com ArduinoJson  
✅ **Retry Logic** — Backoff exponencial (1s, 2s, 4s)  
✅ **Offline Queue** — Cache local em RAM  
✅ **Watchdog** — Proteção contra travamentos  
✅ **Debounce** — Filtro de leituras repetidas  
✅ **Logging** — Saída diagnóstica via Serial  

---

## 🚀 Próximas Ideias

Agora que você tem dados no MongoDB, pode:

1. **Dashboard em Tempo Real** — Grafana, Tableau, PowerBI
2. **Alertas** — Se peso sair do esperado
3. **Análise** — Estatísticas, médias, tendências
4. **Múltiplos Sensores** — Múltiplos ESP32 com device_id único
5. **OTA Updates** — Atualizar firmware via Wi-Fi
6. **App Mobile** — Consumir dados da API

---

## 📞 Suporte

Se tiver problemas:

1. **Leia** [QUICK_START.md](./QUICK_START.md)
2. **Verifique** logs: `pio device monitor -b 115200`
3. **Consulte** [MONGODB_SETUP.md](./MONGODB_SETUP.md) → Troubleshooting
4. **Teste** Python: `python3 test_mongodb.py`

---

**🎉 Pronto!** Seu ESP32 está configurado para enviar dados ao MongoDB!

Próximo passo: Abra [QUICK_START.md](./QUICK_START.md) e comece. ⬅️

---

**v1.0** — 2024-12-21 — Status: ✅ Pronto para Usar
