#!/usr/bin/env python3
"""
OPTO Log Parser Module

Parses OPTO optimization log files to extract:
- Optimized parameters
- Performance scores
- CP efficiency measurements
- Convergence information
- Warnings and diagnostics

Handles both completed and in-progress (partial) log files.
"""

import re
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
import numpy as np


class OptoLogParser:
    """
    Parser for OPTO log files.

    Extracts optimization results, parameters, and diagnostics from
    OPTO output files. Supports parsing of both complete and partial
    (still running) log files.
    """

    def __init__(self):
        """Initialize log parser with regex patterns."""

        # Common patterns for parsing OPTO logs
        self.patterns = {
            # Header information
            "job_type": re.compile(r"Job\s+Type:\s+(\w+)", re.IGNORECASE),
            "timestamp": re.compile(r"Started:\s+(.+)$", re.MULTILINE),
            "nucleus": re.compile(r"Nucleus:\s+(\w+)"),

            # Parameter information
            "param_names": re.compile(r"Optimizing:\s+(.+)$", re.MULTILINE),
            "param_range": re.compile(r"(\w+)\s+range:\s+\[([0-9.]+),\s*([0-9.]+)\]"),

            # Results
            "iteration": re.compile(r"Iteration\s+(\d+)"),
            "score": re.compile(r"Score:\s+([0-9.eE+-]+)"),
            "efficiency": re.compile(r"CP\s+Efficiency:\s+([0-9.]+)%"),
            "best_params": re.compile(r"Best\s+params?:\s+(.+)$", re.MULTILINE),

            # Convergence
            "converged": re.compile(r"Converged|Optimization\s+complete", re.IGNORECASE),
            "max_iterations": re.compile(r"Maximum\s+iterations\s+reached"),

            # Warnings
            "warning": re.compile(r"WARNING:\s+(.+)$", re.MULTILINE),
            "error": re.compile(r"ERROR:\s+(.+)$", re.MULTILINE),
        }

    def parse_log(self, log_file_path: str, partial: bool = False) -> Dict:
        """
        Parse an OPTO log file.

        Parameters
        ----------
        log_file_path : str
            Path to log file
        partial : bool
            If True, parse as partial/incomplete log (job still running)

        Returns
        -------
        dict
            Parsed log data with keys:
            - job_type: str
            - timestamp: str
            - nucleus: str
            - param_names: list
            - optimized_params: dict
            - final_score: float
            - efficiency: float (CP efficiency %)
            - total_iterations: int
            - converged: bool
            - warnings: list
            - errors: list
        """
        log_path = Path(log_file_path)
        if not log_path.exists():
            raise FileNotFoundError(f"Log file not found: {log_file_path}")

        with open(log_path, 'r') as f:
            content = f.read()

        # Extract basic information
        result = {
            "job_type": self._extract_single(self.patterns["job_type"], content),
            "timestamp": self._extract_single(self.patterns["timestamp"], content),
            "nucleus": self._extract_single(self.patterns["nucleus"], content),
            "param_names": [],
            "optimized_params": {},
            "final_score": 0.0,
            "efficiency": None,
            "total_iterations": 0,
            "converged": False,
            "warnings": [],
            "errors": [],
        }

        # Extract parameter names
        param_names_str = self._extract_single(self.patterns["param_names"], content)
        if param_names_str:
            result["param_names"] = [p.strip() for p in param_names_str.split(',')]

        # Extract iteration data
        iterations = self._extract_iterations(content)
        result["total_iterations"] = len(iterations)

        if iterations:
            # Best iteration (highest score)
            best_iter = max(iterations, key=lambda x: x["score"])
            result["final_score"] = best_iter["score"]
            result["optimized_params"] = best_iter.get("params", {})

            # Check for CP efficiency
            if "efficiency" in best_iter:
                result["efficiency"] = best_iter["efficiency"]

        # Check convergence status
        result["converged"] = bool(self.patterns["converged"].search(content))

        # Extract warnings and errors
        result["warnings"] = self._extract_all(self.patterns["warning"], content)
        result["errors"] = self._extract_all(self.patterns["error"], content)

        # If partial parsing, also extract current best parameters
        if partial:
            result["best_params"] = result["optimized_params"]
            result["best_score"] = result["final_score"]
            result["iterations"] = result["total_iterations"]

        return result

    def parse_stripchart_format(self, log_file_path: str) -> Tuple[np.ndarray, List[str]]:
        """
        Parse OPTO log in stripchart format (for opto_stripchart4.py).

        Returns data array and parameter names for real-time plotting.

        Parameters
        ----------
        log_file_path : str
            Path to log file

        Returns
        -------
        data : np.ndarray
            Array of shape (n_points, n_params+1) with parameters and score
        header : list
            Parameter names (last column is score)
        """
        log_path = Path(log_file_path)
        if not log_path.exists():
            raise FileNotFoundError(f"Log file not found: {log_file_path}")

        # Try to parse as tab/space-delimited data
        try:
            # Read header (first line)
            with open(log_path, 'r') as f:
                header_line = f.readline().strip()
                # Skip comment character if present
                if header_line.startswith('#'):
                    header_line = header_line[1:].strip()

                header = header_line.split()

            # Read data (skip header)
            data = np.loadtxt(log_path, skiprows=1)

            return data, header

        except Exception as e:
            # Fallback: parse as structured log and convert to array
            parsed = self.parse_log(str(log_path))

            # Extract iteration data
            iterations = self._extract_iterations(Path(log_path).read_text())

            if not iterations:
                return np.array([]), []

            # Build array
            param_names = parsed["param_names"] + ["score"]
            n_params = len(param_names)
            n_iters = len(iterations)

            data = np.zeros((n_iters, n_params))

            for i, iter_data in enumerate(iterations):
                # Extract parameter values
                for j, param_name in enumerate(parsed["param_names"]):
                    data[i, j] = iter_data.get("params", {}).get(param_name, 0.0)
                # Score in last column
                data[i, -1] = iter_data["score"]

            return data, param_names

    def _extract_single(self, pattern: re.Pattern, text: str) -> Optional[str]:
        """Extract first match of pattern from text."""
        match = pattern.search(text)
        return match.group(1) if match else None

    def _extract_all(self, pattern: re.Pattern, text: str) -> List[str]:
        """Extract all matches of pattern from text."""
        return [match.group(1) for match in pattern.finditer(text)]

    def _extract_iterations(self, content: str) -> List[Dict]:
        """
        Extract iteration-by-iteration data from log.

        Returns
        -------
        list of dict
            Each dict contains iteration number, score, params, etc.
        """
        iterations = []

        # Split content into iteration blocks
        # This is a simplified parser - actual format depends on OPTO log structure
        lines = content.split('\n')

        current_iter = None
        current_score = 0.0
        current_params = {}
        current_efficiency = None

        for line in lines:
            # Check for iteration marker
            iter_match = self.patterns["iteration"].search(line)
            if iter_match:
                # Save previous iteration
                if current_iter is not None:
                    iterations.append({
                        "iteration": current_iter,
                        "score": current_score,
                        "params": current_params.copy(),
                        "efficiency": current_efficiency
                    })

                # Start new iteration
                current_iter = int(iter_match.group(1))
                current_params = {}
                current_efficiency = None

            # Check for score
            score_match = self.patterns["score"].search(line)
            if score_match:
                current_score = float(score_match.group(1))

            # Check for CP efficiency
            eff_match = self.patterns["efficiency"].search(line)
            if eff_match:
                current_efficiency = float(eff_match.group(1))

            # Check for parameter values (simplified - actual format may vary)
            # Example: "H1_field: 50000.0 Hz"
            param_match = re.search(r"(\w+):\s+([0-9.eE+-]+)", line)
            if param_match:
                param_name = param_match.group(1)
                param_value = float(param_match.group(2))
                current_params[param_name] = param_value

        # Save last iteration
        if current_iter is not None:
            iterations.append({
                "iteration": current_iter,
                "score": current_score,
                "params": current_params,
                "efficiency": current_efficiency
            })

        return iterations

    def extract_best_result(self, log_file_path: str) -> Dict:
        """
        Extract only the best optimization result.

        Parameters
        ----------
        log_file_path : str
            Path to log file

        Returns
        -------
        dict
            Best parameters and score
        """
        full_result = self.parse_log(log_file_path, partial=False)

        return {
            "params": full_result["optimized_params"],
            "score": full_result["final_score"],
            "efficiency": full_result["efficiency"],
            "converged": full_result["converged"]
        }

    def monitor_log_realtime(self, log_file_path: str, callback=None):
        """
        Monitor a log file in real-time as it's being written.

        Parameters
        ----------
        log_file_path : str
            Path to log file
        callback : callable, optional
            Function to call with each new iteration's data
        """
        import time

        log_path = Path(log_file_path)
        last_position = 0
        last_iteration = -1

        while True:
            if not log_path.exists():
                time.sleep(1.0)
                continue

            with open(log_path, 'r') as f:
                # Seek to last known position
                f.seek(last_position)

                # Read new content
                new_content = f.read()
                last_position = f.tell()

                if new_content:
                    # Parse new iterations
                    iterations = self._extract_iterations(new_content)

                    for iter_data in iterations:
                        if iter_data["iteration"] > last_iteration:
                            last_iteration = iter_data["iteration"]

                            if callback:
                                callback(iter_data)
                            else:
                                print(f"Iteration {iter_data['iteration']}: "
                                      f"Score = {iter_data['score']:.4f}")

                # Check if optimization completed
                if "Optimization complete" in new_content or "Converged" in new_content:
                    print("Optimization completed")
                    break

            time.sleep(2.0)  # Poll every 2 seconds
