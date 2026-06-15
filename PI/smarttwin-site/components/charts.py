"""
SmartTwin CEP - Chart Components
Premium Plotly charts with dark theme and glow effects
"""

import plotly.graph_objects as go
import plotly.express as px
import numpy as np
import pandas as pd
from datetime import datetime, timedelta


def get_dark_theme():
    """Return the premium dark theme configuration for Plotly"""
    return {
        'paper_bgcolor': 'rgba(0,0,0,0)',
        'plot_bgcolor': 'rgba(0,0,0,0)',
        'font': {
            'family': 'Inter, sans-serif',
            'color': 'rgba(255,255,255,0.7)',
            'size': 12
        },
        'xaxis': {
            'gridcolor': 'rgba(0,212,255,0.1)',
            'linecolor': 'rgba(0,212,255,0.2)',
            'tickfont': {'color': 'rgba(255,255,255,0.5)'},
            'title_font': {'color': 'rgba(255,255,255,0.7)'}
        },
        'yaxis': {
            'gridcolor': 'rgba(0,212,255,0.1)',
            'linecolor': 'rgba(0,212,255,0.2)',
            'tickfont': {'color': 'rgba(255,255,255,0.5)'},
            'title_font': {'color': 'rgba(255,255,255,0.7)'}
        }
    }


def create_control_chart(data: pd.DataFrame = None):
    """Create a premium CEP control chart (X-bar and R chart)"""
    
    # Generate sample data if not provided
    if data is None:
        np.random.seed(42)
        n_points = 50
        dates = [datetime.now() - timedelta(hours=i) for i in range(n_points)][::-1]
        
        # Simulated process data with some out-of-control points
        values = np.random.normal(100, 2, n_points)
        values[15] = 108  # Out of control point
        values[32] = 94   # Out of control point
        values[45] = 107  # Out of control point
        
        data = pd.DataFrame({
            'timestamp': dates,
            'value': values
        })
    
    mean = data['value'].mean()
    std = data['value'].std()
    ucl = mean + 3 * std
    lcl = mean - 3 * std
    
    fig = go.Figure()
    
    # Add control limits as filled areas
    fig.add_trace(go.Scatter(
        x=data['timestamp'],
        y=[ucl] * len(data),
        mode='lines',
        name='UCL',
        line=dict(color='rgba(255,77,77,0.5)', width=1, dash='dash'),
        showlegend=True
    ))
    
    fig.add_trace(go.Scatter(
        x=data['timestamp'],
        y=[lcl] * len(data),
        mode='lines',
        name='LCL',
        line=dict(color='rgba(255,77,77,0.5)', width=1, dash='dash'),
        fill='tonexty',
        fillcolor='rgba(0,212,255,0.05)',
        showlegend=True
    ))
    
    # Add center line
    fig.add_trace(go.Scatter(
        x=data['timestamp'],
        y=[mean] * len(data),
        mode='lines',
        name='Média',
        line=dict(color='rgba(0,255,224,0.5)', width=2),
        showlegend=True
    ))
    
    # Identify out of control points
    out_of_control = (data['value'] > ucl) | (data['value'] < lcl)
    
    # Add data points - in control
    fig.add_trace(go.Scatter(
        x=data.loc[~out_of_control, 'timestamp'],
        y=data.loc[~out_of_control, 'value'],
        mode='lines+markers',
        name='Medições',
        line=dict(color='#00D4FF', width=2),
        marker=dict(
            size=6,
            color='#00D4FF',
            line=dict(color='rgba(0,212,255,0.5)', width=2)
        )
    ))
    
    # Add out of control points
    if out_of_control.any():
        fig.add_trace(go.Scatter(
            x=data.loc[out_of_control, 'timestamp'],
            y=data.loc[out_of_control, 'value'],
            mode='markers',
            name='Anomalias',
            marker=dict(
                size=12,
                color='#FF4D4D',
                symbol='x',
                line=dict(color='#FF4D4D', width=2)
            )
        ))
    
    theme = get_dark_theme()
    fig.update_layout(
        title=dict(
            text='Carta de Controle X̄',
            font=dict(size=16, color='white')
        ),
        **theme,
        height=350,
        margin=dict(l=50, r=30, t=50, b=50),
        legend=dict(
            orientation='h',
            yanchor='bottom',
            y=1.02,
            xanchor='right',
            x=1,
            font=dict(size=10)
        ),
        hovermode='x unified'
    )
    
    return fig


def create_ema_prediction_chart():
    """Create an EMA prediction chart for Digital Twin visualization"""
    
    np.random.seed(123)
    n_points = 100
    
    # Historical data
    dates = [datetime.now() - timedelta(hours=i) for i in range(n_points)][::-1]
    historical = np.cumsum(np.random.randn(n_points)) + 100
    
    # EMA calculation
    span = 12
    ema = pd.Series(historical).ewm(span=span).mean().values
    
    # Future prediction (last 20 points)
    split_idx = 80
    future_dates = dates[split_idx:]
    prediction = ema[split_idx:]
    
    # Confidence interval
    std = np.std(historical[:split_idx] - ema[:split_idx])
    upper_bound = prediction + 2 * std
    lower_bound = prediction - 2 * std
    
    fig = go.Figure()
    
    # Confidence interval
    fig.add_trace(go.Scatter(
        x=future_dates + future_dates[::-1],
        y=list(upper_bound) + list(lower_bound[::-1]),
        fill='toself',
        fillcolor='rgba(124,58,237,0.1)',
        line=dict(color='rgba(0,0,0,0)'),
        name='Intervalo de Confiança',
        showlegend=True
    ))
    
    # Historical data
    fig.add_trace(go.Scatter(
        x=dates[:split_idx],
        y=historical[:split_idx],
        mode='lines',
        name='Dados Históricos',
        line=dict(color='rgba(255,255,255,0.3)', width=1)
    ))
    
    # EMA line
    fig.add_trace(go.Scatter(
        x=dates,
        y=ema,
        mode='lines',
        name='EMA (Gêmeo Digital)',
        line=dict(color='#00FFE0', width=3)
    ))
    
    # Prediction line
    fig.add_trace(go.Scatter(
        x=future_dates,
        y=prediction,
        mode='lines',
        name='Previsão',
        line=dict(color='#7C3AED', width=2, dash='dot')
    ))
    
    # Add vertical line at prediction start
    fig.add_vline(
        x=dates[split_idx],
        line_width=1,
        line_dash="dash",
        line_color="rgba(255,255,255,0.3)"
    )
    
    theme = get_dark_theme()
    fig.update_layout(
        title=dict(
            text='Gêmeo Digital - Previsão EMA',
            font=dict(size=16, color='white')
        ),
        **theme,
        height=350,
        margin=dict(l=50, r=30, t=50, b=50),
        legend=dict(
            orientation='h',
            yanchor='bottom',
            y=1.02,
            xanchor='right',
            x=1,
            font=dict(size=10)
        ),
        hovermode='x unified'
    )
    
    return fig


def create_anomaly_detection_chart():
    """Create an anomaly detection visualization using Isolation Forest style"""
    
    np.random.seed(456)
    n_points = 200
    
    # Generate normal data
    normal_x = np.random.normal(50, 10, n_points)
    normal_y = np.random.normal(50, 10, n_points)
    
    # Generate anomalies
    n_anomalies = 13
    anomaly_x = np.random.uniform(10, 90, n_anomalies)
    anomaly_y = np.random.uniform(10, 90, n_anomalies)
    
    # Make anomalies more spread out
    for i in range(n_anomalies):
        if np.random.random() > 0.5:
            anomaly_x[i] = np.random.choice([np.random.uniform(5, 20), np.random.uniform(80, 95)])
            anomaly_y[i] = np.random.choice([np.random.uniform(5, 20), np.random.uniform(80, 95)])
    
    fig = go.Figure()
    
    # Normal points
    fig.add_trace(go.Scatter(
        x=normal_x,
        y=normal_y,
        mode='markers',
        name='Operação Normal',
        marker=dict(
            size=8,
            color='#00D4FF',
            opacity=0.6,
            line=dict(color='rgba(0,212,255,0.3)', width=1)
        )
    ))
    
    # Anomaly points
    fig.add_trace(go.Scatter(
        x=anomaly_x,
        y=anomaly_y,
        mode='markers',
        name='Anomalias Detectadas',
        marker=dict(
            size=14,
            color='#FF4D4D',
            symbol='diamond',
            line=dict(color='#FF6B6B', width=2)
        )
    ))
    
    # Add decision boundary circle
    theta = np.linspace(0, 2*np.pi, 100)
    r = 25
    circle_x = 50 + r * np.cos(theta)
    circle_y = 50 + r * np.sin(theta)
    
    fig.add_trace(go.Scatter(
        x=circle_x,
        y=circle_y,
        mode='lines',
        name='Limite de Decisão',
        line=dict(color='rgba(0,255,224,0.3)', width=2, dash='dash')
    ))
    
    theme = get_dark_theme()
    fig.update_layout(
        title=dict(
            text='Detecção de Anomalias - Isolation Forest',
            font=dict(size=16, color='white')
        ),
        **theme,
        height=350,
        margin=dict(l=50, r=30, t=50, b=50),
        legend=dict(
            orientation='h',
            yanchor='bottom',
            y=1.02,
            xanchor='right',
            x=1,
            font=dict(size=10)
        ),
        xaxis_title='Feature 1',
        yaxis_title='Feature 2'
    )
    
    return fig


def create_capability_gauge(value: float, title: str, min_val: float = 0, max_val: float = 2):
    """Create a gauge chart for capability indices"""
    
    # Determine color based on value
    if value >= 1.33:
        color = '#00FF88'
    elif value >= 1.0:
        color = '#FFD700'
    else:
        color = '#FF4D4D'
    
    fig = go.Figure(go.Indicator(
        mode="gauge+number",
        value=value,
        title={'text': title, 'font': {'size': 14, 'color': 'white'}},
        number={'font': {'size': 32, 'color': color}},
        gauge={
            'axis': {
                'range': [min_val, max_val],
                'tickwidth': 1,
                'tickcolor': 'rgba(255,255,255,0.3)',
                'tickfont': {'color': 'rgba(255,255,255,0.5)'}
            },
            'bar': {'color': color},
            'bgcolor': 'rgba(0,0,0,0)',
            'borderwidth': 0,
            'steps': [
                {'range': [0, 1], 'color': 'rgba(255,77,77,0.2)'},
                {'range': [1, 1.33], 'color': 'rgba(255,215,0,0.2)'},
                {'range': [1.33, 2], 'color': 'rgba(0,255,136,0.2)'}
            ],
            'threshold': {
                'line': {'color': 'white', 'width': 2},
                'thickness': 0.75,
                'value': value
            }
        }
    ))
    
    fig.update_layout(
        paper_bgcolor='rgba(0,0,0,0)',
        font={'color': 'white'},
        height=200,
        margin=dict(l=30, r=30, t=50, b=30)
    )
    
    return fig


def create_time_series_kpi():
    """Create a time series chart for KPI monitoring"""
    
    np.random.seed(789)
    n_days = 30
    dates = [datetime.now() - timedelta(days=i) for i in range(n_days)][::-1]
    
    # Simulated KPIs
    precision = 85 + np.cumsum(np.random.randn(n_days) * 0.5)
    precision = np.clip(precision, 80, 95)
    
    fig = go.Figure()
    
    # Fill area under curve
    fig.add_trace(go.Scatter(
        x=dates,
        y=precision,
        fill='tozeroy',
        fillcolor='rgba(0,212,255,0.1)',
        line=dict(color='#00D4FF', width=2),
        name='Precisão do Digital Twin'
    ))
    
    # Add markers for latest points
    fig.add_trace(go.Scatter(
        x=[dates[-1]],
        y=[precision[-1]],
        mode='markers',
        marker=dict(
            size=12,
            color='#00D4FF',
            line=dict(color='white', width=2)
        ),
        name='Atual',
        showlegend=False
    ))
    
    theme = get_dark_theme()
    fig.update_layout(
        title=dict(
            text='Evolução da Precisão - Últimos 30 Dias',
            font=dict(size=16, color='white')
        ),
        **theme,
        height=300,
        margin=dict(l=50, r=30, t=50, b=50),
        yaxis_title='Precisão (%)',
        showlegend=False
    )
    
    return fig


def create_residuals_chart():
    """Create a residuals analysis chart"""
    
    np.random.seed(321)
    n_points = 100
    
    # Simulated residuals (mostly normal with some outliers)
    residuals = np.random.normal(0, 1, n_points)
    residuals[25] = 3.5
    residuals[67] = -3.2
    residuals[88] = 2.8
    
    predicted = np.linspace(90, 110, n_points) + np.random.randn(n_points) * 2
    
    # Identify outliers
    outliers = np.abs(residuals) > 2.5
    
    fig = go.Figure()
    
    # Normal residuals
    fig.add_trace(go.Scatter(
        x=predicted[~outliers],
        y=residuals[~outliers],
        mode='markers',
        name='Resíduos',
        marker=dict(
            size=8,
            color='#00D4FF',
            opacity=0.7
        )
    ))
    
    # Outlier residuals
    fig.add_trace(go.Scatter(
        x=predicted[outliers],
        y=residuals[outliers],
        mode='markers',
        name='Outliers',
        marker=dict(
            size=12,
            color='#FF4D4D',
            symbol='x',
            line=dict(width=2)
        )
    ))
    
    # Zero line
    fig.add_hline(y=0, line_dash="dash", line_color="rgba(255,255,255,0.3)")
    
    # 2-sigma bounds
    fig.add_hline(y=2, line_dash="dot", line_color="rgba(255,215,0,0.5)")
    fig.add_hline(y=-2, line_dash="dot", line_color="rgba(255,215,0,0.5)")
    
    theme = get_dark_theme()
    fig.update_layout(
        title=dict(
            text='Análise de Resíduos',
            font=dict(size=16, color='white')
        ),
        **theme,
        height=300,
        margin=dict(l=50, r=30, t=50, b=50),
        xaxis_title='Valor Previsto',
        yaxis_title='Resíduo',
        legend=dict(
            orientation='h',
            yanchor='bottom',
            y=1.02,
            xanchor='right',
            x=1,
            font=dict(size=10)
        )
    )
    
    return fig
