"""
API module for SpeedQuant.

This module provides REST API endpoints for interacting with the SpeedQuant system.
"""

from .app import create_app

__all__ = ['create_app']
