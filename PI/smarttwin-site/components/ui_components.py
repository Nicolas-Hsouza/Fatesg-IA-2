"""
SmartTwin CEP - Component Library
Reusable UI components for the premium landing page
"""

import streamlit as st


def render_navigation():
    """Render the fixed navigation bar"""
    st.html("""
    <div class="nav-container">
        <div class="nav-logo">SmartTwin CEP</div>
        <div class="nav-links">
            <a href="#features" class="nav-link">Funcionalidades</a>
            <a href="#demo" class="nav-link">Demo</a>
            <a href="#tech" class="nav-link">Tecnologia</a>
            <a href="#faq" class="nav-link">FAQ</a>
            <a href="#contact" class="nav-link">Contato</a>
        </div>
    </div>
    """)


def render_hero():
    """Render the hero section with animated background"""
    st.html("""
    <div class="animated-bg"></div>
    <div class="grid-overlay"></div>
    
    <div class="hero-section">
        <div class="hero-badge">
            <span>Indústria 4.0 + IA Industrial</span>
        </div>
        
        <h1 class="hero-title">
            Transforme dados industriais em<br>
            <span class="gradient-text">decisões inteligentes.</span>
        </h1>
        
        <p class="hero-subtitle">
            O SmartTwin CEP integra Inteligência Artificial, Gêmeo Digital e Controle Estatístico 
            de Processo para detectar anomalias, prever instabilidades e otimizar operações 
            industriais em tempo real.
        </p>
        
        <div class="hero-cta-group">
            <a href="#contact" class="btn-primary">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"/>
                </svg>
                Agendar Demonstração
            </a>
            <a href="#demo" class="btn-secondary">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <polygon points="5 3 19 12 5 21 5 3"/>
                </svg>
                Explorar Plataforma
            </a>
        </div>
    </div>
    """)


def render_section_header(badge: str, title: str, subtitle: str = ""):
    """Render a section header with badge"""
    subtitle_html = f'<p class="section-subtitle">{subtitle}</p>' if subtitle else ""
    st.html(f"""
    <div class="section-header">
        <span class="section-badge">{badge}</span>
        <h2 class="section-title">{title}</h2>
        {subtitle_html}
    </div>
    """)


def render_feature_card(icon: str, title: str, description: str, tags: list = None):
    """Render a premium feature card with glassmorphism effect"""
    tags_html = ""
    if tags:
        tags_html = '<div class="feature-tags">' + ''.join([f'<span class="feature-tag">{tag}</span>' for tag in tags]) + '</div>'
    
    return f"""
    <div class="feature-card">
        <div class="feature-icon">{icon}</div>
        <h3 class="feature-title">{title}</h3>
        <p class="feature-description">{description}</p>
        {tags_html}
    </div>
    """


def render_kpi_card(value: str, label: str):
    """Render a KPI metric card"""
    return f"""
    <div class="kpi-card">
        <div class="kpi-value">{value}</div>
        <div class="kpi-label">{label}</div>
    </div>
    """


def render_problem_card(icon: str, title: str, items: list):
    """Render a problem card"""
    items_html = ''.join([f'<li style="color: var(--text-secondary); margin-bottom: 0.5rem;">{item}</li>' for item in items])
    return f"""
    <div class="problem-card">
        <div class="feature-icon problem-icon">{icon}</div>
        <h3 class="feature-title" style="color: #FF6B6B;">{title}</h3>
        <ul style="padding-left: 1.5rem; margin-top: 1rem;">
            {items_html}
        </ul>
    </div>
    """


def render_solution_card(icon: str, title: str, items: list):
    """Render a solution card"""
    items_html = ''.join([f'<li style="color: var(--text-secondary); margin-bottom: 0.5rem;">{item}</li>' for item in items])
    return f"""
    <div class="solution-card">
        <div class="feature-icon solution-icon">{icon}</div>
        <h3 class="feature-title" style="color: var(--accent-cyan);">{title}</h3>
        <ul style="padding-left: 1.5rem; margin-top: 1rem;">
            {items_html}
        </ul>
    </div>
    """


def render_process_flow(steps: list):
    """Render the process flow visualization"""
    steps_html = ""
    for step in steps:
        steps_html += f"""
        <div class="process-step">
            <div class="process-icon">{step['icon']}</div>
            <span class="process-label">{step['label']}</span>
        </div>
        """
    
    return f"""
    <div class="process-flow">
        {steps_html}
    </div>
    """


def render_tech_badge(name: str, icon: str = ""):
    """Render a technology badge"""
    icon_html = f'<span>{icon}</span>' if icon else ""
    return f"""
    <div class="tech-badge">
        {icon_html}
        <span>{name}</span>
    </div>
    """


def render_faq_item(question: str, answer: str):
    """Render an FAQ accordion item"""
    return f"""
    <div class="faq-item">
        <div class="faq-question">
            <span>{question}</span>
            <span style="color: var(--accent-blue);">+</span>
        </div>
        <div class="faq-answer">{answer}</div>
    </div>
    """


def render_cta_section():
    """Render the final call-to-action section"""
    st.html("""
    <div class="cta-section" id="contact">
        <h2 class="cta-title">Leve inteligência industrial para o próximo nível.</h2>
        <p class="cta-subtitle">
            Descubra como o SmartTwin CEP pode transformar sua operação industrial 
            com IA, Gêmeo Digital e CEP integrados.
        </p>
        <a href="mailto:contato@smarttwincep.com" class="btn-primary" style="position: relative;">
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <path d="M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2z"/>
                <polyline points="22,6 12,13 2,6"/>
            </svg>
            Solicitar Demonstração
        </a>
    </div>
    """)


def render_footer():
    """Render the footer"""
    st.html("""
    <div class="footer">
        <div class="footer-logo">SmartTwin CEP</div>
        <p class="footer-text">
            Monitoramento Inteligente para Processos Industriais<br>
            &copy; 2024 SmartTwin CEP. Todos os direitos reservados.
        </p>
    </div>
    """)


def render_dashboard_header():
    """Render the dashboard preview header"""
    return """
    <div class="dashboard-header">
        <span class="dashboard-title">Dashboard de Monitoramento em Tempo Real</span>
        <div class="dashboard-status">Sistema Operacional</div>
    </div>
    """
