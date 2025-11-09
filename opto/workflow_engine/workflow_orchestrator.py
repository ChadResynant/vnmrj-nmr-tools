#!/usr/bin/env python3
"""
OPTO Workflow Orchestrator

Implements intelligent calibration workflows with:
- Sequential job submission based on results
- Automatic failure recovery
- CP optimization pathology detection
- Hardware diagnostic integration

Embodies expert knowledge about NMR optimization strategies.
"""

import time
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
import numpy as np

from .opto_interface import OptoInterface, OptoJobConfig, OptoJobResult
from .diagnostics import HardwareDiagnostics, CPPathologyDetector


@dataclass
class WorkflowConfig:
    """Configuration for complete calibration workflow."""

    # Target nuclei
    nuclei: List[str]  # e.g., ["1H", "13C", "15N"]

    # Experimental conditions
    spinning_rate: float  # Hz
    temperature: float     # K
    probe_id: str          # e.g., "1.6mm_HXY"

    # Workflow options
    skip_h1_calibration: bool = False  # Use previous H1 cal
    skip_x_calibration: bool = False   # Use previous X cal
    full_cp_exploration: bool = True   # Explore all match conditions

    # Optimization parameters
    h1_pw_tolerance: float = 0.25  # ±25% search range
    cp_efficiency_threshold: float = 0.30  # 30% minimum
    max_cp_iterations: int = 3  # Max match conditions to try

    # Database integration (future)
    use_database_defaults: bool = False
    database_path: Optional[str] = None


class WorkflowOrchestrator:
    """
    Orchestrates complete NMR calibration workflows.

    Implements expert strategies for:
    - Pulse width calibration
    - CP optimization with automatic failure recovery
    - Hardware diagnostics and validation
    """

    # Physical constants
    GYROMAGNETIC_RATIOS = {
        "1H": 42.576,   # MHz/T
        "13C": 10.705,
        "15N": 4.316,
        "31P": 17.235
    }

    def __init__(self,
                 opto_interface: Optional[OptoInterface] = None,
                 diagnostics: Optional[HardwareDiagnostics] = None):
        """
        Initialize workflow orchestrator.

        Parameters
        ----------
        opto_interface : OptoInterface, optional
            Interface to OPTO CLI (created if not provided)
        diagnostics : HardwareDiagnostics, optional
            Diagnostics module (created if not provided)
        """
        self.opto = opto_interface or OptoInterface()
        self.diagnostics = diagnostics or HardwareDiagnostics()
        self.cp_detector = CPPathologyDetector()

        # Workflow state
        self.current_calibrations = {}
        self.workflow_history = []

    def run_calibration(self, config: WorkflowConfig) -> Dict:
        """
        Run complete calibration workflow.

        Parameters
        ----------
        config : WorkflowConfig
            Workflow configuration

        Returns
        -------
        dict
            Complete calibration results with validated parameters
        """
        print(f"\n{'='*60}")
        print(f"Starting calibration workflow for {config.probe_id}")
        print(f"Spinning rate: {config.spinning_rate/1000:.1f} kHz")
        print(f"Temperature: {config.temperature:.1f} K")
        print(f"Nuclei: {', '.join(config.nuclei)}")
        print(f"{'='*60}\n")

        results = {
            "probe_id": config.probe_id,
            "spinning_rate": config.spinning_rate,
            "temperature": config.temperature,
            "calibrations": {},
            "validation": {
                "status": "pending",
                "warnings": [],
                "errors": []
            }
        }

        try:
            # Step 1: H1 pulse width calibration
            if not config.skip_h1_calibration:
                h1_result = self._calibrate_h1_pulse_width(config)
                results["calibrations"]["H1_pw90"] = h1_result
                self.current_calibrations["H1_pw90"] = h1_result["value"]

                # Validate H1 calibration
                h1_validation = self.diagnostics.validate_pulse_width(
                    nucleus="1H",
                    measured_pw=h1_result["value"],
                    probe_id=config.probe_id
                )
                results["validation"]["warnings"].extend(h1_validation.get("warnings", []))

            # Step 2: Initial CP optimization (coarse)
            cp_result = self._optimize_cp_initial(config)
            results["calibrations"]["CP_initial"] = cp_result

            # Check for CP pathologies
            pathology_check = self.cp_detector.analyze_cp_result(cp_result)
            if pathology_check["has_pathology"]:
                print(f"\nCP pathology detected: {pathology_check['type']}")
                results["validation"]["warnings"].append(
                    f"CP pathology: {pathology_check['description']}"
                )

            # Step 3: X-nucleus calibrations
            for nucleus in config.nuclei:
                if nucleus == "1H":
                    continue

                if not config.skip_x_calibration:
                    x_result = self._calibrate_x_nucleus(nucleus, config, cp_result)
                    results["calibrations"][f"{nucleus}_pw90"] = x_result
                    self.current_calibrations[f"{nucleus}_pw90"] = x_result["value"]

                    # Validate X-nucleus calibration
                    x_validation = self.diagnostics.validate_pulse_width_ratio(
                        h1_pw=self.current_calibrations.get("H1_pw90", 0),
                        x_pw=x_result["value"],
                        x_nucleus=nucleus
                    )
                    results["validation"]["warnings"].extend(x_validation.get("warnings", []))

            # Step 4: CP fine-tuning
            cp_final = self._optimize_cp_final(config, cp_result)
            results["calibrations"]["CP_final"] = cp_final

            # Final validation
            overall_status = self._validate_overall(results, config)
            results["validation"]["status"] = overall_status

            print(f"\n{'='*60}")
            print(f"Calibration workflow completed: {overall_status}")
            print(f"{'='*60}\n")

        except Exception as e:
            results["validation"]["status"] = "failed"
            results["validation"]["errors"].append(str(e))
            print(f"\nWorkflow failed: {e}\n")

        return results

    def _calibrate_h1_pulse_width(self, config: WorkflowConfig) -> Dict:
        """
        Calibrate H1 90-degree pulse width.

        Uses nutation curve with intelligent search range.
        """
        print("\n[Step 1] H1 Pulse Width Calibration")
        print("-" * 40)

        # Get initial guess (from database or theoretical)
        initial_guess = self._get_h1_initial_guess(config)
        print(f"Initial guess: {initial_guess:.2f} μs")

        # Define search range (±25%)
        pw_min = initial_guess * (1 - config.h1_pw_tolerance)
        pw_max = initial_guess * (1 + config.h1_pw_tolerance)

        # Submit OPTO job for nutation calibration
        job_config = OptoJobConfig(
            job_type="nutation",
            nucleus="1H",
            param1_name="pulse_width",
            param1_min=pw_min,
            param1_max=pw_max,
            param1_steps=7,  # Sample 7 points
            spinning_rate=config.spinning_rate,
            temperature=config.temperature,
            experiment_name="h1_nutation",
            num_scans=8,
            relaxation_delay=2.0
        )

        result = self.opto.submit_job(job_config, wait_for_completion=True, timeout=300)

        if result.status == "completed":
            # Extract 90-degree pulse width
            pw90 = result.optimized_params.get("pulse_width", 0)

            # Calculate deviation from guess
            deviation = abs(pw90 - initial_guess) / initial_guess

            print(f"✓ H1 90° pulse width: {pw90:.2f} μs")
            print(f"  Deviation from guess: {deviation*100:.1f}%")

            return {
                "value": pw90,
                "deviation": deviation,
                "score": result.score,
                "job_id": result.job_id
            }
        else:
            raise RuntimeError(f"H1 calibration failed: {result.status}")

    def _optimize_cp_initial(self, config: WorkflowConfig) -> Dict:
        """
        Initial CP optimization using theoretical match conditions.

        Calculates expected match conditions based on spinning rate
        and explores the most promising ones.
        """
        print("\n[Step 2] Initial CP Optimization (Coarse)")
        print("-" * 40)

        # Calculate theoretical match conditions
        match_conditions = self._calculate_match_conditions(config.spinning_rate)

        print(f"Spinning rate regime: {match_conditions['regime']}")
        print(f"Exploring {len(match_conditions['conditions'])} match conditions:")
        for cond in match_conditions['conditions']:
            print(f"  - {cond['description']}: "
                  f"H1={cond['h1_field']/1000:.1f} kHz, "
                  f"X={cond['x_field']/1000:.1f} kHz")

        # Try primary match condition first
        primary_cond = match_conditions['conditions'][0]
        cp_result = self._run_cp_optimization(primary_cond, config)

        # Check if CP efficiency is acceptable
        if cp_result.get("efficiency", 0) < config.cp_efficiency_threshold * 100:
            print(f"\n⚠ Primary condition gave low efficiency: "
                  f"{cp_result.get('efficiency', 0):.1f}%")
            print("  Systematically exploring alternative match conditions...")

            # Try other conditions
            for alt_cond in match_conditions['conditions'][1:]:
                alt_result = self._run_cp_optimization(alt_cond, config)

                if alt_result.get("efficiency", 0) > cp_result.get("efficiency", 0):
                    print(f"✓ Found better condition: {alt_cond['description']}")
                    cp_result = alt_result
                    cp_result["match_condition"] = alt_cond

                if cp_result.get("efficiency", 0) >= config.cp_efficiency_threshold * 100:
                    break

        return cp_result

    def _calculate_match_conditions(self, spinning_rate: float) -> Dict:
        """
        Calculate theoretical CP match conditions based on spinning rate.

        Parameters
        ----------
        spinning_rate : float
            MAS spinning rate in Hz

        Returns
        -------
        dict
            Match conditions for this spinning rate regime
        """
        conditions = []

        # Determine regime
        if spinning_rate > 30000:
            regime = "high-speed"
            # Low-power CP conditions
            conditions = [
                {
                    "description": "2/3 - 1/3 match",
                    "h1_field": (2/3) * spinning_rate,
                    "x_field": (1/3) * spinning_rate,
                    "n": "0"
                },
                {
                    "description": "3/4 - 1/4 match",
                    "h1_field": (3/4) * spinning_rate,
                    "x_field": (1/4) * spinning_rate,
                    "n": "0"
                },
                {
                    "description": "1/3 - 2/3 match (reversed)",
                    "h1_field": (1/3) * spinning_rate,
                    "x_field": (2/3) * spinning_rate,
                    "n": "0"
                }
            ]

        elif spinning_rate >= 15000:
            regime = "medium-speed"
            # n=±1 sideband matches
            for n in [1, -1, 2, -2]:
                conditions.append({
                    "description": f"n={n} match",
                    "h1_field": spinning_rate + n * spinning_rate * 0.1,  # Approximate
                    "x_field": spinning_rate,
                    "n": str(n)
                })

        else:
            regime = "low-speed"
            # Broader sideband exploration
            for n in range(-3, 4):
                conditions.append({
                    "description": f"n={n} match",
                    "h1_field": spinning_rate * (1 + 0.2 * n),  # Approximate
                    "x_field": spinning_rate,
                    "n": str(n)
                })

        return {
            "regime": regime,
            "conditions": conditions
        }

    def _run_cp_optimization(self, match_condition: Dict, config: WorkflowConfig) -> Dict:
        """Run single CP optimization at given match condition."""

        # Define search range around theoretical match (±20%)
        h1_center = match_condition["h1_field"]
        x_center = match_condition["x_field"]

        job_config = OptoJobConfig(
            job_type="cp_optimization",
            nucleus="13C",  # Default to 13C
            param1_name="h1_field",
            param1_min=h1_center * 0.8,
            param1_max=h1_center * 1.2,
            param1_steps=5,
            param2_name="c13_field",
            param2_min=x_center * 0.8,
            param2_max=x_center * 1.2,
            param2_steps=5,
            spinning_rate=config.spinning_rate,
            temperature=config.temperature,
            contact_time=2.0,  # 2 ms default
            ramp_type="down",  # Try ramp down first
            experiment_name="cp_opt",
            num_scans=16,
            relaxation_delay=2.0,
            scoring_method="symmetry_aware"
        )

        print(f"  Testing: {match_condition['description']}...")
        result = self.opto.submit_job(job_config, wait_for_completion=True, timeout=600)

        if result.status == "completed":
            print(f"    Efficiency: {result.efficiency:.1f}%")
            return {
                "h1_field": result.optimized_params.get("h1_field", 0),
                "x_field": result.optimized_params.get("c13_field", 0),
                "efficiency": result.efficiency,
                "score": result.score,
                "match_condition": match_condition,
                "job_id": result.job_id
            }
        else:
            return {
                "efficiency": 0,
                "score": 0,
                "match_condition": match_condition,
                "status": "failed"
            }

    def _calibrate_x_nucleus(self, nucleus: str, config: WorkflowConfig,
                            cp_result: Dict) -> Dict:
        """
        Calibrate X-nucleus pulse width using CP signal.
        """
        print(f"\n[Step 3] {nucleus} Pulse Width Calibration")
        print("-" * 40)

        # Calculate theoretical pulse width based on H1
        h1_pw = self.current_calibrations.get("H1_pw90", 2.5)
        theoretical_pw = self._calculate_theoretical_x_pw(h1_pw, nucleus)

        print(f"Theoretical {nucleus} pw90: {theoretical_pw:.2f} μs")

        # Use CP to generate signal and calibrate
        pw_min = theoretical_pw * 0.7
        pw_max = theoretical_pw * 1.3

        job_config = OptoJobConfig(
            job_type="nutation_via_cp",
            nucleus=nucleus,
            param1_name="pulse_width",
            param1_min=pw_min,
            param1_max=pw_max,
            param1_steps=7,
            spinning_rate=config.spinning_rate,
            temperature=config.temperature,
            contact_time=cp_result.get("contact_time", 2.0),
            experiment_name=f"{nucleus}_nutation",
            num_scans=16
        )

        result = self.opto.submit_job(job_config, wait_for_completion=True, timeout=300)

        if result.status == "completed":
            pw90 = result.optimized_params.get("pulse_width", 0)
            print(f"✓ {nucleus} 90° pulse width: {pw90:.2f} μs")

            return {
                "value": pw90,
                "theoretical": theoretical_pw,
                "score": result.score,
                "job_id": result.job_id
            }
        else:
            raise RuntimeError(f"{nucleus} calibration failed: {result.status}")

    def _optimize_cp_final(self, config: WorkflowConfig, initial_cp: Dict) -> Dict:
        """
        Final CP optimization with validated pulse widths.

        Refines ramp parameters and contact time.
        """
        print("\n[Step 4] CP Fine-Tuning")
        print("-" * 40)

        # Use optimized parameters from initial CP
        h1_field = initial_cp["h1_field"]
        x_field = initial_cp["x_field"]

        # Refine with narrower range and explore ramp types
        best_result = initial_cp
        best_ramp = None

        for ramp_type in ["up", "down", "from", "to"]:
            print(f"  Trying ramp {ramp_type}...")

            job_config = OptoJobConfig(
                job_type="cp_refinement",
                nucleus="13C",
                param1_name="h1_field",
                param1_min=h1_field * 0.95,
                param1_max=h1_field * 1.05,
                param1_steps=3,
                param2_name="contact_time",
                param2_min=0.5,
                param2_max=5.0,
                param2_steps=5,
                spinning_rate=config.spinning_rate,
                temperature=config.temperature,
                ramp_type=ramp_type,
                experiment_name="cp_final",
                num_scans=32  # More scans for accuracy
            )

            result = self.opto.submit_job(job_config, wait_for_completion=True, timeout=600)

            if result.status == "completed":
                if result.efficiency > best_result.get("efficiency", 0):
                    best_result = {
                        "h1_field": result.optimized_params.get("h1_field", h1_field),
                        "contact_time": result.optimized_params.get("contact_time", 2.0),
                        "efficiency": result.efficiency,
                        "ramp_type": ramp_type,
                        "score": result.score,
                        "job_id": result.job_id
                    }
                    best_ramp = ramp_type

        print(f"\n✓ Optimized CP:")
        print(f"  Efficiency: {best_result['efficiency']:.1f}%")
        print(f"  Best ramp: {best_ramp}")
        print(f"  Contact time: {best_result['contact_time']:.2f} ms")

        return best_result

    def _get_h1_initial_guess(self, config: WorkflowConfig) -> float:
        """Get initial guess for H1 pulse width."""
        # Placeholder - would query database in full implementation
        # For now, use typical value
        return 2.5  # μs

    def _calculate_theoretical_x_pw(self, h1_pw: float, x_nucleus: str) -> float:
        """Calculate theoretical X-nucleus pulse width from H1."""
        h1_gamma = self.GYROMAGNETIC_RATIOS["1H"]
        x_gamma = self.GYROMAGNETIC_RATIOS.get(x_nucleus, 10.0)

        # Pulse widths scale inversely with gyromagnetic ratio
        return h1_pw * (h1_gamma / x_gamma)

    def _validate_overall(self, results: Dict, config: WorkflowConfig) -> str:
        """
        Validate complete calibration results.

        Returns overall status: "PASS", "WARNING", "ALERT", "FAIL"
        """
        warnings = results["validation"]["warnings"]
        errors = results["validation"]["errors"]

        if errors:
            return "FAIL"

        # Check CP efficiency
        cp_final = results["calibrations"].get("CP_final", {})
        if cp_final.get("efficiency", 0) < 30:
            return "FAIL"
        elif cp_final.get("efficiency", 0) < 50:
            return "WARNING"

        # Check for warnings
        if len(warnings) > 3:
            return "ALERT"
        elif len(warnings) > 0:
            return "WARNING"

        return "PASS"
