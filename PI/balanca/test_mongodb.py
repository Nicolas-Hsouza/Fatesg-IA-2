#!/usr/bin/env python3
"""
Script de teste: conectar ao MongoDB e consultar dados da balança

Requisitos:
  pip install pymongo

Uso:
  python3 test_mongodb.py
"""

from pymongo import MongoClient
from datetime import datetime
import sys

# Substitua estes valores pelos seus
MONGO_URI = "mongodb+srv://<usuario>:<senha>@cluster0.xxxxx.mongodb.net/?retryWrites=true&w=majority"
DATABASE = "industrial"
COLLECTION = "pesagens"

def test_connection():
    """Testa conexão com MongoDB e lista documentos recentes"""
    try:
        print("[*] Conectando ao MongoDB...")
        client = MongoClient(MONGO_URI, serverSelectionTimeoutMS=5000)
        
        # Força uma conexão para validar credenciais
        client.admin.command('ping')
        print("[✓] Conectado com sucesso!")
        
    except Exception as e:
        print(f"[✗] Erro de conexão: {e}")
        print("\n[!] Dicas:")
        print("  1. Verifique se o URI está correto")
        print("  2. Confirme usuário/senha")
        print("  3. Verifique Network Access em cloud.mongodb.com")
        sys.exit(1)
    
    # Acessa coleção
    db = client[DATABASE]
    collection = db[COLLECTION]
    
    print(f"\n[*] Consultando {DATABASE}.{COLLECTION}...\n")
    
    # Conta documentos
    count = collection.count_documents({})
    print(f"[i] Total de documentos: {count}\n")
    
    if count == 0:
        print("[!] Nenhum documento encontrado.")
        print("    Aguarde seu ESP32 enviar dados e tente novamente.")
        return
    
    # Lista últimos 5 documentos
    print("=" * 80)
    print("ÚLTIMOS 5 DOCUMENTOS:")
    print("=" * 80)
    
    for i, doc in enumerate(collection.find().sort("_id", -1).limit(5), 1):
        print(f"\n[Documento {i}]")
        print(f"  ID:          {doc.get('_id')}")
        print(f"  Dispositivo: {doc.get('device_id')} ({doc.get('location')})")
        print(f"  Peso:        {doc.get('peso')} {doc.get('unidade')}")
        print(f"  Status:      {doc.get('status')}")
        print(f"  Timestamp:   {doc.get('timestamp')}")
        print(f"  Wi-Fi RSSI:  {doc.get('rssi_dbm')} dBm")
        print(f"  Uptime:      {doc.get('uptime_s')} s")
        print(f"  Firmware:    {doc.get('fw_version')}")
    
    print("\n" + "=" * 80)
    
    # Estatísticas
    print("\nESTATÍSTICAS:")
    print(f"  Dispositivos únicos:   {collection.distinct('device_id')}")
    print(f"  Locais únicos:         {collection.distinct('location')}")
    
    # Peso médio (último 1 hora)
    pipeline = [
        {"$group": {
            "_id": None,
            "peso_medio": {"$avg": "$peso"},
            "peso_min": {"$min": "$peso"},
            "peso_max": {"$max": "$peso"}
        }}
    ]
    stats = list(collection.aggregate(pipeline))
    if stats:
        s = stats[0]
        print(f"  Peso médio:            {s['peso_medio']:.3f} kg")
        print(f"  Peso mínimo:           {s['peso_min']:.3f} kg")
        print(f"  Peso máximo:           {s['peso_max']:.3f} kg")
    
    client.close()
    print("\n[✓] Teste concluído com sucesso!")

if __name__ == "__main__":
    print("\n" + "=" * 80)
    print("TESTE DE CONEXÃO - MongoDB Atlas")
    print("=" * 80 + "\n")
    
    if MONGO_URI.startswith("mongodb"):
        test_connection()
    else:
        print("[✗] ERRO: Configure MONGO_URI no script antes de executar!")
        print("\nPassos:")
        print("  1. Vá para cloud.mongodb.com")
        print("  2. Clique em 'Connect' no seu cluster")
        print("  3. Escolha 'Python' e copie a connection string")
        print("  4. Cole aqui substituindo MONGO_URI")
        sys.exit(1)
