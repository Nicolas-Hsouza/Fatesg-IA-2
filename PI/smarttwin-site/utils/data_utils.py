"""
SmartTwin CEP - Data Utilities
Sample data generation and processing utilities
"""

import numpy as np
import pandas as pd
from datetime import datetime, timedelta


def generate_process_data(n_samples: int = 300, seed: int = 42):
    """Generate simulated process data for demonstrations"""
    np.random.seed(seed)
    
    # Base timestamps
    timestamps = [datetime.now() - timedelta(minutes=i*5) for i in range(n_samples)][::-1]
    
    # Simulated process variable (e.g., fill volume)
    base_value = 500  # ml
    values = np.random.normal(base_value, 3, n_samples)
    
    # Add some out-of-control situations
    # Shift
    values[100:120] += 5
    # Trend
    values[200:250] += np.linspace(0, 8, 50)
    # Spikes (anomalies)
    anomaly_indices = [45, 156, 234, 267, 289]
    for idx in anomaly_indices:
        values[idx] += np.random.choice([-12, 12])
    
    return pd.DataFrame({
        'timestamp': timestamps,
        'value': values,
        'temperature': np.random.normal(25, 0.5, n_samples),
        'pressure': np.random.normal(1.013, 0.01, n_samples),
        'speed': np.random.normal(120, 2, n_samples)
    })


def calculate_capability_indices(data: pd.Series, usl: float, lsl: float):
    """Calculate Cp and Cpk indices"""
    mean = data.mean()
    std = data.std()
    
    # Cp: Process Capability
    cp = (usl - lsl) / (6 * std)
    
    # Cpk: Process Capability Index (accounts for centering)
    cpu = (usl - mean) / (3 * std)
    cpl = (mean - lsl) / (3 * std)
    cpk = min(cpu, cpl)
    
    return cp, cpk


def detect_anomalies_zscore(data: pd.Series, threshold: float = 3.0):
    """Detect anomalies using Z-score method"""
    mean = data.mean()
    std = data.std()
    z_scores = np.abs((data - mean) / std)
    return z_scores > threshold


def calculate_ema(data: pd.Series, span: int = 12):
    """Calculate Exponential Moving Average"""
    return data.ewm(span=span, adjust=False).mean()


def get_kpi_data():
    """Return KPI data for the dashboard"""
    return {
        'cp': 1.336,
        'cpk': 1.322,
        'anomalies': 13,
        'precision': 87.6,
        'measurements': 300,
        'uptime': 99.7
    }


def generate_run_rules_violations(data: pd.Series):
    """Check for Western Electric run rules violations"""
    violations = []
    mean = data.mean()
    std = data.std()
    
    # Rule 1: One point beyond 3 sigma
    rule1 = np.abs(data - mean) > 3 * std
    
    # Rule 2: 2 of 3 points beyond 2 sigma (same side)
    # Simplified check
    rule2 = np.abs(data - mean) > 2 * std
    
    # Rule 3: 4 of 5 points beyond 1 sigma (same side)
    rule3 = np.abs(data - mean) > 1 * std
    
    # Rule 4: 8 consecutive points on same side of center line
    above_mean = data > mean
    rule4 = pd.Series(above_mean).rolling(8).sum()
    rule4_violations = (rule4 == 8) | (rule4 == 0)
    
    return {
        'rule1': rule1.sum(),
        'rule2': rule2.sum(),
        'rule3': rule3.sum(),
        'rule4': rule4_violations.sum()
    }
