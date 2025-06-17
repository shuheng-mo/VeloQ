"""
Portfolio Optimizer Agent for SpeedQuant.

This module provides an AI-powered agent for portfolio optimization
using modern portfolio theory, risk parity, and machine learning approaches.
"""

import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Tuple
import logging
from scipy.optimize import minimize
from dataclasses import dataclass

from .base_agent import BaseAgent

logger = logging.getLogger(__name__)


@dataclass
class OptimizationResult:
    """Data class to store optimization results."""
    weights: Dict[str, float]
    expected_return: float
    expected_volatility: float
    sharpe_ratio: float
    efficient_frontier: Optional[List[Dict[str, float]]] = None


class PortfolioOptimizer(BaseAgent):
    """
    AI agent for portfolio optimization.
    
    This agent uses various optimization techniques to allocate assets
    in a portfolio based on risk-return objectives.
    """
    
    def __init__(self, agent_id: Optional[str] = None, config: Optional[Dict[str, Any]] = None):
        """
        Initialize the portfolio optimizer agent.
        
        Args:
            agent_id: Unique identifier for the agent.
            config: Configuration parameters for the agent.
                - risk_free_rate: Risk-free rate for Sharpe ratio calculation (default: 0.02)
                - optimization_method: Method to use for optimization (default: 'mean_variance')
                    Options: 'mean_variance', 'risk_parity', 'min_volatility', 'max_sharpe'
                - constraints: Dictionary of constraints for the optimization
                    - max_weight: Maximum weight for any asset (default: 1.0)
                    - min_weight: Minimum weight for any asset (default: 0.0)
        """
        super().__init__(agent_id, config)
        
        # Set default configuration values
        self.config.setdefault('risk_free_rate', 0.02)
        self.config.setdefault('optimization_method', 'mean_variance')
        
        if 'constraints' not in self.config:
            self.config['constraints'] = {
                'max_weight': 1.0,
                'min_weight': 0.0
            }
    
    def execute(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """
        Execute portfolio optimization.
        
        Args:
            context: Input data and parameters.
                Required keys:
                - returns: DataFrame of historical returns for each asset
                - expected_returns: Dictionary of expected returns for each asset (optional)
                - risk_target: Target risk level (optional)
                - return_target: Target return level (optional)
                
        Returns:
            Dictionary containing the optimized portfolio weights and metrics.
        """
        # Validate required inputs
        if not self.validate_context(context, ['returns']):
            return {
                "error": "Missing required context: 'returns'",
                "success": False
            }
        
        # Extract inputs
        returns_df = context['returns']
        expected_returns = context.get('expected_returns')
        risk_target = context.get('risk_target')
        return_target = context.get('return_target')
        
        # Calculate expected returns if not provided
        if expected_returns is None:
            expected_returns = returns_df.mean().to_dict()
        
        # Calculate covariance matrix
        cov_matrix = returns_df.cov()
        
        # Run the optimization based on the specified method
        method = self.config['optimization_method']
        
        try:
            if method == 'mean_variance':
                result = self._mean_variance_optimization(
                    returns_df, expected_returns, cov_matrix, 
                    risk_target, return_target
                )
            elif method == 'risk_parity':
                result = self._risk_parity_optimization(
                    returns_df, cov_matrix
                )
            elif method == 'min_volatility':
                result = self._min_volatility_optimization(
                    returns_df, cov_matrix
                )
            elif method == 'max_sharpe':
                result = self._max_sharpe_optimization(
                    returns_df, expected_returns, cov_matrix
                )
            else:
                return {
                    "error": f"Unknown optimization method: {method}",
                    "success": False
                }
            
            # Format the result
            return {
                "optimal_weights": result.weights,
                "expected_return": result.expected_return,
                "expected_volatility": result.expected_volatility,
                "sharpe_ratio": result.sharpe_ratio,
                "efficient_frontier": result.efficient_frontier,
                "optimization_method": method,
                "constraints": self.config['constraints'],
                "success": True
            }
        
        except Exception as e:
            logger.exception(f"Portfolio optimization failed: {str(e)}")
            return {
                "error": f"Optimization failed: {str(e)}",
                "success": False
            }
    
    def _mean_variance_optimization(
        self, 
        returns_df: pd.DataFrame,
        expected_returns: Dict[str, float],
        cov_matrix: pd.DataFrame,
        risk_target: Optional[float] = None,
        return_target: Optional[float] = None
    ) -> OptimizationResult:
        """
        Perform mean-variance optimization.
        
        Args:
            returns_df: DataFrame of historical returns
            expected_returns: Dictionary of expected returns
            cov_matrix: Covariance matrix
            risk_target: Target risk level (optional)
            return_target: Target return level (optional)
            
        Returns:
            OptimizationResult with optimal weights and metrics
        """
        n_assets = len(returns_df.columns)
        assets = list(returns_df.columns)
        
        # Convert expected returns to array
        mu = np.array([expected_returns[asset] for asset in assets])
        
        # Define optimization constraints
        constraints = [
            {'type': 'eq', 'fun': lambda x: np.sum(x) - 1}  # weights sum to 1
        ]
        
        if return_target is not None:
            constraints.append(
                {'type': 'eq', 'fun': lambda x: np.sum(x * mu) - return_target}
            )
        
        # Define bounds for weights
        min_weight = self.config['constraints'].get('min_weight', 0.0)
        max_weight = self.config['constraints'].get('max_weight', 1.0)
        bounds = tuple((min_weight, max_weight) for _ in range(n_assets))
        
        # Initial guess: equal weights
        initial_weights = np.array([1.0 / n_assets] * n_assets)
        
        # Define objective function based on what we're optimizing for
        if risk_target is not None:
            # Minimize return deviation from target
            def objective(weights):
                portfolio_return = np.sum(weights * mu)
                return (portfolio_return - return_target) ** 2
        else:
            # Minimize volatility
            def objective(weights):
                portfolio_volatility = np.sqrt(np.dot(weights.T, np.dot(cov_matrix, weights)))
                return portfolio_volatility
        
        # Run the optimization
        result = minimize(
            objective,
            initial_weights,
            method='SLSQP',
            bounds=bounds,
            constraints=constraints
        )
        
        if not result['success']:
            raise RuntimeError(f"Optimization failed: {result['message']}")
        
        # Calculate portfolio metrics
        optimal_weights = result['x']
        weights_dict = {asset: weight for asset, weight in zip(assets, optimal_weights)}
        
        portfolio_return = np.sum(optimal_weights * mu)
        portfolio_volatility = np.sqrt(np.dot(optimal_weights.T, np.dot(cov_matrix, optimal_weights)))
        sharpe_ratio = (portfolio_return - self.config['risk_free_rate']) / portfolio_volatility
        
        # Generate efficient frontier
        efficient_frontier = self._generate_efficient_frontier(returns_df, expected_returns, cov_matrix)
        
        return OptimizationResult(
            weights=weights_dict,
            expected_return=float(portfolio_return),
            expected_volatility=float(portfolio_volatility),
            sharpe_ratio=float(sharpe_ratio),
            efficient_frontier=efficient_frontier
        )
    
    def _risk_parity_optimization(
        self,
        returns_df: pd.DataFrame,
        cov_matrix: pd.DataFrame
    ) -> OptimizationResult:
        """
        Perform risk parity optimization.
        
        Args:
            returns_df: DataFrame of historical returns
            cov_matrix: Covariance matrix
            
        Returns:
            OptimizationResult with optimal weights and metrics
        """
        n_assets = len(returns_df.columns)
        assets = list(returns_df.columns)
        
        # Calculate expected returns
        mu = returns_df.mean().values
        
        # Define objective function for risk parity
        def risk_contribution_objective(weights):
            weights = np.array(weights)
            portfolio_volatility = np.sqrt(np.dot(weights.T, np.dot(cov_matrix, weights)))
            
            # Calculate risk contribution of each asset
            marginal_risk_contribution = np.dot(cov_matrix, weights)
            risk_contribution = weights * marginal_risk_contribution / portfolio_volatility
            
            # Target: equal risk contribution
            target_risk_contribution = portfolio_volatility / n_assets
            
            # Sum of squared deviations from target
            return np.sum((risk_contribution - target_risk_contribution) ** 2)
        
        # Define constraints
        constraints = [
            {'type': 'eq', 'fun': lambda x: np.sum(x) - 1}  # weights sum to 1
        ]
        
        # Define bounds for weights
        min_weight = self.config['constraints'].get('min_weight', 0.0)
        max_weight = self.config['constraints'].get('max_weight', 1.0)
        bounds = tuple((min_weight, max_weight) for _ in range(n_assets))
        
        # Initial guess: equal weights
        initial_weights = np.array([1.0 / n_assets] * n_assets)
        
        # Run the optimization
        result = minimize(
            risk_contribution_objective,
            initial_weights,
            method='SLSQP',
            bounds=bounds,
            constraints=constraints
        )
        
        if not result['success']:
            raise RuntimeError(f"Risk parity optimization failed: {result['message']}")
        
        # Calculate portfolio metrics
        optimal_weights = result['x']
        weights_dict = {asset: weight for asset, weight in zip(assets, optimal_weights)}
        
        portfolio_return = np.sum(optimal_weights * mu)
        portfolio_volatility = np.sqrt(np.dot(optimal_weights.T, np.dot(cov_matrix, optimal_weights)))
        sharpe_ratio = (portfolio_return - self.config['risk_free_rate']) / portfolio_volatility
        
        return OptimizationResult(
            weights=weights_dict,
            expected_return=float(portfolio_return),
            expected_volatility=float(portfolio_volatility),
            sharpe_ratio=float(sharpe_ratio)
        )
    
    def _min_volatility_optimization(
        self,
        returns_df: pd.DataFrame,
        cov_matrix: pd.DataFrame
    ) -> OptimizationResult:
        """
        Find the minimum volatility portfolio.
        
        Args:
            returns_df: DataFrame of historical returns
            cov_matrix: Covariance matrix
            
        Returns:
            OptimizationResult with optimal weights and metrics
        """
        n_assets = len(returns_df.columns)
        assets = list(returns_df.columns)
        
        # Calculate expected returns
        mu = returns_df.mean().values
        
        # Define objective function: minimize portfolio volatility
        def objective(weights):
            return np.sqrt(np.dot(weights.T, np.dot(cov_matrix, weights)))
        
        # Define constraints
        constraints = [
            {'type': 'eq', 'fun': lambda x: np.sum(x) - 1}  # weights sum to 1
        ]
        
        # Define bounds for weights
        min_weight = self.config['constraints'].get('min_weight', 0.0)
        max_weight = self.config['constraints'].get('max_weight', 1.0)
        bounds = tuple((min_weight, max_weight) for _ in range(n_assets))
        
        # Initial guess: equal weights
        initial_weights = np.array([1.0 / n_assets] * n_assets)
        
        # Run the optimization
        result = minimize(
            objective,
            initial_weights,
            method='SLSQP',
            bounds=bounds,
            constraints=constraints
        )
        
        if not result['success']:
            raise RuntimeError(f"Minimum volatility optimization failed: {result['message']}")
        
        # Calculate portfolio metrics
        optimal_weights = result['x']
        weights_dict = {asset: weight for asset, weight in zip(assets, optimal_weights)}
        
        portfolio_return = np.sum(optimal_weights * mu)
        portfolio_volatility = np.sqrt(np.dot(optimal_weights.T, np.dot(cov_matrix, optimal_weights)))
        sharpe_ratio = (portfolio_return - self.config['risk_free_rate']) / portfolio_volatility
        
        return OptimizationResult(
            weights=weights_dict,
            expected_return=float(portfolio_return),
            expected_volatility=float(portfolio_volatility),
            sharpe_ratio=float(sharpe_ratio)
        )
    
    def _max_sharpe_optimization(
        self,
        returns_df: pd.DataFrame,
        expected_returns: Dict[str, float],
        cov_matrix: pd.DataFrame
    ) -> OptimizationResult:
        """
        Find the portfolio with maximum Sharpe ratio.
        
        Args:
            returns_df: DataFrame of historical returns
            expected_returns: Dictionary of expected returns
            cov_matrix: Covariance matrix
            
        Returns:
            OptimizationResult with optimal weights and metrics
        """
        n_assets = len(returns_df.columns)
        assets = list(returns_df.columns)
        
        # Convert expected returns to array
        mu = np.array([expected_returns[asset] for asset in assets])
        
        # Define objective function: negative Sharpe ratio (to maximize)
        def objective(weights):
            portfolio_return = np.sum(weights * mu)
            portfolio_volatility = np.sqrt(np.dot(weights.T, np.dot(cov_matrix, weights)))
            sharpe_ratio = (portfolio_return - self.config['risk_free_rate']) / portfolio_volatility
            return -sharpe_ratio  # Negative because we're minimizing
        
        # Define constraints
        constraints = [
            {'type': 'eq', 'fun': lambda x: np.sum(x) - 1}  # weights sum to 1
        ]
        
        # Define bounds for weights
        min_weight = self.config['constraints'].get('min_weight', 0.0)
        max_weight = self.config['constraints'].get('max_weight', 1.0)
        bounds = tuple((min_weight, max_weight) for _ in range(n_assets))
        
        # Initial guess: equal weights
        initial_weights = np.array([1.0 / n_assets] * n_assets)
        
        # Run the optimization
        result = minimize(
            objective,
            initial_weights,
            method='SLSQP',
            bounds=bounds,
            constraints=constraints
        )
        
        if not result['success']:
            raise RuntimeError(f"Maximum Sharpe ratio optimization failed: {result['message']}")
        
        # Calculate portfolio metrics
        optimal_weights = result['x']
        weights_dict = {asset: weight for asset, weight in zip(assets, optimal_weights)}
        
        portfolio_return = np.sum(optimal_weights * mu)
        portfolio_volatility = np.sqrt(np.dot(optimal_weights.T, np.dot(cov_matrix, optimal_weights)))
        sharpe_ratio = (portfolio_return - self.config['risk_free_rate']) / portfolio_volatility
        
        # Generate efficient frontier
        efficient_frontier = self._generate_efficient_frontier(returns_df, expected_returns, cov_matrix)
        
        return OptimizationResult(
            weights=weights_dict,
            expected_return=float(portfolio_return),
            expected_volatility=float(portfolio_volatility),
            sharpe_ratio=float(sharpe_ratio),
            efficient_frontier=efficient_frontier
        )
    
    def _generate_efficient_frontier(
        self,
        returns_df: pd.DataFrame,
        expected_returns: Dict[str, float],
        cov_matrix: pd.DataFrame,
        points: int = 20
    ) -> List[Dict[str, float]]:
        """
        Generate points along the efficient frontier.
        
        Args:
            returns_df: DataFrame of historical returns
            expected_returns: Dictionary of expected returns
            cov_matrix: Covariance matrix
            points: Number of points to generate
            
        Returns:
            List of dictionaries with return, volatility, and sharpe ratio
        """
        n_assets = len(returns_df.columns)
        assets = list(returns_df.columns)
        
        # Convert expected returns to array
        mu = np.array([expected_returns[asset] for asset in assets])
        
        # Find minimum and maximum returns
        min_return_weights = np.zeros(n_assets)
        min_return_weights[np.argmin(mu)] = 1.0
        min_return = np.min(mu)
        
        max_return_weights = np.zeros(n_assets)
        max_return_weights[np.argmax(mu)] = 1.0
        max_return = np.max(mu)
        
        # Generate target returns
        target_returns = np.linspace(min_return, max_return, points)
        efficient_frontier = []
        
        for target_return in target_returns:
            try:
                # Define objective function: minimize volatility
                def objective(weights):
                    return np.sqrt(np.dot(weights.T, np.dot(cov_matrix, weights)))
                
                # Define constraints
                constraints = [
                    {'type': 'eq', 'fun': lambda x: np.sum(x) - 1},  # weights sum to 1
                    {'type': 'eq', 'fun': lambda x: np.sum(x * mu) - target_return}  # target return
                ]
                
                # Define bounds for weights
                min_weight = self.config['constraints'].get('min_weight', 0.0)
                max_weight = self.config['constraints'].get('max_weight', 1.0)
                bounds = tuple((min_weight, max_weight) for _ in range(n_assets))
                
                # Initial guess: equal weights
                initial_weights = np.array([1.0 / n_assets] * n_assets)
                
                # Run the optimization
                result = minimize(
                    objective,
                    initial_weights,
                    method='SLSQP',
                    bounds=bounds,
                    constraints=constraints
                )
                
                if result['success']:
                    optimal_weights = result['x']
                    portfolio_volatility = np.sqrt(np.dot(optimal_weights.T, np.dot(cov_matrix, optimal_weights)))
                    sharpe_ratio = (target_return - self.config['risk_free_rate']) / portfolio_volatility
                    
                    efficient_frontier.append({
                        'return': float(target_return),
                        'volatility': float(portfolio_volatility),
                        'sharpe_ratio': float(sharpe_ratio)
                    })
            except:
                # Skip this point if optimization fails
                continue
        
        return efficient_frontier
