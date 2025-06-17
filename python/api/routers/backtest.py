"""
Backtest router for SpeedQuant API.

This module provides API endpoints for running and managing backtests.
"""

from fastapi import APIRouter, Depends, HTTPException, BackgroundTasks, Query, Path
from typing import Dict, List, Any, Optional
from datetime import datetime, timedelta
import uuid

from ..models import BacktestConfigModel, BacktestResultModel


router = APIRouter()


@router.post("/run")
async def run_backtest(
    config: BacktestConfigModel,
    background_tasks: BackgroundTasks
) -> Dict[str, Any]:
    """
    Run a backtest.
    
    Args:
        config: Backtest configuration
        background_tasks: Background tasks
        
    Returns:
        Backtest job information
    """
    # This is a placeholder. In a real system, you would run the backtest in the background.
    backtest_id = f"backtest-{uuid.uuid4()}"
    
    # In a real implementation, you would add a background task to run the backtest
    # background_tasks.add_task(run_backtest_task, backtest_id, config)
    
    return {
        "backtest_id": backtest_id,
        "status": "submitted",
        "message": "Backtest job submitted successfully",
        "timestamp": datetime.now().isoformat(),
        "config": config.dict()
    }


@router.get("/")
async def list_backtests(
    strategy_id: Optional[str] = None,
    status: Optional[str] = None,
    limit: int = Query(10, ge=1, le=100),
    offset: int = Query(0, ge=0)
) -> List[Dict[str, Any]]:
    """
    List all backtests with optional filtering.
    
    Args:
        strategy_id: Filter by strategy ID
        status: Filter by status
        limit: Maximum number of backtests to return
        offset: Number of backtests to skip
        
    Returns:
        List of backtests
    """
    # This is a placeholder. In a real system, you would query a database.
    backtests = [
        {
            "id": "backtest-1",
            "strategy_id": "strategy-1",
            "start_time": (datetime.now() - timedelta(days=1)).isoformat(),
            "end_time": datetime.now().isoformat(),
            "status": "completed",
            "config": {
                "start_date": (datetime.now() - timedelta(days=365)).isoformat(),
                "end_date": datetime.now().isoformat(),
                "initial_capital": 100000.0,
                "symbols": ["AAPL", "MSFT"]
            },
            "summary": {
                "total_return": 15.5,
                "sharpe_ratio": 1.2,
                "max_drawdown": 5.3
            }
        }
    ]
    
    # Apply filters
    if strategy_id:
        backtests = [b for b in backtests if b["strategy_id"] == strategy_id]
    if status:
        backtests = [b for b in backtests if b["status"] == status]
        
    # Apply pagination
    return backtests[offset:offset+limit]


@router.get("/{backtest_id}")
async def get_backtest(backtest_id: str = Path(..., title="Backtest ID")) -> BacktestResultModel:
    """
    Get a backtest by ID.
    
    Args:
        backtest_id: Backtest ID
        
    Returns:
        Backtest result
    """
    # This is a placeholder. In a real system, you would query a database.
    if backtest_id != "backtest-1":
        raise HTTPException(status_code=404, detail="Backtest not found")
        
    # Create a sample backtest result
    start_date = datetime.now() - timedelta(days=365)
    end_date = datetime.now()
    
    config = BacktestConfigModel(
        strategy_id="strategy-1",
        start_date=start_date,
        end_date=end_date,
        initial_capital=100000.0,
        commission=0.001,
        slippage=0.0,
        data_source="yahoo",
        symbols=["AAPL", "MSFT"],
        parameters={
            "short_window": 20,
            "long_window": 50
        }
    )
    
    # Generate sample equity curve
    dates = [start_date + timedelta(days=i) for i in range((end_date - start_date).days + 1)]
    equity_curve = []
    
    # Simple equity curve with some randomness
    capital = 100000.0
    for date in dates:
        # Random daily return between -1% and 2%
        daily_return = (2 * (date.toordinal() % 7) / 7 - 0.5) * 0.01
        capital *= (1 + daily_return)
        
        equity_curve.append({
            "date": date.isoformat(),
            "equity": round(capital, 2),
            "return": round(daily_return * 100, 2),
            "drawdown": round(max(0, (100000.0 - capital) / 100000.0 * 100), 2)
        })
    
    # Generate sample trades
    trades = []
    for i in range(20):
        trade_date = start_date + timedelta(days=i * 18)
        exit_date = trade_date + timedelta(days=10)
        
        if exit_date > end_date:
            break
            
        symbol = "AAPL" if i % 2 == 0 else "MSFT"
        side = "BUY" if i % 3 != 0 else "SELL"
        entry_price = 150.0 + (i * 2)
        exit_price = entry_price * (1.05 if side == "BUY" else 0.95)
        quantity = 10.0
        pnl = (exit_price - entry_price) * quantity if side == "BUY" else (entry_price - exit_price) * quantity
        
        trades.append({
            "id": f"trade-{i+1}",
            "symbol": symbol,
            "side": side,
            "entry_date": trade_date.isoformat(),
            "exit_date": exit_date.isoformat(),
            "entry_price": round(entry_price, 2),
            "exit_price": round(exit_price, 2),
            "quantity": quantity,
            "pnl": round(pnl, 2),
            "pnl_pct": round(pnl / (entry_price * quantity) * 100, 2)
        })
    
    return BacktestResultModel(
        id=backtest_id,
        strategy_id="strategy-1",
        config=config,
        start_time=start_date,
        end_time=end_date,
        status="completed",
        metrics={
            "total_return": 15.5,
            "total_return_pct": 15.5,
            "annualized_return": 12.3,
            "sharpe_ratio": 1.2,
            "sortino_ratio": 1.8,
            "max_drawdown": 5.3,
            "max_drawdown_pct": 5.3,
            "win_rate": 65.0,
            "profit_factor": 2.1,
            "avg_trade": 0.5
        },
        equity_curve=equity_curve,
        trades=trades
    )


@router.delete("/{backtest_id}", status_code=204)
async def delete_backtest(backtest_id: str = Path(..., title="Backtest ID")):
    """
    Delete a backtest.
    
    Args:
        backtest_id: Backtest ID
    """
    # This is a placeholder. In a real system, you would delete from a database.
    if backtest_id != "backtest-1":
        raise HTTPException(status_code=404, detail="Backtest not found")
        
    # No content returned for successful deletion


@router.get("/{backtest_id}/equity-curve")
async def get_backtest_equity_curve(backtest_id: str = Path(..., title="Backtest ID")) -> Dict[str, Any]:
    """
    Get the equity curve for a backtest.
    
    Args:
        backtest_id: Backtest ID
        
    Returns:
        Equity curve data
    """
    # This is a placeholder. In a real system, you would query a database.
    if backtest_id != "backtest-1":
        raise HTTPException(status_code=404, detail="Backtest not found")
        
    # Generate sample equity curve
    start_date = datetime.now() - timedelta(days=365)
    end_date = datetime.now()
    dates = [start_date + timedelta(days=i) for i in range((end_date - start_date).days + 1)]
    equity_curve = []
    
    # Simple equity curve with some randomness
    capital = 100000.0
    for date in dates:
        # Random daily return between -1% and 2%
        daily_return = (2 * (date.toordinal() % 7) / 7 - 0.5) * 0.01
        capital *= (1 + daily_return)
        
        equity_curve.append({
            "date": date.isoformat(),
            "equity": round(capital, 2),
            "return": round(daily_return * 100, 2),
            "drawdown": round(max(0, (100000.0 - capital) / 100000.0 * 100), 2)
        })
    
    return {
        "backtest_id": backtest_id,
        "strategy_id": "strategy-1",
        "start_date": start_date.isoformat(),
        "end_date": end_date.isoformat(),
        "initial_capital": 100000.0,
        "final_capital": round(capital, 2),
        "equity_curve": equity_curve
    }


@router.get("/{backtest_id}/trades")
async def get_backtest_trades(
    backtest_id: str = Path(..., title="Backtest ID"),
    limit: int = Query(100, ge=1, le=1000),
    offset: int = Query(0, ge=0)
) -> Dict[str, Any]:
    """
    Get the trades for a backtest.
    
    Args:
        backtest_id: Backtest ID
        limit: Maximum number of trades to return
        offset: Number of trades to skip
        
    Returns:
        Trade data
    """
    # This is a placeholder. In a real system, you would query a database.
    if backtest_id != "backtest-1":
        raise HTTPException(status_code=404, detail="Backtest not found")
        
    # Generate sample trades
    start_date = datetime.now() - timedelta(days=365)
    trades = []
    
    for i in range(50):
        trade_date = start_date + timedelta(days=i * 7)
        exit_date = trade_date + timedelta(days=3)
        
        symbol = "AAPL" if i % 2 == 0 else "MSFT"
        side = "BUY" if i % 3 != 0 else "SELL"
        entry_price = 150.0 + (i * 2)
        exit_price = entry_price * (1.05 if side == "BUY" else 0.95)
        quantity = 10.0
        pnl = (exit_price - entry_price) * quantity if side == "BUY" else (entry_price - exit_price) * quantity
        
        trades.append({
            "id": f"trade-{i+1}",
            "symbol": symbol,
            "side": side,
            "entry_date": trade_date.isoformat(),
            "exit_date": exit_date.isoformat(),
            "entry_price": round(entry_price, 2),
            "exit_price": round(exit_price, 2),
            "quantity": quantity,
            "pnl": round(pnl, 2),
            "pnl_pct": round(pnl / (entry_price * quantity) * 100, 2)
        })
    
    # Apply pagination
    paginated_trades = trades[offset:offset+limit]
    
    return {
        "backtest_id": backtest_id,
        "strategy_id": "strategy-1",
        "total_trades": len(trades),
        "offset": offset,
        "limit": limit,
        "trades": paginated_trades
    }
