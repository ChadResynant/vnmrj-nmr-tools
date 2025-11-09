#!/usr/bin/env python3
"""
OPTO Diagnostics Module

Implements expert diagnostic capabilities for:
- Hardware problem detection (tuning, power delivery, instabilities)
- CP optimization pathology detection (local optima, T1ρ issues)
- Parameter validation against expected values
- Multi-level warning system (Green/Yellow/Orange/Red)

Embodies expert knowledge about common failure modes and their signatures.
"""

import numpy as np
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum


class ValidationLevel(Enum):
    """Validation severity levels."""
    GREEN = "GREEN"      # ±20%: Normal range
    YELLOW = "YELLOW"    # 20-50%: Attention recommended
    ORANGE = "ORANGE"    # 50-100%: Action recommended
    RED = "RED"          # >100%: Stop and troubleshoot


@dataclass
class DiagnosticResult:
    """Result of a diagnostic check."""
    level: ValidationLevel
    parameter: str
    expected_value: float
    actual_value: float
    deviation_percent: float
    message: str
    recommended_actions: List[str]


class HardwareDiagnostics:
    """
    Hardware diagnostic checks for NMR instrumentation.

    Detects common hardware problems based on calibration results.
    """

    # Expected pulse width ratios relative to H1
    EXPECTED_PW_RATIOS = {
        "13C": 4.0,   # γ(H1)/γ(C13) ≈ 4.0
        "15N": 10.0,  # γ(H1)/γ(N15) ≈ 10.0
        "31P": 2.5    # γ(H1)/γ(P31) ≈ 2.5
    }

    # Tolerance thresholds for different severity levels
    THRESHOLDS = {
        ValidationLevel.GREEN: 0.20,   # ±20%
        ValidationLevel.YELLOW: 0.50,  # 20-50%
        ValidationLevel.ORANGE: 1.00,  # 50-100%
        ValidationLevel.RED: float('inf')  # >100%
    }

    # Variability thresholds (RSD %)
    VARIABILITY_THRESHOLDS = {
        "pulse_width": 0.10,  # 10% RSD for pulse widths
        "cp_efficiency": 0.15  # 15% RSD for CP efficiency
    }

    def __init__(self, database_path: Optional[str] = None):
        """
        Initialize hardware diagnostics.

        Parameters
        ----------
        database_path : str, optional
            Path to historical calibration database
        """
        self.database_path = database_path
        # Would load database statistics here in full implementation
        self.database_stats = {}

    def validate_pulse_width(self,
                           nucleus: str,
                           measured_pw: float,
                           probe_id: str,
                           expected_pw: Optional[float] = None) -> Dict:
        """
        Validate pulse width measurement against expected value.

        Parameters
        ----------
        nucleus : str
            Nucleus (e.g., "1H", "13C")
        measured_pw : float
            Measured pulse width (μs)
        probe_id : str
            Probe identifier
        expected_pw : float, optional
            Expected pulse width (from database or theoretical)

        Returns
        -------
        dict
            Validation results with warnings/alerts
        """
        # Get expected value if not provided
        if expected_pw is None:
            expected_pw = self._get_expected_pulse_width(nucleus, probe_id)

        # Calculate deviation
        deviation = abs(measured_pw - expected_pw) / expected_pw

        # Determine severity level
        level = self._determine_validation_level(deviation)

        # Generate diagnostic result
        result = DiagnosticResult(
            level=level,
            parameter=f"{nucleus}_pw90",
            expected_value=expected_pw,
            actual_value=measured_pw,
            deviation_percent=deviation * 100,
            message=self._generate_pw_message(nucleus, measured_pw, expected_pw, deviation),
            recommended_actions=self._generate_pw_actions(measured_pw, expected_pw)
        )

        return {
            "level": result.level.value,
            "deviation": result.deviation_percent,
            "warnings": [result.message] if level != ValidationLevel.GREEN else [],
            "actions": result.recommended_actions,
            "details": result
        }

    def validate_pulse_width_ratio(self,
                                   h1_pw: float,
                                   x_pw: float,
                                   x_nucleus: str) -> Dict:
        """
        Validate X-nucleus/H1 pulse width ratio.

        This is a critical hardware diagnostic - large deviations indicate
        tuning or power delivery problems.

        Parameters
        ----------
        h1_pw : float
            H1 pulse width (μs)
        x_pw : float
            X-nucleus pulse width (μs)
        x_nucleus : str
            X nucleus identifier

        Returns
        -------
        dict
            Validation results
        """
        expected_ratio = self.EXPECTED_PW_RATIOS.get(x_nucleus, 4.0)
        actual_ratio = x_pw / h1_pw if h1_pw > 0 else 0

        deviation = abs(actual_ratio - expected_ratio) / expected_ratio
        level = self._determine_validation_level(deviation)

        message = f"Pulse width ratio inconsistency detected"
        if deviation > 0.40:  # Expert threshold from input
            message = (f"{x_nucleus}/H1 pulse width ratio = {actual_ratio:.2f} "
                      f"(expected ~{expected_ratio:.1f}, deviation {deviation*100:.0f}%)")

        actions = []
        if level in [ValidationLevel.ORANGE, ValidationLevel.RED]:
            actions = [
                "Check probe tuning for both H1 and X channels",
                "Verify RF power delivery on both channels",
                "Inspect cable connections",
                "Check for over-coupling or under-coupling",
                "Verify amplifier outputs are delivering rated power"
            ]

        return {
            "level": level.value,
            "ratio": actual_ratio,
            "expected_ratio": expected_ratio,
            "deviation": deviation * 100,
            "warnings": [message] if level != ValidationLevel.GREEN else [],
            "actions": actions
        }

    def detect_variability_issues(self,
                                  measurements: List[float],
                                  parameter_type: str) -> Dict:
        """
        Detect unusually high variability in repeated measurements.

        High variability indicates instability: temperature, spinning, RF, etc.

        Parameters
        ----------
        measurements : list of float
            Repeated measurements of same parameter
        parameter_type : str
            Type of parameter ("pulse_width" or "cp_efficiency")

        Returns
        -------
        dict
            Variability analysis and warnings
        """
        if len(measurements) < 2:
            return {"status": "insufficient_data"}

        mean = np.mean(measurements)
        std = np.std(measurements, ddof=1)
        rsd = std / mean if mean != 0 else float('inf')  # Relative standard deviation

        threshold = self.VARIABILITY_THRESHOLDS.get(parameter_type, 0.10)

        if rsd > threshold:
            level = ValidationLevel.YELLOW if rsd < 2*threshold else ValidationLevel.ORANGE

            message = (f"High variability detected in {parameter_type}: "
                      f"{mean:.2f} ± {std:.2f} (RSD = {rsd*100:.1f}%)")

            actions = [
                "Allow more time for temperature stabilization",
                "Check spinning rate stability (±2 Hz required)",
                "Verify bearing air pressure is stable",
                "Check for probe tuning drift",
                "Consider sample heating/cooling effects",
                "Reduce RF power if excessive sample heating suspected"
            ]

            return {
                "level": level.value,
                "mean": mean,
                "std": std,
                "rsd": rsd * 100,
                "threshold_rsd": threshold * 100,
                "warnings": [message],
                "actions": actions
            }

        return {
            "level": ValidationLevel.GREEN.value,
            "mean": mean,
            "std": std,
            "rsd": rsd * 100,
            "status": "acceptable_variability"
        }

    def _get_expected_pulse_width(self, nucleus: str, probe_id: str) -> float:
        """Get expected pulse width from database or typical values."""
        # Placeholder - would query database in full implementation
        typical_values = {
            "1H": 2.5,
            "13C": 4.0,
            "15N": 5.0,
            "31P": 3.5
        }
        return typical_values.get(nucleus, 3.0)

    def _determine_validation_level(self, deviation: float) -> ValidationLevel:
        """Determine validation level from deviation."""
        if deviation < self.THRESHOLDS[ValidationLevel.GREEN]:
            return ValidationLevel.GREEN
        elif deviation < self.THRESHOLDS[ValidationLevel.YELLOW]:
            return ValidationLevel.YELLOW
        elif deviation < self.THRESHOLDS[ValidationLevel.ORANGE]:
            return ValidationLevel.ORANGE
        else:
            return ValidationLevel.RED

    def _generate_pw_message(self, nucleus: str, measured: float,
                            expected: float, deviation: float) -> str:
        """Generate diagnostic message for pulse width validation."""
        if deviation < 0.20:
            return f"{nucleus} pulse width within expected range"

        direction = "longer" if measured > expected else "shorter"
        return (f"{nucleus} pulse width {direction} than expected: "
               f"{measured:.2f} μs (expected ~{expected:.2f} μs, "
               f"deviation {deviation*100:.0f}%)")

    def _generate_pw_actions(self, measured: float, expected: float) -> List[str]:
        """Generate recommended actions for pulse width issues."""
        actions = []

        if measured < expected:  # Pulse width too short
            actions = [
                "Check for probe over-coupling (dip too deep)",
                "Verify probe tuning and matching",
                "Check cable connections for high resistance",
                "Verify amplifier gain settings",
                "Inspect for sample-induced field enhancement (rare)"
            ]
        else:  # Pulse width too long
            actions = [
                "Check for probe under-coupling (dip too shallow)",
                "Re-tune probe for deeper dip on wobble curve",
                "Verify amplifier is delivering full power",
                "Check for cable loss or poor connections",
                "If sample is very salty/conductive, this may be normal"
            ]

        return actions


class CPPathologyDetector:
    """
    Detector for CP optimization pathologies.

    Identifies common failure modes in CP optimization.
    """

    def __init__(self):
        """Initialize CP pathology detector."""
        pass

    def analyze_cp_result(self, cp_result: Dict) -> Dict:
        """
        Analyze CP optimization result for pathologies.

        Parameters
        ----------
        cp_result : dict
            CP optimization result

        Returns
        -------
        dict
            Pathology analysis
        """
        pathologies = []

        # Check for low efficiency
        efficiency = cp_result.get("efficiency", 0)
        if efficiency < 30:
            pathologies.append({
                "type": "low_efficiency",
                "description": "CP efficiency below 30% threshold",
                "severity": "critical",
                "possible_causes": [
                    "Hartmann-Hahn mismatch (incorrect spinning rate?)",
                    "Incorrect pulse widths used in CP",
                    "Poor T1ρ (mismatched decoupling)",
                    "Probe tuning shifted during experiment"
                ],
                "recommended_actions": [
                    "Verify spinning rate is stable",
                    "Re-verify H1 and X pulse widths",
                    "Check probe tuning before CP optimization",
                    "Try different CP match condition"
                ]
            })

        # Check for multiple local optima (would need full optimization landscape)
        # Placeholder for more sophisticated analysis
        if self._detect_multiple_optima(cp_result):
            pathologies.append({
                "type": "multiple_local_optima",
                "description": "Multiple local maxima detected in CP landscape",
                "severity": "warning",
                "possible_causes": [
                    "Several match conditions give similar efficiency",
                    "Need to explore broader parameter space"
                ],
                "recommended_actions": [
                    "Perform systematic exploration of all theoretical match conditions",
                    "Compare efficiencies across different n values",
                    "Consider using 2D optimization grid"
                ]
            })

        # Check for probe drift
        if self._detect_probe_drift(cp_result):
            pathologies.append({
                "type": "probe_drift",
                "description": "Probe tuning appears to have shifted during optimization",
                "severity": "warning",
                "possible_causes": [
                    "Sample heating from RF",
                    "Bearing pressure fluctuations",
                    "Temperature changes"
                ],
                "recommended_actions": [
                    "Re-tune probe and retry",
                    "Reduce RF power to minimize heating",
                    "Allow more temperature stabilization time",
                    "Check bearing air pressure stability"
                ]
            })

        return {
            "has_pathology": len(pathologies) > 0,
            "pathologies": pathologies,
            "type": pathologies[0]["type"] if pathologies else None,
            "description": pathologies[0]["description"] if pathologies else "No pathologies detected"
        }

    def _detect_multiple_optima(self, cp_result: Dict) -> bool:
        """
        Detect multiple local optima in CP optimization.

        Placeholder - full implementation would analyze entire optimization
        trajectory to identify multiple peaks.
        """
        # Would check if optimization converged to different solutions
        # from different starting points
        return False

    def _detect_probe_drift(self, cp_result: Dict) -> bool:
        """
        Detect probe tuning drift during optimization.

        Placeholder - would analyze how optimal parameters changed
        over the course of optimization.
        """
        # Would check if best parameters shifted systematically
        return False

    def analyze_ramp_asymmetry(self,
                              ramp_up_result: Dict,
                              ramp_down_result: Dict) -> Dict:
        """
        Analyze asymmetry between ramp-up and ramp-down CP.

        Large asymmetry indicates RF inhomogeneity or phase issues.

        Parameters
        ----------
        ramp_up_result : dict
            CP results with ramp-up configuration
        ramp_down_result : dict
            CP results with ramp-down configuration

        Returns
        -------
        dict
            Asymmetry analysis
        """
        eff_up = ramp_up_result.get("efficiency", 0)
        eff_down = ramp_down_result.get("efficiency", 0)

        if eff_up == 0 or eff_down == 0:
            return {"status": "insufficient_data"}

        asymmetry = abs(eff_up - eff_down) / max(eff_up, eff_down)

        if asymmetry > 0.30:  # >30% difference
            return {
                "asymmetry": asymmetry * 100,
                "level": ValidationLevel.ORANGE.value,
                "message": (f"Significant ramp asymmetry detected: "
                          f"up={eff_up:.1f}%, down={eff_down:.1f}%"),
                "possible_causes": [
                    "RF field inhomogeneity across sample",
                    "Phase transients during ramp",
                    "Power amplifier non-linearity"
                ],
                "actions": [
                    "Check RF coil uniformity",
                    "Verify phase coherence during ramp",
                    "Test with different ramp profiles"
                ]
            }

        return {
            "asymmetry": asymmetry * 100,
            "level": ValidationLevel.GREEN.value,
            "status": "acceptable_asymmetry"
        }


class WorkflowValidator:
    """
    Validates complete calibration workflows.

    Combines hardware diagnostics and CP pathology detection to
    provide overall assessment of calibration quality.
    """

    def __init__(self):
        """Initialize workflow validator."""
        self.hardware = HardwareDiagnostics()
        self.cp_detector = CPPathologyDetector()

    def validate_complete_workflow(self, results: Dict) -> Dict:
        """
        Validate complete calibration workflow results.

        Parameters
        ----------
        results : dict
            Complete workflow results

        Returns
        -------
        dict
            Comprehensive validation report
        """
        validation = {
            "overall_status": "PENDING",
            "warnings": [],
            "errors": [],
            "diagnostics": [],
            "recommendations": []
        }

        # Validate each calibration
        for param_name, param_data in results.get("calibrations", {}).items():
            # Add parameter-specific validation
            pass  # Would implement detailed checks here

        # Determine overall status
        if validation["errors"]:
            validation["overall_status"] = "FAIL"
        elif len(validation["warnings"]) > 3:
            validation["overall_status"] = "ALERT"
        elif validation["warnings"]:
            validation["overall_status"] = "WARNING"
        else:
            validation["overall_status"] = "PASS"

        return validation
