"""
Market data router for SpeedQuant API.

This module provides API endpoints for accessing market data.
"""

from fastapi import APIRouter, Depends, HTTPException, BackgroundTasks, Query, Path
from typing import Dict, List, Any, Optional
from datetime import datetime, timedelta
import pandas as pd
import numpy as np

from ..models import MarketDataRequestModel, MarketDataSourceType


router = APIRouter()


@router.post("/historical")
async def get_historical_data(request: MarketDataRequestModel) -> Dict[str, Any]:
    """
    Get historical market data.
    
    Args:
        request: Market data request
        
    Returns:
        Historical market data
    """
    # This is a placeholder. In a real system, you would fetch data from a data provider.
    result = {}
    
    # Generate sample data for each symbol
    for symbol in request.symbols:
        # Create a date range
        end_date = request.end_date or datetime.now()
        start_date = request.start_date or (end_date - timedelta(days=30))
        
        # Generate dates
        dates = pd.date_range(start=start_date, end=end_date, freq=request.interval)
        
        # Generate sample data
        data = []
        base_price = 100.0  # Starting price
        for date in dates:
            # Simple random walk
            price_change = np.random.normal(0, 1) * 0.01 * base_price
            open_price = base_price + price_change
            high_price = open_price * (1 + abs(np.random.normal(0, 1) * 0.005))
            low_price = open_price * (1 - abs(np.random.normal(0, 1) * 0.005))
            close_price = (open_price + high_price + low_price) / 3 + np.random.normal(0, 1) * 0.002 * base_price
            volume = int(np.random.normal(1000000, 200000))
            
            # Create bar data
            bar = {
                "timestamp": date.isoformat(),
                "open": round(open_price, 2),
                "high": round(high_price, 2),
                "low": round(low_price, 2),
                "close": round(close_price, 2),
                "volume": volume
            }
            
            # Filter fields if specified
            if request.fields:
                bar = {field: bar[field] for field in request.fields if field in bar}
                
            data.append(bar)
            
            # Update base price for next iteration
            base_price = close_price
            
        result[symbol] = data
    
    return {
        "source": request.source,
        "interval": request.interval,
        "start_date": request.start_date.isoformat() if request.start_date else None,
        "end_date": request.end_date.isoformat() if request.end_date else datetime.now().isoformat(),
        "data": result
    }


@router.get("/realtime")
async def get_realtime_data(
    symbols: str = Query(..., title="Symbols", description="Comma-separated list of symbols"),
    fields: str = Query("last,bid,ask,volume", title="Fields", description="Comma-separated list of fields")
) -> Dict[str, Any]:
    """
    Get real-time market data.
    
    Args:
        symbols: Comma-separated list of symbols
        fields: Comma-separated list of fields
        
    Returns:
        Real-time market data
    """
    # This is a placeholder. In a real system, you would fetch data from a data provider.
    symbol_list = symbols.split(",")
    field_list = fields.split(",")
    
    result = {}
    for symbol in symbol_list:
        # Generate sample data
        base_price = 100.0 + hash(symbol) % 900  # Different base price for different symbols
        last_price = base_price + np.random.normal(0, 1) * 0.01 * base_price
        bid_price = last_price - 0.01
        ask_price = last_price + 0.01
        volume = int(np.random.normal(10000, 2000))
        
        # Create quote data
        quote = {
            "symbol": symbol,
            "timestamp": datetime.now().isoformat(),
            "last": round(last_price, 2),
            "bid": round(bid_price, 2),
            "ask": round(ask_price, 2),
            "volume": volume,
            "change": round(np.random.normal(0, 1) * 0.5, 2),
            "change_percent": round(np.random.normal(0, 1) * 0.5, 2)
        }
        
        # Filter fields if specified
        if field_list:
            quote = {field: quote[field] for field in field_list if field in quote}
            
        result[symbol] = quote
    
    return {
        "type": "realtime",
        "timestamp": datetime.now().isoformat(),
        "data": result
    }


@router.get("/sources")
async def get_data_sources() -> Dict[str, Any]:
    """
    Get available market data sources.
    
    Returns:
        Available market data sources
    """
    return {
        "sources": [
            {
                "id": "local",
                "name": "Local Data",
                "description": "Data stored locally in CSV, Parquet, or HDF5 format",
                "type": "historical",
                "requires_auth": False
            },
            {
                "id": "yahoo",
                "name": "Yahoo Finance",
                "description": "Free historical data from Yahoo Finance",
                "type": "historical",
                "requires_auth": False
            },
            {
                "id": "alpha_vantage",
                "name": "Alpha Vantage",
                "description": "Historical and real-time data from Alpha Vantage",
                "type": "historical,realtime",
                "requires_auth": True
            }
        ]
    }


@router.get("/symbols")
async def search_symbols(
    query: str = Query(..., title="Query", description="Search query"),
    source: MarketDataSourceType = Query(MarketDataSourceType.YAHOO, title="Source")
) -> Dict[str, Any]:
    """
    Search for symbols.
    
    Args:
        query: Search query
        source: Data source
        
    Returns:
        Matching symbols
    """
    # This is a placeholder. In a real system, you would search a database or API.
    symbols = []
    
    # Sample data
    if "AAPL" in query.upper():
        symbols.append({
            "symbol": "AAPL",
            "name": "Apple Inc.",
            "exchange": "NASDAQ",
            "type": "stock"
        })
    
    if "MSFT" in query.upper():
        symbols.append({
            "symbol": "MSFT",
            "name": "Microsoft Corporation",
            "exchange": "NASDAQ",
            "type": "stock"
        })
        
    if "GOOG" in query.upper():
        symbols.append({
            "symbol": "GOOGL",
            "name": "Alphabet Inc.",
            "exchange": "NASDAQ",
            "type": "stock"
        })
        
    # If query is empty or very short, return some default symbols
    if len(query) <= 2:
        symbols = [
            {
                "symbol": "AAPL",
                "name": "Apple Inc.",
                "exchange": "NASDAQ",
                "type": "stock"
            },
            {
                "symbol": "MSFT",
                "name": "Microsoft Corporation",
                "exchange": "NASDAQ",
                "type": "stock"
            },
            {
                "symbol": "GOOGL",
                "name": "Alphabet Inc.",
                "exchange": "NASDAQ",
                "type": "stock"
            },
            {
                "symbol": "AMZN",
                "name": "Amazon.com Inc.",
                "exchange": "NASDAQ",
                "type": "stock"
            },
            {
                "symbol": "TSLA",
                "name": "Tesla Inc.",
                "exchange": "NASDAQ",
                "type": "stock"
            }
        ]
    
    return {
        "query": query,
        "source": source,
        "count": len(symbols),
        "symbols": symbols
    }
