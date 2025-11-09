"""
OPTO Workflow Automation Engine

This package provides intelligent workflow orchestration for OPTO-based
NMR calibration and cross-polarization optimization.
"""

from .opto_interface import OptoJob, OptoInterface
from .workflow_orchestrator import WorkflowOrchestrator

__version__ = "0.1.0-alpha"
__all__ = ["OptoJob", "OptoInterface", "WorkflowOrchestrator"]
