"""
FastAPI application for SpeedQuant.

This module provides the FastAPI application for the SpeedQuant system.
"""

from fastapi import FastAPI, Depends, HTTPException, BackgroundTasks, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from typing import Dict, List, Any, Optional
import uvicorn
import logging
import os
from datetime import datetime

from .models import (
    StrategyModel,
    StrategyCreateModel,
    StrategyUpdateModel,
    OrderModel,
    OrderCreateModel,
    PositionModel,
    BacktestConfigModel,
    BacktestResultModel
)
from .routers import strategies, orders, market_data, backtest, agents


def create_app() -> FastAPI:
    """
    Create and configure the FastAPI application.
    
    Returns:
        FastAPI application
    """
    app = FastAPI(
        title="SpeedQuant API",
        description="API for the SpeedQuant high-performance AI-driven quantitative trading system",
        version="0.1.0"
    )
    
    # Configure CORS
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["*"],  # In production, replace with specific origins
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )
    
    # Include routers
    app.include_router(strategies.router, prefix="/api/strategies", tags=["strategies"])
    app.include_router(orders.router, prefix="/api/orders", tags=["orders"])
    app.include_router(market_data.router, prefix="/api/market-data", tags=["market-data"])
    app.include_router(backtest.router, prefix="/api/backtest", tags=["backtest"])
    app.include_router(agents.router, prefix="/api/agents", tags=["agents"])
    
    @app.get("/api/health")
    async def health_check() -> Dict[str, Any]:
        """
        Health check endpoint.
        
        Returns:
            Health status
        """
        return {
            "status": "ok",
            "timestamp": datetime.now().isoformat(),
            "version": "0.1.0"
        }
    
    @app.get("/api/config")
    async def get_config() -> Dict[str, Any]:
        """
        Get system configuration.
        
        Returns:
            System configuration
        """
        # This is a placeholder. In a real system, you would get the configuration from a config manager.
        return {
            "system": {
                "name": "SpeedQuant",
                "version": "0.1.0",
                "environment": os.environ.get("ENVIRONMENT", "development")
            },
            "logging": {
                "level": os.environ.get("LOG_LEVEL", "INFO")
            },
            "market_data": {
                "sources": ["local", "yahoo", "alpha_vantage"]
            }
        }
    
    return app


if __name__ == "__main__":
    # Run the application directly when this module is executed
    logging.basicConfig(level=logging.INFO)
    uvicorn.run(create_app(), host="0.0.0.0", port=8000)
