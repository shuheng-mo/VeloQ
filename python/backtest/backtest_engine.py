"""
Backtesting Engine for SpeedQuant.

This module provides the BacktestEngine class for backtesting trading strategies.
"""

from typing import Dict, List, Any, Optional, Union, Callable
import pandas as pd
import numpy as np
from datetime import datetime, timedelta
import os
import json
import logging
from pathlib import Path

from ..strategies.base_strategy import BaseStrategy
from .performance_metrics import PerformanceMetrics


class BacktestEngine:
    """
    Backtesting engine for SpeedQuant.
    
    This class provides functionality for backtesting trading strategies using
    historical market data.
    """
    
    def __init__(self, 
                 strategy: BaseStrategy, 
                 data_source: Union[str, Dict[str, pd.DataFrame]], 
                 start_date: Optional[Union[str, datetime]] = None,
                 end_date: Optional[Union[str, datetime]] = None,
                 initial_capital: float = 100000.0,
                 commission: float = 0.001,
                 slippage: float = 0.0):
        """
        Initialize the backtesting engine.
        
        Args:
            strategy: The strategy to backtest
            data_source: Path to data files or dictionary of DataFrames with historical data
            start_date: Start date for the backtest (optional)
            end_date: End date for the backtest (optional)
            initial_capital: Initial capital for the backtest
            commission: Commission rate (as a fraction of trade value)
            slippage: Slippage (as a fraction of price)
        """
        self.strategy = strategy
        self.initial_capital = initial_capital
        self.commission = commission
        self.slippage = slippage
        
        # Initialize portfolio and performance tracking
        self.portfolio = {
            'cash': initial_capital,
            'positions': {symbol: 0.0 for symbol in strategy.symbols},
            'equity': initial_capital,
            'history': []
        }
        
        # Load data
        self.data = {}
        if isinstance(data_source, str):
            self._load_data_from_files(data_source, strategy.symbols)
        else:
            self.data = data_source
            
        # Filter data by date range if specified
        if start_date or end_date:
            self._filter_data_by_date(start_date, end_date)
            
        # Initialize order book and trade history
        self.orders = []
        self.trades = []
        
        # Set up logging
        self.logger = logging.getLogger('speedquant.backtest')
        
    def _load_data_from_files(self, data_dir: str, symbols: List[str]) -> None:
        """
        Load historical data from files.
        
        Args:
            data_dir: Directory containing data files
            symbols: List of symbols to load data for
        """
        for symbol in symbols:
            # Try different file extensions
            for ext in ['.csv', '.parquet', '.h5']:
                file_path = os.path.join(data_dir, f"{symbol}{ext}")
                if os.path.exists(file_path):
                    if ext == '.csv':
                        df = pd.read_csv(file_path)
                    elif ext == '.parquet':
                        df = pd.read_parquet(file_path)
                    elif ext == '.h5':
                        df = pd.read_hdf(file_path)
                        
                    # Ensure timestamp column is datetime
                    if 'timestamp' in df.columns:
                        df['timestamp'] = pd.to_datetime(df['timestamp'])
                    elif 'date' in df.columns:
                        df['timestamp'] = pd.to_datetime(df['date'])
                        
                    # Sort by timestamp
                    df = df.sort_values('timestamp')
                    
                    self.data[symbol] = df
                    break
            
            if symbol not in self.data:
                self.logger.warning(f"No data found for symbol {symbol}")
                
    def _filter_data_by_date(self, start_date: Optional[Union[str, datetime]], 
                            end_date: Optional[Union[str, datetime]]) -> None:
        """
        Filter data by date range.
        
        Args:
            start_date: Start date
            end_date: End date
        """
        if start_date:
            start_date = pd.to_datetime(start_date)
            for symbol in self.data:
                self.data[symbol] = self.data[symbol][self.data[symbol]['timestamp'] >= start_date]
                
        if end_date:
            end_date = pd.to_datetime(end_date)
            for symbol in self.data:
                self.data[symbol] = self.data[symbol][self.data[symbol]['timestamp'] <= end_date]
                
    def run(self) -> Dict[str, Any]:
        """
        Run the backtest.
        
        Returns:
            results: Dictionary containing backtest results
        """
        # Start the strategy
        self.strategy.on_start()
        
        # Override strategy's buy/sell methods
        original_buy = self.strategy.buy
        original_sell = self.strategy.sell
        
        self.strategy.buy = lambda symbol, quantity, price=None, order_type='MARKET': self._execute_buy(symbol, quantity, price, order_type)
        self.strategy.sell = lambda symbol, quantity, price=None, order_type='MARKET': self._execute_sell(symbol, quantity, price, order_type)
        
        # Combine all data and sort by timestamp
        all_data = []
        for symbol, df in self.data.items():
            df_copy = df.copy()
            df_copy['symbol'] = symbol
            all_data.append(df_copy)
            
        if not all_data:
            self.logger.error("No data available for backtest")
            return {'success': False, 'error': 'No data available'}
            
        combined_data = pd.concat(all_data).sort_values('timestamp')
        
        # Process each data point
        for _, row in combined_data.iterrows():
            symbol = row['symbol']
            
            # Create bar data
            if all(col in row.index for col in ['open', 'high', 'low', 'close', 'volume']):
                bar = {
                    'symbol': symbol,
                    'timestamp': row['timestamp'],
                    'open': row['open'],
                    'high': row['high'],
                    'low': row['low'],
                    'close': row['close'],
                    'volume': row['volume']
                }
                self.strategy.on_bar(bar)
            
            # Create tick data
            tick = {
                'symbol': symbol,
                'timestamp': row['timestamp'],
                'price': row['close'] if 'close' in row else row['price'],
                'volume': row['volume'] if 'volume' in row else 0
            }
            self.strategy.on_tick(tick)
            
            # Update portfolio value
            self._update_portfolio(row['timestamp'])
            
        # Restore original methods
        self.strategy.buy = original_buy
        self.strategy.sell = original_sell
        
        # Stop the strategy
        self.strategy.on_stop()
        
        # Calculate performance metrics
        metrics = PerformanceMetrics(
            initial_capital=self.initial_capital,
            portfolio_history=self.portfolio['history'],
            trades=self.trades
        )
        
        results = {
            'success': True,
            'portfolio': self.portfolio,
            'trades': self.trades,
            'orders': self.orders,
            'metrics': metrics.calculate_all(),
            'equity_curve': pd.DataFrame(self.portfolio['history'])
        }
        
        return results
    
    def _execute_buy(self, symbol: str, quantity: float, price: Optional[float] = None, 
                    order_type: str = 'MARKET') -> str:
        """
        Execute a buy order in the backtest.
        
        Args:
            symbol: Symbol to buy
            quantity: Quantity to buy
            price: Price for limit orders
            order_type: Order type (MARKET, LIMIT, etc.)
            
        Returns:
            order_id: ID of the placed order
        """
        if symbol not in self.data:
            self.logger.warning(f"Symbol {symbol} not found in data")
            return ""
            
        # Get the latest data point for the symbol
        latest_data = self.data[symbol].iloc[-1]
        execution_price = price if price else latest_data['close']
        
        # Apply slippage
        execution_price *= (1 + self.slippage)
        
        # Calculate order value and commission
        order_value = execution_price * quantity
        commission_cost = order_value * self.commission
        
        # Check if we have enough cash
        total_cost = order_value + commission_cost
        if self.portfolio['cash'] < total_cost:
            self.logger.warning(f"Not enough cash to execute buy order: {total_cost} required, {self.portfolio['cash']} available")
            return ""
            
        # Create order
        order_id = f"BUY-{symbol}-{datetime.now().timestamp()}"
        order = {
            'id': order_id,
            'symbol': symbol,
            'side': 'BUY',
            'quantity': quantity,
            'price': execution_price,
            'timestamp': latest_data['timestamp'],
            'status': 'FILLED',
            'commission': commission_cost
        }
        self.orders.append(order)
        
        # Update portfolio
        self.portfolio['cash'] -= total_cost
        self.portfolio['positions'][symbol] += quantity
        
        # Record trade
        trade = {
            'order_id': order_id,
            'symbol': symbol,
            'side': 'BUY',
            'quantity': quantity,
            'price': execution_price,
            'timestamp': latest_data['timestamp'],
            'commission': commission_cost,
            'value': order_value
        }
        self.trades.append(trade)
        
        # Update strategy's position
        self.strategy.on_position_update({
            'symbol': symbol,
            'quantity': self.portfolio['positions'][symbol]
        })
        
        return order_id
    
    def _execute_sell(self, symbol: str, quantity: float, price: Optional[float] = None, 
                     order_type: str = 'MARKET') -> str:
        """
        Execute a sell order in the backtest.
        
        Args:
            symbol: Symbol to sell
            quantity: Quantity to sell
            price: Price for limit orders
            order_type: Order type (MARKET, LIMIT, etc.)
            
        Returns:
            order_id: ID of the placed order
        """
        if symbol not in self.data:
            self.logger.warning(f"Symbol {symbol} not found in data")
            return ""
            
        # Check if we have enough position
        current_position = self.portfolio['positions'][symbol]
        if current_position < quantity:
            self.logger.warning(f"Not enough position to execute sell order: {quantity} required, {current_position} available")
            return ""
            
        # Get the latest data point for the symbol
        latest_data = self.data[symbol].iloc[-1]
        execution_price = price if price else latest_data['close']
        
        # Apply slippage
        execution_price *= (1 - self.slippage)
        
        # Calculate order value and commission
        order_value = execution_price * quantity
        commission_cost = order_value * self.commission
        
        # Create order
        order_id = f"SELL-{symbol}-{datetime.now().timestamp()}"
        order = {
            'id': order_id,
            'symbol': symbol,
            'side': 'SELL',
            'quantity': quantity,
            'price': execution_price,
            'timestamp': latest_data['timestamp'],
            'status': 'FILLED',
            'commission': commission_cost
        }
        self.orders.append(order)
        
        # Update portfolio
        self.portfolio['cash'] += (order_value - commission_cost)
        self.portfolio['positions'][symbol] -= quantity
        
        # Record trade
        trade = {
            'order_id': order_id,
            'symbol': symbol,
            'side': 'SELL',
            'quantity': quantity,
            'price': execution_price,
            'timestamp': latest_data['timestamp'],
            'commission': commission_cost,
            'value': order_value
        }
        self.trades.append(trade)
        
        # Update strategy's position
        self.strategy.on_position_update({
            'symbol': symbol,
            'quantity': self.portfolio['positions'][symbol]
        })
        
        return order_id
    
    def _update_portfolio(self, timestamp: datetime) -> None:
        """
        Update portfolio value at a given timestamp.
        
        Args:
            timestamp: Current timestamp
        """
        equity = self.portfolio['cash']
        
        # Add value of positions
        for symbol, quantity in self.portfolio['positions'].items():
            if quantity > 0 and symbol in self.data:
                # Get the latest price for the symbol
                symbol_data = self.data[symbol]
                symbol_data = symbol_data[symbol_data['timestamp'] <= timestamp]
                
                if not symbol_data.empty:
                    latest_price = symbol_data.iloc[-1]['close']
                    equity += quantity * latest_price
        
        # Update portfolio equity
        self.portfolio['equity'] = equity
        
        # Add to history
        self.portfolio['history'].append({
            'timestamp': timestamp,
            'equity': equity,
            'cash': self.portfolio['cash'],
            'positions': self.portfolio['positions'].copy()
        })
    
    def save_results(self, output_dir: str) -> None:
        """
        Save backtest results to files.
        
        Args:
            output_dir: Directory to save results to
        """
        # Create output directory if it doesn't exist
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        
        # Save portfolio history
        pd.DataFrame(self.portfolio['history']).to_csv(
            os.path.join(output_dir, 'portfolio_history.csv'), index=False
        )
        
        # Save trades
        pd.DataFrame(self.trades).to_csv(
            os.path.join(output_dir, 'trades.csv'), index=False
        )
        
        # Save orders
        pd.DataFrame(self.orders).to_csv(
            os.path.join(output_dir, 'orders.csv'), index=False
        )
        
        # Save metrics
        metrics = PerformanceMetrics(
            initial_capital=self.initial_capital,
            portfolio_history=self.portfolio['history'],
            trades=self.trades
        )
        
        with open(os.path.join(output_dir, 'metrics.json'), 'w') as f:
            json.dump(metrics.calculate_all(), f, indent=4)
            
        self.logger.info(f"Backtest results saved to {output_dir}")
