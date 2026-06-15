"""
load.py — Etapa 4: Monta a coleção documental enriquecida
Cria 'producoes_com_participantes' aninhando os participantes dentro de cada produção.

Estrutura do documento resultante:
{
  "id_producao": 123,
  "titulo": "...",
  "ano": 1999,
  "tipo_id": 1,
  "tipo_nome": "Filme",
  "participantes": [
    {"id_pessoa": 456, "nome": "...", "papel": "Actor"},
    ...
  ],
  "total_participantes": 10
}
"""

import os
import sys
from collections import defaultdict
from pymongo import MongoClient
from pymongo.errors import BulkWriteError

MONGO_URI = os.getenv("MONGO_URI", "mongodb://root:password@localhost:27017")
DB_NAME   = "bd_producao_artistica"
BATCH     = 2_000


def main():
    print(f"Conectando ao MongoDB: {MONGO_URI}")
    client = MongoClient(MONGO_URI, serverSelectionTimeoutMS=5_000)

    try:
        client.admin.command("ping")
        print("Conexão OK!\n")
    except Exception as e:
        print(f"[ERRO] {e}")
        sys.exit(1)

    db = client[DB_NAME]

    # ── 1. Índice de pessoas ───────────────────────────────────────────────────
    print("Carregando índice de pessoas na memória...")
    pessoa_map = {
        doc["id_pessoa"]: doc["nome"]
        for doc in db["pessoa_clean"].find({}, {"id_pessoa": 1, "nome": 1, "_id": 0})
    }
    print(f"  {len(pessoa_map):,} pessoas indexadas.\n")

    # ── 2. Agrupa participantes por produção ───────────────────────────────────
    print("Agrupando participantes por produção...")
    participantes_por_producao = defaultdict(list)

    for doc in db["equipe_clean"].find({}, {"id_producao": 1, "id_pessoa": 1, "papel": 1, "_id": 0}):
        id_prod = doc["id_producao"]
        participantes_por_producao[id_prod].append({
            "id_pessoa": doc["id_pessoa"],
            "nome":      pessoa_map.get(doc["id_pessoa"], "Desconhecido"),
            "papel":     doc["papel"],
        })

    print(f"  {len(participantes_por_producao):,} produções com participantes.\n")

    # ── 3. Monta e insere documentos enriquecidos ─────────────────────────────
    print("Montando coleção 'producoes_com_participantes'...")
    col = db["producoes_com_participantes"]
    col.drop()

    batch = []
    total = 0

    for producao in db["producao_clean"].find({}, {"_id": 0}):
        id_prod       = producao["id_producao"]
        participantes = participantes_por_producao.get(id_prod, [])

        doc = {
            **producao,
            "participantes":      participantes,
            "total_participantes": len(participantes),
        }
        batch.append(doc)
        total += 1

        if len(batch) >= BATCH:
            col.insert_many(batch, ordered=False)
            batch = []
            print(f"  → {total:,} documentos inseridos...", end="\r")

    if batch:
        col.insert_many(batch, ordered=False)

    # Índices úteis para as consultas
    col.create_index("id_producao", unique=True)
    col.create_index("ano")
    col.create_index("tipo_id")
    col.create_index("total_participantes")
    col.create_index("participantes.papel")

    print(f"\n  [OK] {total:,} documentos criados em 'producoes_com_participantes'.")
    print("\nCarga documental concluída!")
    client.close()


if _name_ == "_main_":
    main()