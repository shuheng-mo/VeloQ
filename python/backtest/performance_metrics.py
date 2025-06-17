"""
Performance Metrics for SpeedQuant backtesting.

This module provides functionality for calculating performance metrics for trading strategies.
"""

from typing import Dict, List, Any, Optional, Union
import pandas as pd
import numpy as np
from datetime import datetime
import math


class PerformanceMetrics:
    """
    Calculate performance metrics for trading strategies.
    
    This class provides methods for calculating various performance metrics
    such as returns, drawdowns, Sharpe ratio, etc.
    """
    
    def __init__(self, 
                 initial_capital: float,
                 portfolio_history: List[Dict[str, Any]],
                 trades: List[Dict[str, Any]]):
        """
        Initialize the performance metrics calculator.
        
        Args:
            initial_capital: Initial capital
            portfolio_history: List of portfolio snapshots
            trades: List of executed trades
        """
        self.initial_capital = initial_capital
        self.portfolio_history = pd.DataFrame(portfolio_history)
        self.trades = pd.DataFrame(trades) if trades else pd.DataFrame()
        
        # Convert timestamp to datetime if it's not already
        if not self.portfolio_history.empty and 'timestamp' in self.portfolio_history.columns:
            self.portfolio_history['timestamp'] = pd.to_datetime(self.portfolio_history['timestamp'])
            
        if not self.trades.empty and 'timestamp' in self.trades.columns:
            self.trades['timestamp'] = pd.to_datetime(self.trades['timestamp'])
            
    def calculate_returns(self) -> Dict[str, float]:
        """
        Calculate various return metrics.
        
        Returns:
            Dictionary containing return metrics
        """
        if self.portfolio_history.empty:
            return {
                'total_return': 0.0,
                'total_return_pct': 0.0,
                'annualized_return': 0.0,
                'daily_returns': []
            }
            
        # Calculate total return
        final_equity = self.portfolio_history['equity'].iloc[-1]
        total_return = final_equity - self.initial_capital
        total_return_pct = (total_return / self.initial_capital) * 100
        
        # Calculate daily returns
        self.portfolio_history['date'] = self.portfolio_history['timestamp'].dt.date
        daily_equity = self.portfolio_history.groupby('date')['equity'].last().reset_index()
        daily_equity['prev_equity'] = daily_equity['equity'].shift(1)
        daily_equity['daily_return'] = (daily_equity['equity'] / daily_equity['prev_equity'] - 1) * 100
        daily_equity = daily_equity.dropna()
        
        # Calculate annualized return
        if len(daily_equity) > 1:
            days = (daily_equity['date'].iloc[-1] - daily_equity['date'].iloc[0]).days
            if days > 0:
                annualized_return = ((1 + total_return_pct / 100) ** (365 / days) - 1) * 100
            else:
                annualized_return = 0.0
        else:
            annualized_return = 0.0
            
        return {
            'total_return': total_return,
            'total_return_pct': total_return_pct,
            'annualized_return': annualized_return,
            'daily_returns': daily_equity['daily_return'].tolist()
        }
        
    def calculate_drawdowns(self) -> Dict[str, float]:
        """
        Calculate drawdown metrics.
        
        Returns:
            Dictionary containing drawdown metrics
        """
        if self.portfolio_history.empty:
            return {
                'max_drawdown': 0.0,
                'max_drawdown_pct': 0.0,
                'avg_drawdown': 0.0,
                'avg_drawdown_pct': 0.0
            }
            
        # Calculate running maximum
        self.portfolio_history['running_max'] = self.portfolio_history['equity'].cummax()
        
        # Calculate drawdown in absolute terms and percentage
        self.portfolio_history['drawdown'] = self.portfolio_history['running_max'] - self.portfolio_history['equity']
        self.portfolio_history['drawdown_pct'] = (self.portfolio_history['drawdown'] / self.portfolio_history['running_max']) * 100
        
        # Calculate maximum drawdown
        max_drawdown = self.portfolio_history['drawdown'].max()
        max_drawdown_pct = self.portfolio_history['drawdown_pct'].max()
        
        # Calculate average drawdown (only considering periods in drawdown)
        drawdown_periods = self.portfolio_history[self.portfolio_history['drawdown'] > 0]
        avg_drawdown = drawdown_periods['drawdown'].mean() if not drawdown_periods.empty else 0.0
        avg_drawdown_pct = drawdown_periods['drawdown_pct'].mean() if not drawdown_periods.empty else 0.0
        
        return {
            'max_drawdown': max_drawdown,
            'max_drawdown_pct': max_drawdown_pct,
            'avg_drawdown': avg_drawdown,
            'avg_drawdown_pct': avg_drawdown_pct
        }
        
    def calculate_ratios(self) -> Dict[str, float]:
        """
        Calculate performance ratios.
        
        Returns:
            Dictionary containing performance ratios
        """
        if self.portfolio_history.empty:
            return {
                'sharpe_ratio': 0.0,
                'sortino_ratio': 0.0,
                'calmar_ratio': 0.0
            }
            
        # Calculate daily returns if not already done
        if 'daily_return' not in self.portfolio_history.columns:
            self.portfolio_history['date'] = self.portfolio_history['timestamp'].dt.date
            daily_equity = self.portfolio_history.groupby('date')['equity'].last().reset_index()
            daily_equity['prev_equity'] = daily_equity['equity'].shift(1)
            daily_equity['daily_return'] = daily_equity['equity'] / daily_equity['prev_equity'] - 1
            daily_equity = daily_equity.dropna()
        else:
            daily_equity = self.portfolio_history[['date', 'daily_return']].copy()
            
        if daily_equity.empty:
            return {
                'sharpe_ratio': 0.0,
                'sortino_ratio': 0.0,
                'calmar_ratio': 0.0
            }
            
        # Calculate Sharpe ratio (assuming risk-free rate of 0)
        returns = daily_equity['daily_return'].values
        mean_return = np.mean(returns)
        std_return = np.std(returns)
        sharpe_ratio = (mean_return / std_return) * np.sqrt(252) if std_return > 0 else 0.0
        
        # Calculate Sortino ratio (using only negative returns for denominator)
        negative_returns = returns[returns < 0]
        downside_std = np.std(negative_returns) if len(negative_returns) > 0 else 0.0001
        sortino_ratio = (mean_return / downside_std) * np.sqrt(252) if downside_std > 0 else 0.0
        
        # Calculate Calmar ratio (annualized return / maximum drawdown)
        drawdowns = self.calculate_drawdowns()
        max_drawdown_pct = drawdowns['max_drawdown_pct']
        returns_data = self.calculate_returns()
        annualized_return = returns_data['annualized_return']
        calmar_ratio = annualized_return / max_drawdown_pct if max_drawdown_pct > 0 else 0.0
        
        return {
            'sharpe_ratio': sharpe_ratio,
            'sortino_ratio': sortino_ratio,
            'calmar_ratio': calmar_ratio
        }
        
    def calculate_trade_metrics(self) -> Dict[str, Any]:
        """
        Calculate trade-related metrics.
        
        Returns:
            Dictionary containing trade metrics
        """
        if self.trades.empty:
            return {
                'total_trades': 0,
                'win_rate': 0.0,
                'avg_profit': 0.0,
                'avg_loss': 0.0,
                'profit_factor': 0.0,
                'avg_trade': 0.0
            }
            
        # Calculate basic trade metrics
        total_trades = len(self.trades)
        
        # Calculate profit/loss for each trade
        self.trades['profit_loss'] = 0.0
        
        # Group trades by order_id to handle partial fills
        buy_trades = self.trades[self.trades['side'] == 'BUY'].copy()
        sell_trades = self.trades[self.trades['side'] == 'SELL'].copy()
        
        # Simple P&L calculation (this is a simplification - in a real system you'd match buys and sells)
        buy_trades['cost'] = buy_trades['value'] + buy_trades['commission']
        sell_trades['revenue'] = sell_trades['value'] - sell_trades['commission']
        
        # Calculate profit/loss (this is simplified)
        total_buy_cost = buy_trades['cost'].sum() if not buy_trades.empty else 0
        total_sell_revenue = sell_trades['revenue'].sum() if not sell_trades.empty else 0
        total_profit_loss = total_sell_revenue - total_buy_cost
        
        # Calculate win rate (simplified - in a real system you'd match trades)
        # Here we're just checking if the overall P&L is positive
        win_rate = 100.0 if total_profit_loss > 0 else 0.0
        
        # Other metrics
        avg_trade = total_profit_loss / total_trades if total_trades > 0 else 0.0
        
        return {
            'total_trades': total_trades,
            'win_rate': win_rate,
            'total_profit_loss': total_profit_loss,
            'avg_trade': avg_trade
        }
        
    def calculate_all(self) -> Dict[str, Any]:
        """
        Calculate all performance metrics.
        
        Returns:
            Dictionary containing all performance metrics
        """
        returns = self.calculate_returns()
        drawdowns = self.calculate_drawdowns()
        ratios = self.calculate_ratios()
        trade_metrics = self.calculate_trade_metrics()
        
        # Combine all metrics
        all_metrics = {
            **returns,
            **drawdowns,
            **ratios,
            **trade_metrics
        }
        
        return all_metrics
