"""
SmartTwin CEP - Premium Industrial AI Landing Page
Main Application Entry Point

A cinematographic, premium landing page for SmartTwin CEP,
the first platform integrating SPC, AI, and Digital Twin
for intelligent industrial monitoring.
"""

import streamlit as st
import sys
from pathlib import Path

# Add project root to path
sys.path.insert(0, str(Path(__file__).parent))

from utils.style_utils import set_page_config, load_css, inject_google_fonts, inject_particles_script
from components.ui_components import (
    render_navigation,
    render_hero,
    render_section_header,
    render_feature_card,
    render_kpi_card,
    render_problem_card,
    render_solution_card,
    render_process_flow,
    render_tech_badge,
    render_faq_item,
    render_cta_section,
    render_footer,
    render_dashboard_header
)
from components.charts import (
    create_control_chart,
    create_ema_prediction_chart,
    create_anomaly_detection_chart,
    create_capability_gauge,
    create_time_series_kpi,
    create_residuals_chart
)
from utils.data_utils import get_kpi_data

# ==============================================
# PAGE CONFIGURATION
# ==============================================
set_page_config()
inject_google_fonts()
load_css()
inject_particles_script()

# ==============================================
# NAVIGATION
# ==============================================
render_navigation()

# ==============================================
# HERO SECTION
# ==============================================
render_hero()

# ==============================================
# SOCIAL PROOF - METRICS BAR
# ==============================================
st.html('<div class="section">')

kpi_data = get_kpi_data()

col1, col2, col3, col4, col5 = st.columns(5)

with col1:
    st.html(render_kpi_card("99.7%", "Uptime"))
with col2:
    st.html(render_kpi_card(f"{kpi_data['precision']}%", "Precisão IA"))
with col3:
    st.html(render_kpi_card("< 100ms", "Latência"))
with col4:
    st.html(render_kpi_card("24/7", "Monitoramento"))
with col5:
    st.html(render_kpi_card("300+", "Medições/min"))

st.html('</div>')

# ==============================================
# PROBLEM & SOLUTION SECTION
# ==============================================
st.html('<div class="section">')

render_section_header(
    badge="O Desafio",
    title="Processos industriais sem visibilidade geram perdas.",
    subtitle="A falta de monitoramento inteligente custa milhões em retrabalho, paradas e ineficiências."
)

col1, col2 = st.columns(2)

with col1:
    st.html(render_problem_card(
        icon="⚠️",
        title="Sem SmartTwin CEP",
        items=[
            "Análise manual e reativa",
            "Detecção tardia de anomalias",
            "Perdas por instabilidade",
            "Retrabalho constante",
            "Falta de previsibilidade"
        ]
    ))

with col2:
    st.html(render_solution_card(
        icon="✓",
        title="Com SmartTwin CEP",
        items=[
            "Monitoramento em tempo real",
            "Detecção automática de desvios",
            "Previsão de instabilidades",
            "Insights acionáveis com IA",
            "Controle estatístico integrado"
        ]
    ))

st.html('</div>')

# ==============================================
# FEATURES SECTION
# ==============================================
st.html('<div class="section" id="features">')

render_section_header(
    badge="Funcionalidades",
    title="Tecnologia de ponta para monitoramento industrial.",
    subtitle="Uma plataforma completa que integra CEP, IA e Gêmeo Digital em um único ambiente."
)

# Feature Grid
st.html('<div class="feature-grid">')

col1, col2 = st.columns(2)

with col1:
    st.html(render_feature_card(
        icon="📊",
        title="CEP Inteligente",
        description="Controle estatístico de processo com cartas X̄ e R, cálculo automático de Cp/Cpk e Run Rules em tempo real.",
        tags=["Cp", "Cpk", "X̄ & R", "Run Rules"]
    ))

with col2:
    st.html(render_feature_card(
        icon="🔮",
        title="Gêmeo Digital",
        description="Modelo preditivo EMA com análise de resíduos e monitoramento contínuo para antecipar desvios.",
        tags=["EMA", "Previsão", "Resíduos", "Real-time"]
    ))

col3, col4 = st.columns(2)

with col3:
    st.html(render_feature_card(
        icon="🤖",
        title="IA para Anomalias",
        description="Detecção automática com Isolation Forest, alertas inteligentes e análise preditiva avançada.",
        tags=["Isolation Forest", "Alertas", "Predição"]
    ))

with col4:
    st.html(render_feature_card(
        icon="💡",
        title="Insights com Gemini",
        description="Recomendações em linguagem natural, interpretação automática e apoio à decisão com IA generativa.",
        tags=["NLP", "Gemini AI", "Decisões"]
    ))

st.html('</div>')
st.html('</div>')

# ==============================================
# DASHBOARD DEMO SECTION
# ==============================================
st.html('<div class="section" id="demo">')

render_section_header(
    badge="Demo Interativa",
    title="Visualize o poder do monitoramento inteligente.",
    subtitle="Dashboard em tempo real com CEP, previsão e detecção de anomalias."
)

st.html('<div class="dashboard-container">')
st.html(render_dashboard_header())

# KPI Row
st.html("### Indicadores de Performance")
kpi_col1, kpi_col2, kpi_col3, kpi_col4, kpi_col5 = st.columns(5)

with kpi_col1:
    st.markdown(render_kpi_card(f"{kpi_data['cp']:.3f}", "Cp"))
with kpi_col2:
    st.html(render_kpi_card(f"{kpi_data['cpk']:.3f}", "Cpk"))
with kpi_col3:
    st.html(render_kpi_card(str(kpi_data['anomalies']), "Anomalias"))
with kpi_col4:
    st.html(render_kpi_card(f"{kpi_data['precision']}%", "Precisão"))
with kpi_col5:
    st.html(render_kpi_card(str(kpi_data['measurements']), "Medições"))

st.html("---")

# Charts Row 1
chart_col1, chart_col2 = st.columns(2)

with chart_col1:
    st.plotly_chart(create_control_chart(), use_container_width=True, config={'displayModeBar': False})

with chart_col2:
    st.plotly_chart(create_ema_prediction_chart(), use_container_width=True, config={'displayModeBar': False})

# Charts Row 2
chart_col3, chart_col4 = st.columns(2)

with chart_col3:
    st.plotly_chart(create_anomaly_detection_chart(), use_container_width=True, config={'displayModeBar': False})

with chart_col4:
    st.plotly_chart(create_residuals_chart(), use_container_width=True, config={'displayModeBar': False})

# Gauge Charts
st.markdown("### Índices de Capacidade")
gauge_col1, gauge_col2, gauge_col3 = st.columns(3)

with gauge_col1:
    st.plotly_chart(create_capability_gauge(kpi_data['cp'], "Cp"), use_container_width=True, config={'displayModeBar': False})

with gauge_col2:
    st.plotly_chart(create_capability_gauge(kpi_data['cpk'], "Cpk"), use_container_width=True, config={'displayModeBar': False})

with gauge_col3:
    st.plotly_chart(create_capability_gauge(kpi_data['precision']/100, "Precisão", 0, 1), use_container_width=True, config={'displayModeBar': False})

st.markdown('</div>')
st.html('</div>')

# ==============================================
# HOW IT WORKS - PROCESS FLOW
# ==============================================
st.html('<div class="section">')

render_section_header(
    badge="Como Funciona",
    title="Do dado bruto à inteligência operacional.",
    subtitle="Um fluxo integrado que transforma informações em ações."
)

process_steps = [
    {"icon": "📥", "label": "Coleta de Dados"},
    {"icon": "📊", "label": "CEP"},
    {"icon": "🔮", "label": "Digital Twin"},
    {"icon": "🤖", "label": "IA"},
    {"icon": "⚡", "label": "Anomalias"},
    {"icon": "📈", "label": "Dashboard"},
    {"icon": "💡", "label": "Insights"}
]

st.html(render_process_flow(process_steps))

st.html('</div>')

# ==============================================
# TECHNOLOGY STACK
# ==============================================
st.html('<div class="section" id="tech">')

render_section_header(
    badge="Arquitetura",
    title="Stack tecnológica moderna e escalável.",
    subtitle="Construído com as melhores ferramentas para performance e confiabilidade."
)

st.html('<div class="tech-grid">')

tech_stack = [
    ("🐍", "Python"),
    ("📊", "Streamlit"),
    ("📈", "Plotly"),
    ("⚡", "FastAPI"),
    ("🗄️", "SQLite"),
    ("🐼", "Pandas"),
    ("🔢", "NumPy"),
    ("🤖", "Scikit-learn"),
    ("🧠", "Gemini AI")
]

tech_html = "".join([render_tech_badge(name, icon) for icon, name in tech_stack])
st.html(f'<div style="display: flex; flex-wrap: wrap; justify-content: center; gap: 1rem;">{tech_html}</div>')

st.html('</div>')
st.html('</div>')

# ==============================================
# FAQ SECTION
# ==============================================
st.html('<div class="section" id="faq">')

render_section_header(
    badge="FAQ",
    title="Perguntas Frequentes",
    subtitle="Tire suas dúvidas sobre a plataforma SmartTwin CEP."
)

faq_data = [
    {
        "question": "O sistema funciona em tempo real?",
        "answer": "Sim. O SmartTwin CEP processa dados em tempo real com latência inferior a 100ms, permitindo detecção instantânea de anomalias e alertas imediatos."
    },
    {
        "question": "Integra com sensores industriais?",
        "answer": "Sim. A plataforma se conecta via APIs REST e protocolos industriais padrão como OPC-UA, MQTT e Modbus para coleta de dados de sensores."
    },
    {
        "question": "Funciona além do envase?",
        "answer": "Absolutamente. Embora desenvolvido inicialmente para envase, o SmartTwin CEP é aplicável a qualquer processo industrial com variáveis mensuráveis."
    },
    {
        "question": "Como a IA detecta anomalias?",
        "answer": "Utilizamos Isolation Forest e outros algoritmos de machine learning que aprendem o comportamento normal do processo e identificam desvios estatisticamente significativos."
    },
    {
        "question": "O sistema é escalável?",
        "answer": "Sim. A arquitetura modular permite escalar horizontalmente para monitorar múltiplas linhas de produção simultaneamente."
    },
    {
        "question": "Pode integrar com SCADA/MES?",
        "answer": "Sim. Oferecemos APIs para integração com sistemas SCADA, MES, ERP e outras plataformas industriais existentes."
    }
]

for faq in faq_data:
    with st.expander(faq["question"]):
        st.html(f'<p style="color: rgba(255,255,255,0.7); line-height: 1.7;">{faq["answer"]}</p>')

st.html('</div>')

# ==============================================
# FINAL CTA
# ==============================================
st.html('<div class="section">')
render_cta_section()
st.html('</div>')

# ==============================================
# FOOTER
# ==============================================
render_footer()
