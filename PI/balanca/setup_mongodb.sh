#!/bin/bash
# ============================================================================
# Script: Configuração Rápida do MongoDB para ESP32 + Balança
# ============================================================================
# Este script ajuda a configurar o ambiente para conectar seu ESP32 ao MongoDB
# 
# Requisitos:
#   - Conta Google ou GitHub (para MongoDB Atlas)
#   - Navegador web
#   - Terminal bash
# ============================================================================

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

clear

echo -e "${BLUE}"
echo "╔════════════════════════════════════════════════════════════════════════════╗"
echo "║              SETUP: MongoDB Atlas para ESP32 Balança                       ║"
echo "╚════════════════════════════════════════════════════════════════════════════╝"
echo -e "${NC}"

# Passo 1: Verificar arquivos
echo -e "\n${YELLOW}[1/4] Verificando arquivos do projeto...${NC}"
if [ ! -f "config.h" ]; then
    echo -e "${RED}[✗] config.h não encontrado!${NC}"
    exit 1
fi
echo -e "${GREEN}[✓] Arquivos encontrados${NC}"

# Passo 2: Criar conta MongoDB
echo -e "\n${YELLOW}[2/4] Informações para criar conta MongoDB${NC}"
echo -e "\nAbra seu navegador em: ${BLUE}https://cloud.mongodb.com${NC}"
echo -e "  1. Clique em 'Register' ou faça login"
echo -e "  2. Complete o cadastro (use email do seu projeto)"
echo -e "  3. Crie um projeto novo"
echo -e "  4. Clique 'Create a Database' → M0 Free"
echo -e "\nPressione ENTER quando tiver criado o cluster..."
read

# Passo 3: Configurar banco de dados
echo -e "\n${YELLOW}[3/4] Criar banco de dados${NC}"
echo -e "\nNo painel MongoDB, vá para 'Database' → 'Collections'"
echo -e "Clique em '+ Create Database' e insira:"
echo -e "  Database name: ${BLUE}industrial${NC}"
echo -e "  Collection name: ${BLUE}pesagens${NC}"
echo -e "\nPressione ENTER quando tiver criado..."
read

# Passo 4: Obter credenciais
echo -e "\n${YELLOW}[4/4] Obter URL e Chave API${NC}"
echo -e "\nAgora você precisa de 2 valores:"
echo -e "\n${BLUE}Valor 1 - URL da Data API:${NC}"
echo -e "  • No menu lateral, clique em 'Tools' → 'Data API'"
echo -e "  • Clique em 'Enable HTTPS Endpoint'"
echo -e "  • Copie a URL (começará com https://data.mongodb-api.com)"
echo -e "\n${BLUE}Valor 2 - Chave API:${NC}"
echo -e "  • Em 'Data API', procure por 'API Keys'"
echo -e "  • Clique em 'Create API Key'"
echo -e "  • Copie a chave (começará com eyJhbGc...)"
echo -e "\n${BLUE}Valor 3 - Autorizar IP:${NC}"
echo -e "  • Clique em 'Security' → 'Network Access'"
echo -e "  • Clique em '+ Add IP Address'"
echo -e "  • Digite: ${YELLOW}0.0.0.0/0${NC} (para teste, em produção use seu IP)"

echo -e "\n\nPaste a URL da Data API (termine com ENTER):"
read -r MONGO_URL

echo -e "\nPaste a Chave API (termine com ENTER):"
read -r MONGO_KEY

# Validar entrada
if [ -z "$MONGO_URL" ] || [ -z "$MONGO_KEY" ]; then
    echo -e "${RED}[✗] URL ou chave vazias!${NC}"
    exit 1
fi

# Passo 5: Atualizar config.h
echo -e "\n${YELLOW}Atualizando config.h com suas credenciais...${NC}"

# Backup
cp config.h config.h.backup
echo -e "${GREEN}[✓] Backup criado: config.h.backup${NC}"

# Substituir no arquivo
# Precisamos fazer isso com sed de forma segura
sed -i.bak "s|https://data.mongodb-api.com/app/data-XXXXXXXXXXXXXXXXXXXXXXXX/endpoint/data/v1/action/insertOne|${MONGO_URL}/action/insertOne|g" config.h
sed -i.bak "s|eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9\.\.\.|${MONGO_KEY}|g" config.h

echo -e "${GREEN}[✓] config.h atualizado!${NC}"

# Passo 6: Pedir credenciais Wi-Fi
echo -e "\n${YELLOW}Agora configure seu Wi-Fi:${NC}"
echo -e "\nSSID (nome da rede Wi-Fi):"
read -r WIFI_SSID

echo -e "\nSenha do Wi-Fi:"
read -rs WIFI_PASSWORD

if [ -z "$WIFI_SSID" ] || [ -z "$WIFI_PASSWORD" ]; then
    echo -e "${RED}[✗] SSID ou senha vazios!${NC}"
    exit 1
fi

# Substituir Wi-Fi
sed -i.bak "s|\"SUA_REDE_AQUI\"|\"${WIFI_SSID}\"|g" config.h
sed -i.bak "s|\"SUA_SENHA_AQUI\"|\"${WIFI_PASSWORD}\"|g" config.h

echo -e "${GREEN}[✓] Wi-Fi configurado!${NC}"

# Passo 7: Resumo
echo -e "\n"
echo -e "${GREEN}╔════════════════════════════════════════════════════════════════════════════╗"
echo -e "║              CONFIGURAÇÃO CONCLUÍDA!                                  ║"
echo -e "╚════════════════════════════════════════════════════════════════════════════╝${NC}"

echo -e "\n${BLUE}Próximos passos:${NC}"
echo -e "  1. Abra a pasta do projeto no VS Code"
echo -e "  2. Execute: ${YELLOW}pio run -e esp32dev -t upload${NC}"
echo -e "  3. Monitore: ${YELLOW}pio device monitor -b 115200${NC}"
echo -e "  4. Coloque um peso na balança"
echo -e "  5. Verifique os dados em cloud.mongodb.com → Collections"

echo -e "\n${BLUE}Para testar a conexão MongoDB com Python:${NC}"
echo -e "  1. ${YELLOW}pip install pymongo${NC}"
echo -e "  2. ${YELLOW}python3 test_mongodb.py${NC}"

echo -e "\n${YELLOW}Dúvidas?${NC} Verifique MONGODB_SETUP.md para troubleshooting"
echo -e "\n"
