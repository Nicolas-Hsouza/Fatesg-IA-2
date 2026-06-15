# Projeto: Análise de Dados e Machine Learning - Agronegócio em Goiás

1. Identificação Acadêmica 

* 
**Disciplina:** Big Data e Data Science 


* 
**Professor:** Reinaldo Jr 


* **Instituição:** Fatesg SENAI
* **Estudante:** Adriano Silva (e grupo)

2. Introdução 

Este projeto consiste na análise da evolução da produção agrícola no Estado de Goiás entre os anos de 2000 e 2024. Goiás consolidou-se como uma potência global do agronegócio, e este estudo busca aplicar fundamentos de estatística descritiva para entender o comportamento das principais commodities e culturas especializadas.

3. Coleta e Estatística Descritiva (Requisito R1) 

### Base de Dados

* **Fonte:** Instituto Mauro Borges de Estatísticas e Estudos Socioeconômicos (IMB) - [https://www.imb.go.gov.br/bde/](https://www.imb.go.gov.br/bde/).
* **Resumo:** Dados anuais cobrindo **Área Colhida (ha)** e **Quantidade Produzida (t)** para culturas como Soja, Milho, Cana-de-açúcar, Sorgo, Tomate, Algodão, Feijão, Trigo, Girassol e Alho.

Dicionário de Variáveis 

| Coluna | Descrição |
| --- | --- |
| **Localidade** | Abrangência geográfica (Estado de Goiás). |
| **Variável** | Descrição da cultura e unidade de medida ($ha$ ou $t$). |
| **2000 a 2024** | Série temporal com os valores registrados em cada ano. |

Resultados da Análise Estatística 

Após o tratamento dos dados (conversão de strings para floats e remoção de caracteres especiais), obtivemos os seguintes indicadores para a **Quantidade Produzida (t)**:

| Cultura | Máximo (t) | Mínimo (t) | Moda (t) | Q1 (25%) | Q3 (75%) |
| --- | --- | --- | --- | --- | --- |
| **Cana-de-açúcar** | 81.599.588 | 10.162.959 | 10.162.959 | 19.049.550 | 72.066.835 |
| **Soja** | 17.405.060 | 4.052.169 | 4.052.169 | 6.319.213 | 11.372.539 |
| **Milho** | 14.460.846 | 2.855.538 | 2.855.538 | 4.157.387 | 9.996.344 |
| **Tomate** | 1.463.461 | 712.448 | 712.448 | 912.976 | 1.249.525 |

Interpretação dos Resultados 

* **Amplitude (Mín/Máx):** O salto da **Cana-de-açúcar** de 10 milhões para 81 milhões de toneladas evidencia a transição energética e o boom das usinas em Goiás.
* **Quartis (Q1/Q3):** Na **Soja** e no **Milho**, o terceiro quartil ($Q3$) é significativamente maior que a média histórica, indicando que os recordes de produtividade estão concentrados na última década, refletindo o avanço tecnológico no campo.
* **Estabilidade:** O **Tomate** apresenta um valor mínimo elevado e quartis próximos, sugerindo uma cultura consolidada e estável para a indústria de processamento goiana.
* **Moda:** Em séries temporais crescentes, a moda frequentemente reflete os patamares iniciais (anos 2000), servindo como base histórica para medir a evolução.

4. Ferramentas Utilizadas 

* **Linguagem:** Python
* **Bibliotecas:** Pandas (Tratamento de dados) 
                   Numpy (Cálculos estatísticos).
* **IDE:** Antigravity

---
## 5. Visualização de Dados (R2)

Para complementar a análise estatística, foram desenvolvidos gráficos utilizando as bibliotecas **Matplotlib** e **Seaborn**, permitindo identificar padrões de produção agrícola das principais culturas do estado de Goiás.

### Objetivos dos Gráficos

Os gráficos foram criados com o objetivo de:

- Identificar as culturas com maior produção histórica;
- Comparar médias de produtividade entre culturas;
- Avaliar a variabilidade da produção agrícola ao longo dos anos;
- Facilitar a interpretação visual dos dados estatísticos.

### Bibliotecas Utilizadas

```python
import matplotlib.pyplot as plt
import seaborn as sns
```

---

## 6. Modelagem Preditiva (R3)

### Objetivo

Desenvolver modelos preditivos para prever a quantidade produzida de culturas específicas em anos futuros, utilizando técnicas de Machine Learning.

### Metodologia

Três abordagens foram implementadas para realizar previsões:

#### 6.1 Random Forest com Features Temporais

**Descrição:** Utiliza características defasadas (lags) da produção histórica para treinar um modelo Random Forest.

**Features Utilizadas:**
- `Ano`: Ano do registro
- `Producao_Ano_Anterior`: Produção do ano anterior (lag de 1)
- `Media_2_Anos`: Média móvel de 2 anos
- `Producao_2_Anos_Atras`: Produção de 2 anos atrás (lag de 2)

**Divisão de Dados:**
- **Treino:** 2000 a 2021
- **Teste:** 2022 a 2024

**Justificativa do Algoritmo:** Escolhido por ser um método ensemble baseado em múltiplas árvores de decisão. É robusto para lidar com variações não lineares em séries temporais agrícolas curtas, reduzindo a variância e mitigando o risco de overfitting.

#### 6.2 Random Forest Otimizado com Área Colhida

**Descrição:** Versão aprimorada que incorpora a variável "Área Colhida" como feature adicional, melhorando a precisão das previsões.

**Features Utilizadas:**
- `Ano`: Ano do registro
- `Lag_Producao`: Produção do ano anterior
- `Area`: Área colhida em hectares

**Parâmetros do Modelo:**
- `n_estimators=200`: 200 árvores de decisão
- `random_state=42`: Reprodutibilidade

**Benefício:** A inclusão da área colhida melhora significativamente a capacidade preditiva, pois existe forte correlação entre área plantada e produção.

#### 6.3 Prophet (Detecção de Tendências Sazonais)

**Descrição:** Utiliza o modelo Prophet do Facebook para capturar tendências e padrões sazonais na série temporal agrícola.

**Características:**
- Sazonalidade anual ativada
- Sem sazonalidade diária ou semanal (dados anuais)
- Período de previsão: 3 anos (2022-2024)

**Vantagem:** Prophet é especialmente eficaz em séries temporais com sazonalidade clara e dados limitados.

### Métricas de Avaliação

Os modelos foram avaliados utilizando as seguintes métricas:

| Métrica | Fórmula | Interpretação |
| --- | --- | --- |
| **MAE** | $\frac{1}{n}\sum_{i=1}^{n}\|y_i - \hat{y}_i\|$ | Erro médio absoluto em toneladas |
| **RMSE** | $\sqrt{\frac{1}{n}\sum_{i=1}^{n}(y_i - \hat{y}_i)^2}$ | Penaliza erros maiores |
| **R² Score** | $1 - \frac{\sum(y_i - \hat{y}_i)^2}{\sum(y_i - \bar{y})^2}$ | Proporção da variância explicada (0 a 1) |

### Ferramentas Utilizadas

```python
from sklearn.ensemble import RandomForestRegressor
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
from prophet import Prophet



