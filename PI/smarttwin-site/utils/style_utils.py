"""
SmartTwin CEP - Style Utilities
Functions to inject custom CSS and handle theming
"""

import streamlit as st
from pathlib import Path


def load_css():
    """Load and inject custom CSS into the Streamlit app"""
    css_path = Path(__file__).parent.parent / "styles" / "main.css"
    
    if css_path.exists():
        with open(css_path, 'r', encoding='utf-8') as f:
            css = f.read()
    else:
        # Fallback inline CSS if file not found
        css = get_fallback_css()
    
    st.html(f'<style>{css}</style>')


def get_fallback_css():
    """Return minimal fallback CSS"""
    return """
    :root {
        --bg-primary: #0B1020;
        --accent-blue: #00D4FF;
        --accent-cyan: #00FFE0;
    }
    
    #MainMenu {visibility: hidden;}
    footer {visibility: hidden;}
    header {visibility: hidden;}
    
    .stApp {
        background: var(--bg-primary) !important;
    }
    """


def inject_google_fonts():
    """Inject Google Fonts into the page"""
    st.html("""
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
    """)


def set_page_config():
    """Configure Streamlit page settings"""
    st.set_page_config(
        page_title="SmartTwin CEP | Monitoramento Inteligente Industrial",
        page_icon="🔷",
        layout="wide",
        initial_sidebar_state="collapsed",
        menu_items={
            'Get Help': None,
            'Report a bug': None,
            'About': None
        }
    )


def inject_particles_script():
    """Inject floating particles animation"""
    st.html("""
    <div class="particles">
        <div class="particle" style="left: 10%; animation-delay: 0s;"></div>
        <div class="particle" style="left: 20%; animation-delay: 2s;"></div>
        <div class="particle" style="left: 30%; animation-delay: 4s;"></div>
        <div class="particle" style="left: 40%; animation-delay: 1s;"></div>
        <div class="particle" style="left: 50%; animation-delay: 3s;"></div>
        <div class="particle" style="left: 60%; animation-delay: 5s;"></div>
        <div class="particle" style="left: 70%; animation-delay: 2.5s;"></div>
        <div class="particle" style="left: 80%; animation-delay: 4.5s;"></div>
        <div class="particle" style="left: 90%; animation-delay: 1.5s;"></div>
    </div>
    """)


def inject_scroll_animations():
    """Inject JavaScript for scroll reveal animations"""
    st.html("""
    <script>
    document.addEventListener('DOMContentLoaded', function() {
        const reveals = document.querySelectorAll('.reveal');
        
        function reveal() {
            reveals.forEach(element => {
                const windowHeight = window.innerHeight;
                const elementTop = element.getBoundingClientRect().top;
                const elementVisible = 150;
                
                if (elementTop < windowHeight - elementVisible) {
                    element.classList.add('active');
                }
            });
        }
        
        window.addEventListener('scroll', reveal);
        reveal();
    });
    </script>
    """)
