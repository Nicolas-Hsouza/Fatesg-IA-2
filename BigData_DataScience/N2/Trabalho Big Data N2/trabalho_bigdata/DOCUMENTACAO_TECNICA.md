### 📄 Documentação Técnica: Tratamento e Normalização (Requisito R1)

#### 1. Identificação da Fonte

* **Arquivo original:** `consulta (3).csv`
* **Origem:** Instituto Mauro Borges (IMB).
* **Estado dos dados brutos:** O arquivo apresentava números formatados como strings no padrão brasileiro (ex: `1.234.567`), além de caracteres especiais como hífens (`-`) para representar valores nulos ou não informados.

#### 2. Pipeline de Limpeza (Data Cleaning)

Para garantir que os cálculos estatísticos fossem precisos, o grupo implementou as seguintes etapas de pré-processamento via Python:

1. **Conversão de Tipos:** Criamos a função `limpar_valor()` que:
* Removeu o ponto (`.`) que servia como separador de milhar.
* Substituiu o caractere `-` pelo valor numérico `0.0`.
* Converteu todas as colunas temporais (2000-2024) de `object` (texto) para `float64` (numérico).


2. **Normalização de Strings:** Aplicamos o método `.strip()` e `.split()` na coluna **Variável** para isolar o nome da cultura (ex: transformar *"Produção Agrícola - Soja - Total..."* apenas em *"Soja"*).
3. **Tratamento de Exceções:** Implementamos o tratamento do erro de indexação da **Moda** (`.iloc`), garantindo que em casos de distribuições multimodais ou vazias, o código não interrompesse a execução.

#### 3. Metodologia Estatística

* **Média vs. Mediana:** Utilizamos essa comparação para identificar se a produção de uma commodity teve picos de crescimento (assimetria).
* **Análise de Quartis:** O cálculo de Q1 e Q3 foi essencial para definir a variabilidade da safra, permitindo identificar o "piso" e o "teto" produtivo de Goiás no século XXI.
* **Variância e Desvio Padrão:** Calculados para medir o risco e a volatilidade de cada cultura agrícola frente a fatores externos (clima e mercado).

---

## 4. Visualização Estatística dos Dados (R2)

Após o tratamento e cálculo das estatísticas descritivas, foi implementada uma etapa de visualização gráfica para apoiar a interpretação dos resultados.

### Bibliotecas Utilizadas

```python
import matplotlib.pyplot as plt
import seaborn as sns
```

---

## 5. Modelagem Preditiva (Requisito R3)

### Objetivo

Implementar modelos de regressão para prever a quantidade produzida de culturas em anos futuros usando dados históricos (2000-2024).

### Modelos Utilizados

#### 1. Random Forest com Features Temporais
- **Features:** Ano, Produção do ano anterior, Média móvel 2 anos, Produção de 2 anos atrás
- **Divisão:** Treino (2000-2021) e Teste (2022-2024)
- **Parâmetros:** 100 árvores, max_depth=5
- **Vantagem:** Captura relações não-lineares em séries temporais curtas

#### 2. Random Forest com Área Colhida
- **Features:** Ano, Produção defasada, Área colhida
- **Parâmetros:** 200 árvores
- **Vantagem:** Inclui variável exógena para melhor precisão

#### 3. Prophet (Facebook)
- **Características:** Sazonalidade anual, detecção de tendências
- **Vantagem:** Eficaz para séries com padrões sazonais claros

### Métricas de Avaliação

| Métrica | Descrição |
| --- | --- |
| **MAE** | Erro médio absoluto em toneladas |
| **RMSE** | Penaliza erros maiores |
| **R²** | Proporção da variância explicada (0 a 1) |

---

## 6. Infraestrutura e Dependências

### Ambiente de Execução

```python
import pandas as pd          # Manipulação de dados
import numpy as np           # Cálculos numéricos
import matplotlib.pyplot as plt  # Visualização
import seaborn as sns        # Gráficos estatísticos
from sklearn.ensemble import RandomForestRegressor  # ML
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score  # Métricas
from prophet import Prophet  # Séries temporais
```

### Arquivo requirements.txt

```
pandas>=2.1.0
numpy>=1.24.0
matplotlib>=3.8.0
seaborn>=0.13.0
scikit-learn>=1.3.0
prophet>=1.1.0
jupyter>=1.0.0
ipython>=8.15.0
```

---

## 7. Estrutura do Repositório

```
trabalho_bigdata/
├── data/
│   └── consulta (3).csv              # Dados brutos do IMB
├── script/
│   └── Tratamento.ipynb              # Notebook com R1, R2 e R3
├── resultados/
│   └── [Gráficos e previsões geradas]
├── README.md                          # Documentação resumida
├── DOCUMENTACAO_TECNICA.md           # Este arquivo
└── requirements.txt                   # Dependências Python
```

---

## 8. Conclusão

Este projeto demonstra a aplicação de técnicas de **Estatística Descritiva**, **Visualização de Dados** e **Machine Learning** em um contexto agrícola real. A combinação de múltiplas abordagens preditivas permite uma compreensão mais robusta dos padrões de produção agrícola do estado de Goiás

