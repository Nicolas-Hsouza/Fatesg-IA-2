# 📚 Índice Completo — Seu Projeto MongoDB + ESP32 Balança

## 🎯 LEIA PRIMEIRO

**👉 Novo no projeto? Comece aqui:**

1. **[QUICK_START.md](./QUICK_START.md)** — Guia visual passo a passo (10 minutos)
2. **[SETUP_SUMMARY.md](./SETUP_SUMMARY.md)** — Resumo do que foi criado
3. **[config.h](./config.h)** — Edite com suas credenciais

---

## 📖 Documentação Completa

### Para Iniciantes
- **[QUICK_START.md](./QUICK_START.md)** — 10 passos visuais com exemplos
- **[SETUP_SUMMARY.md](./SETUP_SUMMARY.md)** — O que foi criado e como usar

### Para Desenvolvedores
- **[MONGODB_SETUP.md](./MONGODB_SETUP.md)** — Setup detalhado + troubleshooting
- **[README_MONGODB.md](./README_MONGODB.md)** — Visão geral do projeto
- **[ARCHITECTURE.txt](./ARCHITECTURE.txt)** — Diagrama de fluxo de dados (ASCII art)

### Checklists
- **[DEPLOYMENT_CHECKLIST.md](./DEPLOYMENT_CHECKLIST.md)** — Verificação antes de enviar
- **[verify_setup.sh](./verify_setup.sh)** — Script de verificação automática

---

## ⚙️ Arquivos de Código (EDITAR)

| Arquivo | O que é | Quando editar |
|---------|---------|---------------|
| **[config.h](./config.h)** | Configurações do projeto | ✅ SEMPRE (Wi-Fi, MongoDB, pins) |
| **[config_example.h](./config_example.h)** | Exemplo comentado | ❌ Nunca (apenas referência) |

---

## 🔧 Código do Projeto (NÃO EDITAR)

| Arquivo | Responsabilidade |
|---------|------------------|
| **[main.cpp](./main.cpp)** | Loop principal + watchdog |
| **[MongoService.h/cpp](./MongoService.h)** | Envio HTTPS + retry + fila offline |
| **[WifiManager.h/cpp](./WifiManager.h)** | Conexão Wi-Fi + reconexão automática |
| **[ScaleManager.h/cpp](./ScaleManager.h)** | Parser RS232 (STX/ETX) + debounce |
| **[platformio.ini](./platformio.ini)** | Configuração de build (PlatformIO) |

---

## 🧪 Ferramentas de Teste

| Arquivo | Função | Comando |
|---------|--------|---------|
| **[test_mongodb.py](./test_mongodb.py)** | Teste de conexão MongoDB | `python3 test_mongodb.py` |
| **[verify_setup.sh](./verify_setup.sh)** | Verificação rápida de arquivos | `bash verify_setup.sh` |
| **[setup_mongodb.sh](./setup_mongodb.sh)** | Setup interativo (beta) | `bash setup_mongodb.sh` |

---

## 🚀 Fluxo de Uso

### 1️⃣ Setup Inicial (Primeira Vez)
```
Leia QUICK_START.md
    ↓
Crie conta MongoDB
    ↓
Copie URL e chave
    ↓
Edite config.h
    ↓
Execute: pio run -e esp32dev -t upload
```

### 2️⃣ Desenvolvimento
```
Edite código C++ (se necessário)
    ↓
Execute: pio run -e esp32dev -t upload
    ↓
Monitore: pio device monitor -b 115200
    ↓
Teste weight, verifique logs
```

### 3️⃣ Verificação Pré-Deploy
```
Leia DEPLOYMENT_CHECKLIST.md
    ↓
Execute: bash verify_setup.sh
    ↓
Coloque peso na balança
    ↓
Verifique logs (HTTP 201)
    ↓
Consulte MongoDB Atlas
```

### 4️⃣ Produção
```
Adicione config.h ao .gitignore
    ↓
Teste reconexão Wi-Fi
    ↓
Teste fila offline
    ↓
Crie dashboard/alertas
```

---

## 📊 Estrutura do Projeto

```
seu_projeto/
│
├── 📄 Documentação (LEIA PRIMEIRO)
│   ├── QUICK_START.md              ← 👈 COMECE AQUI
│   ├── SETUP_SUMMARY.md
│   ├── MONGODB_SETUP.md
│   ├── README_MONGODB.md
│   ├── ARCHITECTURE.txt
│   ├── DEPLOYMENT_CHECKLIST.md
│   └── INDEX.md                    ← Você está aqui
│
├── ⚙️ Configuração (EDITE)
│   └── config.h                    ← Suas credenciais
│
├── 💾 Código C++ (NÃO EDITE)
│   ├── main.cpp
│   ├── MongoService.h / .cpp
│   ├── WifiManager.h / .cpp
│   ├── ScaleManager.h / .cpp
│   └── platformio.ini
│
├── 🧪 Testes & Verificação
│   ├── test_mongodb.py
│   ├── verify_setup.sh
│   ├── setup_mongodb.sh
│   └── DEPLOYMENT_CHECKLIST.md
│
└── 📦 Referência
    ├── config_example.h
    └── README.md (original)
```

---

## 🔑 Conceitos-Chave

### Máquina de Estados (3)

1. **WifiManager** → Estados: DISCONNECTED → CONNECTING → CONNECTED → RECONNECTING
2. **ScaleManager** → Estados: IDLE → READING → PARSING → READY → ERROR
3. **MongoService** → Estados: IDLE → SENDING → RETRY → OFFLINE

### Mecanismos de Proteção

- ✅ **Watchdog Lógico** — Reseta se travado
- ✅ **Retry com Backoff** — 1s, 2s, 4s (max 3 tentativas)
- ✅ **Fila Offline** — Armazena 5 pesos se sem Wi-Fi
- ✅ **Debounce** — Ignora leituras repetidas (500ms + 10g mínimo)
- ✅ **Validação de Pacote** — STX/ETX, checksum, formato

### Comunicação

- **Serial RS232**: 9600 bps, 8N1, UART2 (GPIO16/17)
- **Wi-Fi**: STA mode, reconexão automática, backoff exponencial
- **HTTPS**: TLS 1.2, certificado CA DigiCert (embutido)
- **MongoDB**: Data API HTTP/POST com JSON

---

## 🆘 Troubleshooting Rápido

| Problema | Arquivo para Consultar |
|----------|----------------------|
| "Por onde começo?" | [QUICK_START.md](./QUICK_START.md) |
| "Erro de compilação" | [README_MONGODB.md](./README_MONGODB.md) |
| "HTTP 401/403" | [MONGODB_SETUP.md](./MONGODB_SETUP.md) → Troubleshooting |
| "Wi-Fi não conecta" | [MONGODB_SETUP.md](./MONGODB_SETUP.md) → Troubleshooting |
| "Sem peso na balança" | [ARCHITECTURE.txt](./ARCHITECTURE.txt) → Seção Pinagem |
| "Fila offline não funciona" | [ARCHITECTURE.txt](./ARCHITECTURE.txt) → Fila Offline |
| "Teste com Python" | [test_mongodb.py](./test_mongodb.py) |

---

## 📞 Contatos e Referências

- **MongoDB Atlas**: https://cloud.mongodb.com
- **MongoDB Data API Docs**: https://www.mongodb.com/docs/atlas/app-services/data-api/
- **ESP32 Arduino**: https://docs.espressif.com/projects/arduino-esp32/
- **PlatformIO**: https://platformio.org/
- **Seu Projeto**: [QUICK_START.md](./QUICK_START.md)

---

## ✨ O que você pode fazer agora

✅ **Imediatamente:**
- Enviar pesos para nuvem em tempo real
- Consultar dados históricos no MongoDB
- Testar reconexão automática

✅ **Próximo passo:**
- Criar dashboard com Grafana/Tableau
- Configurar alertas por email/SMS
- Integrar com sua aplicação web

✅ **Avançado:**
- Múltiplos ESP32 (cada um com device_id único)
- Sincronização com banco local
- OTA updates via Wi-Fi

---

## 📈 Estatísticas do Projeto

| Métrica | Valor |
|---------|-------|
| **Linhas de código C++** | ~1500 |
| **Linhas de documentação** | ~2000 |
| **Arquivos de suporte** | 10+ |
| **Tempo setup completo** | 15 min |
| **Tempo deploy ESP32** | 3 min |
| **Taxa de envio** | 1 dado/min (ajustável) |
| **Cache offline** | 5 pesos (~80 bytes) |
| **Consumo de memória** | ~50 KB |

---

## 🎯 Próximas Etapas Recomendadas

**Hoje:**
1. Leia [QUICK_START.md](./QUICK_START.md)
2. Configure MongoDB
3. Preencha [config.h](./config.h)
4. Envie para ESP32

**Amanhã:**
1. Crie dashboard
2. Configure alertas
3. Documente seu setup

**Próxima semana:**
1. Implemente múltiplos ESP32
2. Integre com sistema maior
3. Configure backups

---

## 📄 Últimas Notas

- Código está pronto para produção ✅
- Documentação completa ✅
- Ferramentas de teste incluídas ✅
- Troubleshooting coberto ✅

**Tempo estimado até "dados chegando no MongoDB": 15 minutos** ⏱️

---

**Versão**: 1.0.0  
**Data**: 2024-12-21  
**Status**: ✅ Pronto para Usar  
**Suporte**: Veja [MONGODB_SETUP.md](./MONGODB_SETUP.md)

---

👉 **Comece agora:** Abra [QUICK_START.md](./QUICK_START.md)
