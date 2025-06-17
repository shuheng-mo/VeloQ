"""
Backtesting module for SpeedQuant.

This module provides functionality for backtesting trading strategies.
"""

from .backtest_engine import BacktestEngine
from .performance_metrics import PerformanceMetrics

__all__ = ['BacktestEngine', 'PerformanceMetrics']
