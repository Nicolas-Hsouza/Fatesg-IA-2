# SmartTwin CEP

**Monitoramento Inteligente para Processos Industriais**

Site institucional premium para a startup SmartTwin CEP - a primeira plataforma que integra Controle Estatístico de Processo, Inteligência Artificial e Gêmeo Digital em um único ambiente inteligente.

## Stack Tecnológica

- **Python** - Linguagem principal
- **Streamlit** - Framework web
- **Plotly** - Visualizações interativas
- **Pandas/NumPy** - Processamento de dados

## Estrutura do Projeto

```
smarttwin-site/
├── app.py                 # Aplicação principal
├── requirements.txt       # Dependências
├── components/
│   ├── __init__.py
│   ├── ui_components.py   # Componentes de UI reutilizáveis
│   └── charts.py          # Gráficos Plotly premium
├── utils/
│   ├── __init__.py
│   ├── data_utils.py      # Utilitários de dados
│   └── style_utils.py     # Configurações de estilo
├── styles/
│   └── main.css           # CSS customizado premium
├── assets/                # Assets estáticos
├── pages/                 # Páginas adicionais (futuro)
└── data/                  # Dados (futuro)
```

## Instalação

1. Clone o repositório
2. Crie um ambiente virtual:
   ```bash
   python -m venv venv
   source venv/bin/activate  # Linux/Mac
   venv\Scripts\activate     # Windows
   ```
3. Instale as dependências:
   ```bash
   pip install -r requirements.txt
   ```

## Execução

```bash
cd smarttwin-site
streamlit run app.py
```

O site estará disponível em `http://localhost:8501`

## Funcionalidades

- Hero section cinematográfica com efeitos visuais
- Dashboard interativo com gráficos CEP em tempo real
- Visualização de Gêmeo Digital com previsão EMA
- Detecção de anomalias com Isolation Forest
- Design glassmorphism com tema dark premium
- Navegação customizada
- Totalmente responsivo
- Animações e microinterações

## Customização

### Cores
Edite as variáveis CSS em `styles/main.css`:
```css
:root {
    --bg-primary: #0B1020;
    --accent-blue: #00D4FF;
    --accent-cyan: #00FFE0;
}
```

### Dados
Modifique `utils/data_utils.py` para conectar a fontes de dados reais.

### Gráficos
Personalize os gráficos em `components/charts.py`.

## Licença

Proprietary - SmartTwin CEP
