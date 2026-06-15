#!/bin/bash
# Script de verificação rápida — rode antes de enviar para produção

echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║        VERIFICAÇÃO PRÉ-DEPLOY — MongoDB + ESP32 Balança           ║"
echo "╚════════════════════════════════════════════════════════════════════╝"

PASS=0
FAIL=0

check_file() {
    if [ -f "$1" ]; then
        echo "✅ $1 existe"
        ((PASS++))
    else
        echo "❌ $1 NÃO ENCONTRADO"
        ((FAIL++))
    fi
}

check_string() {
    if grep -q "$2" "$1" 2>/dev/null; then
        echo "✅ $1 contém '$2'"
        ((PASS++))
    else
        echo "❌ $1 NÃO contém '$2' (precisa ser configurado)"
        ((FAIL++))
    fi
}

echo ""
echo "1️⃣  Verificando arquivos do projeto..."
check_file "config.h"
check_file "MongoService.h"
check_file "MongoService.cpp"
check_file "WifiManager.h"
check_file "WifiManager.cpp"
check_file "ScaleManager.h"
check_file "ScaleManager.cpp"
check_file "main.cpp"
check_file "platformio.ini"

echo ""
echo "2️⃣  Verificando configurações em config.h..."
check_string "config.h" "WIFI_SSID"
check_string "config.h" "WIFI_PASSWORD"
check_string "config.h" "MONGO_API_URL"
check_string "config.h" "MONGO_API_KEY"

echo ""
echo "3️⃣  Verificando documentação..."
check_file "QUICK_START.md"
check_file "MONGODB_SETUP.md"
check_file "README_MONGODB.md"
check_file "config_example.h"
check_file "test_mongodb.py"

echo ""
echo "════════════════════════════════════════════════════════════════════"
echo "RESULTADO: $PASS✅ / $FAIL❌"
echo "════════════════════════════════════════════════════════════════════"

if [ $FAIL -eq 0 ]; then
    echo ""
    echo "🎉 Tudo pronto! Próximos passos:"
    echo ""
    echo "1. Abra QUICK_START.md e siga os passos"
    echo "2. Configure MongoDB Atlas (cloud.mongodb.com)"
    echo "3. Preencha config.h com seus valores reais"
    echo "4. Execute: pio run -e esp32dev -t upload"
    echo "5. Monitore: pio device monitor -b 115200"
    echo ""
else
    echo ""
    echo "⚠️  Alguns arquivos ou configurações estão faltando."
    echo "Verifique os items marcados com ❌ acima."
    echo ""
fi
