"""
API models for SpeedQuant.

This module defines the Pydantic models for the SpeedQuant API.
"""

from pydantic import BaseModel, Field, validator
from typing import Dict, List, Any, Optional, Union
from datetime import datetime
from enum import Enum


class OrderType(str, Enum):
    MARKET = "MARKET"
    LIMIT = "LIMIT"
    STOP = "STOP"
    STOP_LIMIT = "STOP_LIMIT"


class OrderSide(str, Enum):
    BUY = "BUY"
    SELL = "SELL"


class OrderStatus(str, Enum):
    PENDING = "PENDING"
    SUBMITTED = "SUBMITTED"
    PARTIAL_FILLED = "PARTIAL_FILLED"
    FILLED = "FILLED"
    CANCELED = "CANCELED"
    REJECTED = "REJECTED"


class OrderModel(BaseModel):
    """Order model for API responses."""
    order_id: str
    symbol: str
    type: OrderType
    side: OrderSide
    status: OrderStatus
    price: Optional[float] = None
    stop_price: Optional[float] = None
    quantity: float
    filled_quantity: float = 0.0
    avg_fill_price: Optional[float] = None
    create_time: datetime
    update_time: Optional[datetime] = None
    account: str
    strategy_id: Optional[str] = None
    extra_params: Dict[str, Any] = {}


class OrderCreateModel(BaseModel):
    """Order model for creating new orders."""
    symbol: str
    type: OrderType
    side: OrderSide
    price: Optional[float] = None
    stop_price: Optional[float] = None
    quantity: float
    account: str
    strategy_id: Optional[str] = None
    extra_params: Dict[str, Any] = {}


class PositionModel(BaseModel):
    """Position model for API responses."""
    symbol: str
    quantity: float
    avg_price: float
    unrealized_pnl: float
    realized_pnl: float
    account: str
    strategy_id: Optional[str] = None


class StrategyType(str, Enum):
    MOMENTUM = "MOMENTUM"
    MEAN_REVERSION = "MEAN_REVERSION"
    TREND_FOLLOWING = "TREND_FOLLOWING"
    STATISTICAL_ARBITRAGE = "STATISTICAL_ARBITRAGE"
    MACHINE_LEARNING = "MACHINE_LEARNING"
    CUSTOM = "CUSTOM"


class StrategyStatus(str, Enum):
    ACTIVE = "ACTIVE"
    PAUSED = "PAUSED"
    STOPPED = "STOPPED"
    ERROR = "ERROR"


class StrategyModel(BaseModel):
    """Strategy model for API responses."""
    id: str
    name: str
    type: StrategyType
    status: StrategyStatus
    symbols: List[str]
    parameters: Dict[str, Any] = {}
    created_at: datetime
    updated_at: datetime
    description: Optional[str] = None
    performance: Dict[str, Any] = {}


class StrategyCreateModel(BaseModel):
    """Strategy model for creating new strategies."""
    name: str
    type: StrategyType
    symbols: List[str]
    parameters: Dict[str, Any] = {}
    description: Optional[str] = None


class StrategyUpdateModel(BaseModel):
    """Strategy model for updating existing strategies."""
    name: Optional[str] = None
    status: Optional[StrategyStatus] = None
    symbols: Optional[List[str]] = None
    parameters: Optional[Dict[str, Any]] = None
    description: Optional[str] = None


class BacktestConfigModel(BaseModel):
    """Backtest configuration model."""
    strategy_id: str
    start_date: datetime
    end_date: datetime
    initial_capital: float = 100000.0
    commission: float = 0.001
    slippage: float = 0.0
    data_source: str
    symbols: List[str]
    parameters: Dict[str, Any] = {}


class BacktestResultModel(BaseModel):
    """Backtest result model."""
    id: str
    strategy_id: str
    config: BacktestConfigModel
    start_time: datetime
    end_time: datetime
    status: str
    metrics: Dict[str, Any]
    equity_curve: List[Dict[str, Any]]
    trades: List[Dict[str, Any]]


class MarketDataSourceType(str, Enum):
    LOCAL = "LOCAL"
    YAHOO = "YAHOO"
    ALPHA_VANTAGE = "ALPHA_VANTAGE"
    CUSTOM = "CUSTOM"


class MarketDataRequestModel(BaseModel):
    """Market data request model."""
    symbols: List[str]
    source: MarketDataSourceType
    start_date: Optional[datetime] = None
    end_date: Optional[datetime] = None
    interval: str = "1d"
    fields: List[str] = ["open", "high", "low", "close", "volume"]


class AgentType(str, Enum):
    PORTFOLIO_OPTIMIZER = "PORTFOLIO_OPTIMIZER"
    NEWS_PARSER = "NEWS_PARSER"
    STRATEGY_GENERATOR = "STRATEGY_GENERATOR"
    REVIEW = "REVIEW"


class AgentRequestModel(BaseModel):
    """Agent request model."""
    agent_type: AgentType
    parameters: Dict[str, Any] = {}
    context: Dict[str, Any] = {}


class AgentResponseModel(BaseModel):
    """Agent response model."""
    agent_type: AgentType
    result: Dict[str, Any]
    execution_time: float
    timestamp: datetime
