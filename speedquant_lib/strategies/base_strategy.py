"""
Base Strategy class for SpeedQuant.

This module defines the BaseStrategy class that all strategies should inherit from.
"""

from abc import ABC, abstractmethod
from typing import Dict, List, Optional, Any, Union
import pandas as pd
import numpy as np
from datetime import datetime


class BaseStrategy(ABC):
    """Base class for all trading strategies in SpeedQuant."""
    
    def __init__(self, name: str, symbols: List[str], parameters: Dict[str, Any] = None):
        """
        Initialize a new strategy.
        
        Args:
            name: Strategy name
            symbols: List of symbols to trade
            parameters: Dictionary of strategy parameters
        """
        self.name = name
        self.symbols = symbols
        self.parameters = parameters or {}
        self.positions: Dict[str, float] = {symbol: 0.0 for symbol in symbols}
        self.is_running = False
        self.last_update_time = datetime.now()
        
    @abstractmethod
    def on_tick(self, tick: Dict[str, Any]) -> None:
        """
        Called when a new tick is received.
        
        Args:
            tick: Tick data containing symbol, price, volume, etc.
        """
        pass
    
    @abstractmethod
    def on_bar(self, bar: Dict[str, Any]) -> None:
        """
        Called when a new bar is formed.
        
        Args:
            bar: Bar data containing symbol, open, high, low, close, volume, etc.
        """
        pass
    
    def on_order_update(self, order: Dict[str, Any]) -> None:
        """
        Called when an order status is updated.
        
        Args:
            order: Order data containing order_id, status, filled_quantity, etc.
        """
        pass
    
    def on_position_update(self, position: Dict[str, Any]) -> None:
        """
        Called when a position is updated.
        
        Args:
            position: Position data containing symbol, quantity, avg_price, etc.
        """
        self.positions[position['symbol']] = position['quantity']
    
    def on_start(self) -> None:
        """Called when the strategy is started."""
        self.is_running = True
        self.last_update_time = datetime.now()
        
    def on_stop(self) -> None:
        """Called when the strategy is stopped."""
        self.is_running = False
    
    def buy(self, symbol: str, quantity: float, price: Optional[float] = None, 
            order_type: str = 'MARKET') -> str:
        """
        Place a buy order.
        
        Args:
            symbol: Symbol to buy
            quantity: Quantity to buy
            price: Price for limit orders
            order_type: Order type (MARKET, LIMIT, etc.)
            
        Returns:
            order_id: ID of the placed order
        """
        # This is a placeholder. The actual implementation will be provided by the strategy engine.
        return "order_id"
    
    def sell(self, symbol: str, quantity: float, price: Optional[float] = None, 
             order_type: str = 'MARKET') -> str:
        """
        Place a sell order.
        
        Args:
            symbol: Symbol to sell
            quantity: Quantity to sell
            price: Price for limit orders
            order_type: Order type (MARKET, LIMIT, etc.)
            
        Returns:
            order_id: ID of the placed order
        """
        # This is a placeholder. The actual implementation will be provided by the strategy engine.
        return "order_id"
    
    def cancel_order(self, order_id: str) -> bool:
        """
        Cancel an order.
        
        Args:
            order_id: ID of the order to cancel
            
        Returns:
            success: Whether the cancellation was successful
        """
        # This is a placeholder. The actual implementation will be provided by the strategy engine.
        return True
    
    def get_position(self, symbol: str) -> float:
        """
        Get the current position for a symbol.
        
        Args:
            symbol: Symbol to get position for
            
        Returns:
            quantity: Current position quantity
        """
        return self.positions.get(symbol, 0.0)
    
    def get_parameters(self) -> Dict[str, Any]:
        """
        Get the strategy parameters.
        
        Returns:
            parameters: Dictionary of strategy parameters
        """
        return self.parameters
    
    def set_parameters(self, parameters: Dict[str, Any]) -> None:
        """
        Set the strategy parameters.
        
        Args:
            parameters: Dictionary of strategy parameters
        """
        self.parameters.update(parameters)
