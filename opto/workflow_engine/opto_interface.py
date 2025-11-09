#!/usr/bin/env python3
"""
OPTO CLI Interface Module

Provides Python wrapper around OPTO command-line interface for:
- Submitting OPTO optimization jobs
- Monitoring job execution via log files
- Parsing results from OPTO output

This module preserves existing OPTO functionality while enabling
automated workflow orchestration.
"""

import os
import subprocess
import time
import re
from pathlib import Path
from typing import Dict, List, Optional, Union
from dataclasses import dataclass
from datetime import datetime
import json


@dataclass
class OptoJobConfig:
    """Configuration for an OPTO optimization job."""

    job_type: str  # e.g., "nutation", "cp_optimization", "2d_grid"
    nucleus: str   # e.g., "1H", "13C", "15N"

    # Parameter ranges
    param1_name: str
    param1_min: float
    param1_max: float
    param1_steps: int

    param2_name: Optional[str] = None
    param2_min: Optional[float] = None
    param2_max: Optional[float] = None
    param2_steps: Optional[int] = None

    # Acquisition parameters
    spinning_rate: float = 20000.0  # Hz
    temperature: float = 298.0       # K
    contact_time: Optional[float] = None  # ms (for CP)
    ramp_type: Optional[str] = None  # "up", "down", "from", "to"

    # Experiment details
    experiment_name: str = "opto_cal"
    num_scans: int = 8
    relaxation_delay: float = 2.0  # s

    # OPTO-specific
    scoring_method: str = "max"  # or "symmetry_aware"
    safety_margin: float = 0.9   # fraction of max safe power

    def to_dict(self) -> Dict:
        """Convert to dictionary for JSON serialization."""
        return {
            "job_type": self.job_type,
            "nucleus": self.nucleus,
            "param1": {
                "name": self.param1_name,
                "min": self.param1_min,
                "max": self.param1_max,
                "steps": self.param1_steps
            },
            "param2": {
                "name": self.param2_name,
                "min": self.param2_min,
                "max": self.param2_max,
                "steps": self.param2_steps
            } if self.param2_name else None,
            "acquisition": {
                "spinning_rate": self.spinning_rate,
                "temperature": self.temperature,
                "contact_time": self.contact_time,
                "ramp_type": self.ramp_type,
                "num_scans": self.num_scans,
                "relaxation_delay": self.relaxation_delay
            },
            "experiment_name": self.experiment_name,
            "scoring_method": self.scoring_method,
            "safety_margin": self.safety_margin
        }


@dataclass
class OptoJobResult:
    """Results from an OPTO optimization job."""

    job_id: str
    job_type: str
    status: str  # "running", "completed", "failed"

    # Optimized parameters
    optimized_params: Dict[str, float]

    # Performance metrics
    score: float
    efficiency: Optional[float] = None  # CP efficiency percentage

    # Diagnostics
    convergence_iterations: int = 0
    warnings: List[str] = None

    # Metadata
    timestamp: datetime = None
    log_file_path: str = ""

    def __post_init__(self):
        if self.warnings is None:
            self.warnings = []
        if self.timestamp is None:
            self.timestamp = datetime.now()

    def to_dict(self) -> Dict:
        """Convert to dictionary for JSON serialization."""
        return {
            "job_id": self.job_id,
            "job_type": self.job_type,
            "status": self.status,
            "optimized_params": self.optimized_params,
            "score": self.score,
            "efficiency": self.efficiency,
            "convergence_iterations": self.convergence_iterations,
            "warnings": self.warnings,
            "timestamp": self.timestamp.isoformat(),
            "log_file_path": self.log_file_path
        }


class OptoInterface:
    """
    Interface to OPTO CLI for submitting jobs and monitoring results.

    This class wraps the OPTO command-line interface, allowing Python
    code to orchestrate optimization workflows while preserving all
    existing OPTO functionality.
    """

    def __init__(self,
                 opto_cli_path: str = "opto",
                 experiment_dir: str = "~/vnmrsys/exp1",
                 log_dir: str = "./opto_logs",
                 polling_interval: float = 2.0):
        """
        Initialize OPTO interface.

        Parameters
        ----------
        opto_cli_path : str
            Path to OPTO CLI executable
        experiment_dir : str
            Base directory for NMR experiments
        log_dir : str
            Directory where OPTO logs are written
        polling_interval : float
            Seconds between log file checks
        """
        self.opto_cli_path = opto_cli_path
        self.experiment_dir = Path(experiment_dir).expanduser()
        self.log_dir = Path(log_dir).expanduser()
        self.polling_interval = polling_interval

        # Create log directory if it doesn't exist
        self.log_dir.mkdir(parents=True, exist_ok=True)

        # Active jobs tracking
        self.active_jobs: Dict[str, subprocess.Popen] = {}

        # Import log parser (will be created next)
        from .log_parser import OptoLogParser
        self.log_parser = OptoLogParser()

    def submit_job(self, config: OptoJobConfig,
                   wait_for_completion: bool = False,
                   timeout: Optional[float] = None) -> OptoJobResult:
        """
        Submit an OPTO optimization job.

        Parameters
        ----------
        config : OptoJobConfig
            Job configuration
        wait_for_completion : bool
            If True, block until job completes
        timeout : float, optional
            Maximum time to wait (seconds)

        Returns
        -------
        OptoJobResult
            Job results (may be incomplete if not waiting)
        """
        # Generate unique job ID
        job_id = self._generate_job_id(config)

        # Write job configuration to file
        job_file = self.log_dir / f"{job_id}_config.json"
        with open(job_file, 'w') as f:
            json.dump(config.to_dict(), f, indent=2)

        # Build OPTO CLI command
        cmd = self._build_opto_command(config, job_id)

        # Submit job
        print(f"Submitting OPTO job: {job_id}")
        print(f"Command: {' '.join(cmd)}")

        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        # Track active job
        self.active_jobs[job_id] = process

        if wait_for_completion:
            return self._wait_for_job(job_id, timeout)
        else:
            # Return preliminary result
            return OptoJobResult(
                job_id=job_id,
                job_type=config.job_type,
                status="running",
                optimized_params={},
                score=0.0,
                log_file_path=str(self.log_dir / f"{job_id}.log")
            )

    def check_job_status(self, job_id: str) -> OptoJobResult:
        """
        Check status of a submitted job.

        Parameters
        ----------
        job_id : str
            Job identifier

        Returns
        -------
        OptoJobResult
            Current job status and any available results
        """
        # Check if job is still running
        if job_id in self.active_jobs:
            process = self.active_jobs[job_id]
            if process.poll() is None:
                # Still running, try to parse partial results
                return self._parse_partial_results(job_id)
            else:
                # Job finished, parse final results
                del self.active_jobs[job_id]
                return self._parse_final_results(job_id)
        else:
            # Job not active, parse results from log
            return self._parse_final_results(job_id)

    def _wait_for_job(self, job_id: str, timeout: Optional[float] = None) -> OptoJobResult:
        """
        Wait for job to complete and return results.

        Parameters
        ----------
        job_id : str
            Job identifier
        timeout : float, optional
            Maximum time to wait (seconds)

        Returns
        -------
        OptoJobResult
            Final job results
        """
        start_time = time.time()

        while True:
            result = self.check_job_status(job_id)

            if result.status in ["completed", "failed"]:
                return result

            # Check timeout
            if timeout and (time.time() - start_time) > timeout:
                result.status = "timeout"
                result.warnings.append(f"Job timed out after {timeout}s")
                return result

            # Wait before next check
            time.sleep(self.polling_interval)

    def _generate_job_id(self, config: OptoJobConfig) -> str:
        """Generate unique job ID from config and timestamp."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        return f"OPTO-{config.job_type}-{config.nucleus}-{timestamp}"

    def _build_opto_command(self, config: OptoJobConfig, job_id: str) -> List[str]:
        """
        Build OPTO CLI command from configuration.

        This is a placeholder - actual command structure depends on
        OPTO CLI interface. Modify based on actual CLI syntax.
        """
        cmd = [self.opto_cli_path]

        # Job type
        cmd.extend(["--job-type", config.job_type])

        # Parameters to optimize
        cmd.extend([
            "--param1", config.param1_name,
            "--param1-range", f"{config.param1_min},{config.param1_max}",
            "--param1-steps", str(config.param1_steps)
        ])

        if config.param2_name:
            cmd.extend([
                "--param2", config.param2_name,
                "--param2-range", f"{config.param2_min},{config.param2_max}",
                "--param2-steps", str(config.param2_steps)
            ])

        # Acquisition parameters
        cmd.extend([
            "--spinning-rate", str(config.spinning_rate),
            "--temperature", str(config.temperature),
            "--num-scans", str(config.num_scans),
            "--relaxation-delay", str(config.relaxation_delay)
        ])

        # CP-specific
        if config.contact_time:
            cmd.extend(["--contact-time", str(config.contact_time)])
        if config.ramp_type:
            cmd.extend(["--ramp-type", config.ramp_type])

        # Output
        cmd.extend([
            "--output-dir", str(self.log_dir),
            "--job-id", job_id
        ])

        return cmd

    def _parse_partial_results(self, job_id: str) -> OptoJobResult:
        """Parse partial results from running job."""
        # Try to find and parse log file
        log_file = self._find_log_file(job_id)

        if log_file and log_file.exists():
            try:
                partial_data = self.log_parser.parse_log(str(log_file), partial=True)
                return OptoJobResult(
                    job_id=job_id,
                    job_type=partial_data.get("job_type", "unknown"),
                    status="running",
                    optimized_params=partial_data.get("best_params", {}),
                    score=partial_data.get("best_score", 0.0),
                    efficiency=partial_data.get("efficiency"),
                    convergence_iterations=partial_data.get("iterations", 0),
                    log_file_path=str(log_file)
                )
            except Exception as e:
                print(f"Warning: Failed to parse partial results: {e}")

        # Return running status with no data
        return OptoJobResult(
            job_id=job_id,
            job_type="unknown",
            status="running",
            optimized_params={},
            score=0.0
        )

    def _parse_final_results(self, job_id: str) -> OptoJobResult:
        """Parse final results from completed job."""
        log_file = self._find_log_file(job_id)

        if not log_file or not log_file.exists():
            return OptoJobResult(
                job_id=job_id,
                job_type="unknown",
                status="failed",
                optimized_params={},
                score=0.0,
                warnings=["Log file not found"]
            )

        try:
            final_data = self.log_parser.parse_log(str(log_file), partial=False)

            return OptoJobResult(
                job_id=job_id,
                job_type=final_data.get("job_type", "unknown"),
                status="completed",
                optimized_params=final_data.get("optimized_params", {}),
                score=final_data.get("final_score", 0.0),
                efficiency=final_data.get("efficiency"),
                convergence_iterations=final_data.get("total_iterations", 0),
                warnings=final_data.get("warnings", []),
                log_file_path=str(log_file)
            )
        except Exception as e:
            return OptoJobResult(
                job_id=job_id,
                job_type="unknown",
                status="failed",
                optimized_params={},
                score=0.0,
                warnings=[f"Failed to parse results: {str(e)}"]
            )

    def _find_log_file(self, job_id: str) -> Optional[Path]:
        """Find OPTO log file for given job ID."""
        # Try various possible log file patterns
        patterns = [
            f"{job_id}.log",
            f"{job_id}_*.log",
            f"OPTO-{job_id}.log"
        ]

        for pattern in patterns:
            matches = list(self.log_dir.glob(pattern))
            if matches:
                return matches[0]

        return None

    def cancel_job(self, job_id: str):
        """Cancel a running job."""
        if job_id in self.active_jobs:
            process = self.active_jobs[job_id]
            process.terminate()
            try:
                process.wait(timeout=5.0)
            except subprocess.TimeoutExpired:
                process.kill()
            del self.active_jobs[job_id]
            print(f"Job {job_id} cancelled")

    def list_active_jobs(self) -> List[str]:
        """List all currently active jobs."""
        # Clean up finished jobs
        finished = []
        for job_id, process in self.active_jobs.items():
            if process.poll() is not None:
                finished.append(job_id)

        for job_id in finished:
            del self.active_jobs[job_id]

        return list(self.active_jobs.keys())


# Convenience class alias
OptoJob = OptoJobConfig
