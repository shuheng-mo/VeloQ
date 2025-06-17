"""
Moving Average Crossover Strategy Template

This is a template for a simple moving average crossover strategy.
"""

from typing import Dict, List, Any, Optional
import pandas as pd
import numpy as np
from datetime import datetime

from ..base_strategy import BaseStrategy


class MovingAverageCrossoverStrategy(BaseStrategy):
    """
    Moving Average Crossover Strategy
    
    This strategy generates buy signals when the short-term moving average crosses above
    the long-term moving average, and sell signals when the short-term moving average
    crosses below the long-term moving average.
    """
    
    def __init__(self, name: str, symbols: List[str], parameters: Optional[Dict[str, Any]] = None):
        """
        Initialize the strategy.
        
        Args:
            name: Strategy name
            symbols: List of symbols to trade
            parameters: Dictionary of strategy parameters
                - short_window: Short-term moving average window (default: 20)
                - long_window: Long-term moving average window (default: 50)
                - quantity: Trading quantity per signal (default: 1.0)
        """
        default_params = {
            'short_window': 20,
            'long_window': 50,
            'quantity': 1.0
        }
        
        if parameters:
            default_params.update(parameters)
            
        super().__init__(name, symbols, default_params)
        
        # Initialize data storage for each symbol
        self.data = {symbol: pd.DataFrame(columns=['timestamp', 'price', 'volume']) for symbol in symbols}
        self.signals = {symbol: None for symbol in symbols}
        self.last_position = {symbol: 0 for symbol in symbols}
        
    def on_tick(self, tick: Dict[str, Any]) -> None:
        """
        Process incoming tick data.
        
        Args:
            tick: Tick data containing symbol, price, volume, etc.
        """
        symbol = tick['symbol']
        if symbol not in self.symbols:
            return
            
        # Append tick data to the dataframe
        new_row = pd.DataFrame({
            'timestamp': [pd.Timestamp(tick['timestamp'])],
            'price': [tick['price']],
            'volume': [tick['volume']]
        })
        
        self.data[symbol] = pd.concat([self.data[symbol], new_row], ignore_index=True)
        
        # We don't generate signals on every tick to avoid excessive trading
        # Instead, we'll rely on the on_bar method for signal generation
        
    def on_bar(self, bar: Dict[str, Any]) -> None:
        """
        Process incoming bar data and generate trading signals.
        
        Args:
            bar: Bar data containing symbol, open, high, low, close, volume, etc.
        """
        symbol = bar['symbol']
        if symbol not in self.symbols:
            return
            
        # Get parameters
        short_window = self.parameters['short_window']
        long_window = self.parameters['long_window']
        quantity = self.parameters['quantity']
        
        # Append bar data
        new_row = pd.DataFrame({
            'timestamp': [pd.Timestamp(bar['timestamp'])],
            'price': [bar['close']],
            'volume': [bar['volume']]
        })
        
        self.data[symbol] = pd.concat([self.data[symbol], new_row], ignore_index=True)
        
        # Calculate moving averages
        df = self.data[symbol]
        if len(df) >= long_window:
            # Calculate short and long moving averages
            df['short_ma'] = df['price'].rolling(window=short_window).mean()
            df['long_ma'] = df['price'].rolling(window=long_window).mean()
            
            # Generate signals: 1 for buy, -1 for sell, 0 for hold
            df['signal'] = 0.0
            df['signal'][short_window:] = np.where(
                df['short_ma'][short_window:] > df['long_ma'][short_window:], 1.0, 0.0
            )
            df['position'] = df['signal'].diff()
            
            # Get the current position signal
            current_position = df['position'].iloc[-1]
            
            # Execute trades based on position changes
            if current_position > 0:  # Buy signal
                if self.get_position(symbol) <= 0:  # Only buy if we don't have a long position
                    self.buy(symbol, quantity)
                    print(f"[{datetime.now()}] BUY {symbol}: {quantity} @ {bar['close']}")
            elif current_position < 0:  # Sell signal
                if self.get_position(symbol) >= 0:  # Only sell if we don't have a short position
                    self.sell(symbol, quantity)
                    print(f"[{datetime.now()}] SELL {symbol}: {quantity} @ {bar['close']}")
            
            # Store the current position for the next iteration
            self.last_position[symbol] = current_position
    
    def on_start(self) -> None:
        """Called when the strategy is started."""
        super().on_start()
        print(f"[{datetime.now()}] Strategy {self.name} started with parameters: {self.parameters}")
        
    def on_stop(self) -> None:
        """Called when the strategy is stopped."""
        super().on_stop()
        print(f"[{datetime.now()}] Strategy {self.name} stopped")
