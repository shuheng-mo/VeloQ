"""
Base Agent class for SpeedQuant.

This module defines the BaseAgent abstract class that all agents must inherit from.
"""

from abc import ABC, abstractmethod
from typing import Dict, Any, Optional, List
import time
import uuid
import logging
from datetime import datetime

logger = logging.getLogger(__name__)


class BaseAgent(ABC):
    """
    Abstract base class for all SpeedQuant AI agents.
    
    All agents in the system must inherit from this class and implement
    the required methods.
    """
    
    def __init__(self, agent_id: Optional[str] = None, config: Optional[Dict[str, Any]] = None):
        """
        Initialize the agent.
        
        Args:
            agent_id: Unique identifier for the agent. If not provided, a UUID will be generated.
            config: Configuration parameters for the agent.
        """
        self.agent_id = agent_id or f"{self.__class__.__name__}-{uuid.uuid4()}"
        self.config = config or {}
        self.created_at = datetime.now()
        self.last_execution = None
        self.execution_count = 0
        self.execution_times = []
        
        logger.info(f"Agent {self.agent_id} initialized")
    
    @abstractmethod
    def execute(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """
        Execute the agent's logic.
        
        This is the main method that must be implemented by all agent subclasses.
        It takes a context dictionary as input and returns a result dictionary.
        
        Args:
            context: Input data and parameters for the agent.
            
        Returns:
            Dictionary containing the agent's output.
        """
        pass
    
    def run(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """
        Run the agent with timing and logging.
        
        This method wraps the execute method with timing, logging, and error handling.
        
        Args:
            context: Input data and parameters for the agent.
            
        Returns:
            Dictionary containing the agent's output.
        """
        start_time = time.time()
        self.last_execution = datetime.now()
        self.execution_count += 1
        
        try:
            logger.info(f"Running agent {self.agent_id} (execution #{self.execution_count})")
            result = self.execute(context)
            
            execution_time = time.time() - start_time
            self.execution_times.append(execution_time)
            
            # Add metadata to result
            result.update({
                "agent_id": self.agent_id,
                "execution_time": execution_time,
                "timestamp": self.last_execution.isoformat(),
                "execution_count": self.execution_count
            })
            
            logger.info(f"Agent {self.agent_id} completed in {execution_time:.2f}s")
            return result
        except Exception as e:
            logger.exception(f"Error running agent {self.agent_id}: {str(e)}")
            
            # Return error information
            return {
                "agent_id": self.agent_id,
                "error": str(e),
                "execution_time": time.time() - start_time,
                "timestamp": self.last_execution.isoformat(),
                "execution_count": self.execution_count,
                "success": False
            }
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get statistics about the agent's execution history.
        
        Returns:
            Dictionary containing execution statistics.
        """
        avg_time = sum(self.execution_times) / len(self.execution_times) if self.execution_times else 0
        
        return {
            "agent_id": self.agent_id,
            "agent_type": self.__class__.__name__,
            "execution_count": self.execution_count,
            "average_execution_time": avg_time,
            "last_execution": self.last_execution.isoformat() if self.last_execution else None,
            "created_at": self.created_at.isoformat()
        }
    
    def validate_context(self, context: Dict[str, Any], required_keys: List[str]) -> bool:
        """
        Validate that the context contains all required keys.
        
        Args:
            context: Context dictionary to validate.
            required_keys: List of required keys.
            
        Returns:
            True if all required keys are present, False otherwise.
        """
        missing_keys = [key for key in required_keys if key not in context]
        
        if missing_keys:
            logger.error(f"Missing required keys in context: {', '.join(missing_keys)}")
            return False
        
        return True
