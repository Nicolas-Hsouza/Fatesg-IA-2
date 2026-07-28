# Projeto RAG simples com Qdrant local + Maritaca

Este projeto usa:

- CSV local: `C:\Users\engwi\OneDrive\Documentos\qdrant\Data\imdb_top_1000.csv`
- Qdrant local: `http://localhost:6333`
- Embeddings leves: `sentence-transformers/all-MiniLM-L6-v2`
- LLM Maritaca: `sabiazinho-4`
- Chave da Maritaca em `.env`

## 1. Instalar dependências

No terminal, dentro da pasta do projeto:

```bash
pip install -r requirements.txt
```

## 2. Criar o arquivo `.env`

Copie `.env.example` para `.env`.

Depois, edite o arquivo `.env`:

```env
MARITACA_API_KEY=sua_chave_real_aqui
```

## 3. Rodar o Qdrant local

Exemplo usando Docker:

```bash
docker run -p 6333:6333 -p 6334:6334 -v "%cd%/qdrant_storage:/qdrant/storage" qdrant/qdrant
```

## 4. Abrir o notebook

Abra o arquivo:

```text
01_rag_qdrant_imdb_maritaca.ipynb
```

Execute as células em ordem.

## 5. Fazer perguntas

No final do notebook existe um loop:

```text
Pergunta:
```

Digite perguntas sobre os filmes e escreva `sair` para parar.
