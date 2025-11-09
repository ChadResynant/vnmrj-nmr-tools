# Optimizing the Opto Front End Interface: A Comprehensive Workflow for Enhanced User Experience

**Date:** November 9, 2025  
**Author:** Chad Rienstra  
**Project:** OPTO Front-End Development  
**Organization:** NMRFAM / Resynant Inc.

---

## Executive Summary

This document outlines the comprehensive design for an intelligent front-end interface for the OPTO optimization platform. The system aims to make automated NMR pulse calibration and cross-polarization optimization accessible to non-experts while maintaining the sophistication required for advanced users. The interface will provide context-aware defaults, intelligent workflow guidance, comprehensive safety checks, and historical data mining capabilities.

---

## Current State & Motivation

### Problems to Address
1. **Documentation Gap**: OPTO documentation exists but needs significant improvement
2. **Accessibility**: Non-experts struggle with optimization workflows
3. **Consistency**: Need standardized approaches across different users and instruments
4. **Safety**: Insufficient warnings for problematic calibrations
5. **Knowledge Transfer**: Song Lin's successful approaches need to be systematized

### Key Insight
Song Lin achieves excellent results by calculating initial pulse widths and deriving CP conditions from theoretical models, then refining with OPTO. This approach should be formalized and made accessible through the interface.

---

## System Architecture

### Overall Design

```
┌─────────────────────────────────────────────────────────┐
│           OPTO Front-End Interface (Web/GUI)            │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐   │
│  │ Calibration  │ │      CP      │ │   Workflow   │   │
│  │    Status    │ │ Optimization │ │    Status    │   │
│  └──────────────┘ └──────────────┘ └──────────────┘   │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐   │
│  │   Database   │ │   Warnings   │ │   Reports    │   │
│  │ Intelligence │ │   & Alerts   │ │ & Logging    │   │
│  └──────────────┘ └──────────────┘ └──────────────┘   │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│              OPTO Backend Engine                         │
│   • Parameter Calculation                                │
│   • Optimization Algorithms                              │
│   • Symmetry-Aware Scoring                               │
│   • Database Management                                  │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│         Spectrometer Control Layer                       │
│   • VnmrJ/OBJ (Varian)                                   │
│   • TopSpin (Bruker)                                     │
└─────────────────────────────────────────────────────────┘
```

---

## Dashboard Components

### 1. Calibration Status Panel

**Purpose**: Real-time view of all critical pulse parameters with quality indicators

**Display Elements**:

| Parameter Type | Nucleus | Value | Status | Last Cal | Deviation |
|---------------|---------|-------|--------|----------|-----------|
| Hard Pulse (90°) | ¹H | 2.5 μs | 🟢 | 10 min ago | -5% |
| Hard Pulse (90°) | ¹³C | 3.8 μs | 🟡 | 10 min ago | +22% |
| Hard Pulse (90°) | ¹⁵N | 4.2 μs | 🟢 | 10 min ago | +8% |
| TPPM Decoupling | ¹H | 10 kHz | 🟢 | 15 min ago | -2% |
| CP Efficiency | ¹H→¹³C | 68% | 🟢 | 12 min ago | +5% |

**Status Indicators**:
- 🟢 **Green**: Within ±20% of expected/previous values
- 🟡 **Yellow**: 20-50% deviation, warrants attention
- 🟠 **Orange**: 50-100% deviation, likely problem
- 🔴 **Red**: >100% deviation or calibration failed

**Features**:
- Click any parameter to view detailed calibration curve
- Historical trend plots (last 10 samples)
- Quick recalibration button
- Export parameters to spectrometer

---

### 2. CP Optimization Panel

**Purpose**: Context-aware cross-polarization optimization with intelligent defaults

#### Spinning Rate Context

**High-Speed Regime (ωr > 30 kHz)**
- **Primary Default**: Low-power CP
  - ¹H: 2/3 ωr to 3/4 ωr
  - ¹³C/¹⁵N: 1/4 ωr to 1/3 ωr
- **Alternative Conditions to Explore**:
  - 2/3 - 1/3 match
  - 3/4 - 1/4 match
  - 1/3 - 2/3 (reversed)

**Medium-Speed Regime (15 ≤ ωr ≤ 30 kHz)**
- **Primary Default**: n = ±1 sideband match
  - ¹H: ωr ± offset
  - ¹³C/¹⁵N: calculated for match condition
- **Alternative Conditions to Explore**:
  - n = 0 (zero quantum)
  - n = ±2 matches
  - Double quantum (n = -1, -1)

**Low-Speed Regime (ωr < 15 kHz)**
- **Primary Default**: n = 1, 2 conditions
- **Explore broader sideband manifold**
- **Consider adiabatic CP alternatives**

#### Ramp Configuration

**Available Options** (Varian current capability):
1. **Ramp Up**: Start below match, ramp to above
2. **Ramp Down**: Start above match, ramp to below
3. **Ramp From**: Start at match, ramp away
4. **Ramp To**: Start away from match, ramp to

**Future Enhancement** (dual-channel ramping):
- Simultaneous ramping on both ¹H and X channels
- More complex trajectories through match manifold

**Optimization Strategy**:
```
Branch Parallel OPTO Jobs:
├── Job 1: Default condition (2/3 - 1/3)
│   ├── Ramp Up
│   └── Ramp Down
├── Job 2: Alternative 1 (3/4 - 1/4)
│   ├── Ramp Up
│   └── Ramp Down
└── Job 3: Alternative 2 (1/3 - 2/3)
    ├── Ramp Up
    └── Ramp Down
    
→ Best performer becomes new default for this condition
→ Results logged to database for future predictions
```

---

### 3. Workflow Status Panel

**Purpose**: Sequential checklist with automatic progression and validation

#### Complete Workflow Sequence

```
SAMPLE PREPARATION
☑ 1. Sample loaded and spinning
    ↳ Spinning rate: 20.000 ± 0.002 kHz ✓
    ↳ Stable for: 5 minutes ✓

☑ 2. Probe tuning completed
    ↳ ¹H: -45 dB @ 600.13 MHz ✓
    ↳ ¹³C: -42 dB @ 150.90 MHz ✓
    ↳ ¹⁵N: -38 dB @ 60.82 MHz ✓

☑ 3. Temperature stabilization
    ↳ Target: 298.0 K
    ↳ Current: 298.0 K (stable ±0.1 K for 10 min) ✓

☐ 4. Magic angle verification (OPTIONAL)
    ↳ KBr sample required
    ↳ Skip for now | Run check

☐ 5. Shimming optimization (OPTIONAL)
    ↳ Depends on sample
    ↳ Skip for now | Run autoshim

CALIBRATIONS (AUTOMATED)
→ 6. ¹H pulse width calibration (IN PROGRESS)
    ↳ Previous sample: 2.4 μs
    ↳ Database average: 2.5 μs
    ↳ Testing array: [2.0, 2.2, 2.4, 2.6, 2.8, 3.0] μs
    ↳ Progress: ████████░░ 80%

☐ 7. Initial CP optimization (coarse)
    ↳ Using theoretical match conditions
    ↳ Estimated time: 8 minutes

☐ 8. ¹³C pulse width calibration
    ↳ Will use CP signal
    ↳ Estimated time: 5 minutes

☐ 9. ¹⁵N pulse width calibration
    ↳ Will use CP signal
    ↳ Estimated time: 5 minutes

☐ 10. CP fine-tuning with known pulses
    ↳ Refine ramp parameters
    ↳ Estimated time: 12 minutes

VALIDATION
☐ 11. Parameter validation
    ↳ Compare to theoretical models
    ↳ Check against database statistics
    ↳ Generate QC report

✓ READY FOR EXPERIMENTS
    Total setup time: ~35 minutes
```

**Interactive Features**:
- Click any step to view details
- Pause/resume workflow at any point
- Skip optional steps with confirmation
- View live data during acquisitions
- Override automated decisions if needed

---

### 4. Database Intelligence Panel

**Purpose**: Historical analysis and predictive capabilities

#### Features

**Historical Comparison**
```
Current Sample vs Last 10 Samples (Same Probe)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

¹H Pulse Width:
2.8 ┤                                    ●
2.6 ┤              ●         ●     ●
2.4 ┤    ●    ●         ●              ○ ← Current
2.2 ┤         ●    ●
2.0 ┤ ●
    └─────────────────────────────────────→
    Oct 15        Oct 30        Nov 14

Status: Within normal range ✓
Trend: Slight increase (probe aging expected)
```

**Similar Sample Retrieval**
- Find previous samples with similar:
  - Spinning rate (±2 kHz)
  - Temperature (±5 K)
  - Probe configuration
  - Sample type (if metadata available)
- Display their calibration parameters
- One-click load of previous parameters

**Trend Detection**
- **Probe Aging**: Gradual increase in pulse widths over months
- **Seasonal Effects**: Temperature-dependent tuning shifts
- **Maintenance Events**: Sharp changes after service/repairs
- **Warnings**: Alert if trends suggest upcoming probe issues

**Anomaly Detection**
```python
# Statistical outlier detection
IF current_value > mean + 2*std_dev:
    FLAG as anomaly
    SUGGEST: Check tuning, cables, amplifier
    
IF current_value < mean - 2*std_dev:
    FLAG as anomaly
    SUGGEST: Over-coupling, sample conductivity issue
```

---

## Intelligent Workflow Engine

### Calibration Sequence Logic

#### Step 1: Load Previous Calibration
```python
def get_initial_guess():
    """Priority hierarchy for initial parameter guess"""
    
    # Priority 1: Same probe, same day (last 12 hours)
    if exists(same_probe_recent):
        return same_probe_recent
    
    # Priority 2: Same probe, last week
    elif exists(same_probe_week):
        return same_probe_week * adjustment_factor
    
    # Priority 3: Database average for this probe
    elif exists(probe_average):
        return probe_average
    
    # Priority 4: Generic default
    else:
        return calculate_theoretical_default()
```

#### Step 2: ¹H Pulse Width Calibration
```python
def calibrate_proton_pulse(guess):
    """
    Quick nutation calibration with intelligent array
    """
    # Generate array around guess (±25%)
    array = np.linspace(guess * 0.75, guess * 1.25, 6)
    
    # Acquire nutation curve
    result = acquire_nutation(array)
    
    # Fit and extract 90° pulse width
    pw90 = fit_nutation_curve(result)
    
    # Validation checks
    deviation = abs(pw90 - guess) / guess
    
    if deviation < 0.20:
        log_info(f"Pulse width within 20% of guess: {pw90:.2f} μs")
        return pw90, "PASS"
    
    elif deviation < 0.50:
        log_warning(f"Pulse width differs by {deviation*100:.0f}%")
        log_warning("Possible tuning issue or sample properties")
        return pw90, "WARNING"
    
    elif deviation < 1.00:
        log_alert(f"Pulse width differs by {deviation*100:.0f}%")
        log_alert("Likely tuning problem - suggest re-tune")
        return pw90, "ALERT"
    
    else:
        log_error(f"Pulse width differs by {deviation*100:.0f}%")
        log_error("STOP - require user intervention")
        return None, "FAIL"
```

#### Step 3: Coarse CP Optimization
```python
def initial_cp_optimization(spinning_rate, h1_power, x_nucleus):
    """
    Calculate theoretical match conditions and perform 2D grid search
    """
    # Calculate expected match conditions
    conditions = []
    
    if spinning_rate > 30e3:  # High-speed regime
        conditions = [
            {"H1": 2/3 * spinning_rate, "X": 1/3 * spinning_rate, "n": "2/3-1/3"},
            {"H1": 3/4 * spinning_rate, "X": 1/4 * spinning_rate, "n": "3/4-1/4"},
            {"H1": 1/3 * spinning_rate, "X": 2/3 * spinning_rate, "n": "1/3-2/3"},
        ]
    
    elif spinning_rate >= 15e3:  # Medium-speed regime
        for n in [-2, -1, 0, 1, 2]:
            conditions.append({
                "H1": calculate_h1_field(n, spinning_rate),
                "X": calculate_x_field(n, spinning_rate, x_nucleus),
                "n": f"n={n}"
            })
    
    else:  # Low-speed regime
        for n in range(-3, 4):
            conditions.append({
                "H1": calculate_h1_field(n, spinning_rate),
                "X": calculate_x_field(n, spinning_rate, x_nucleus),
                "n": f"n={n}"
            })
    
    # For each condition, perform 2D grid search
    results = []
    for cond in conditions:
        grid_result = opto_2d_grid_search(
            h1_center=cond["H1"],
            x_center=cond["X"],
            h1_range=0.2 * cond["H1"],  # ±20%
            x_range=0.2 * cond["X"],
            grid_size=5,
            scoring="symmetry_aware"
        )
        results.append({
            "condition": cond["n"],
            "optimized": grid_result,
            "efficiency": grid_result["score"]
        })
    
    # Return top 3 conditions for further exploration
    results.sort(key=lambda x: x["efficiency"], reverse=True)
    return results[:3]
```

#### Step 4: X-Nucleus Calibration
```python
def calibrate_x_nucleus(cp_optimized, x_nucleus):
    """
    Calibrate X-nucleus pulse width using CP signal
    """
    # Start from theoretical prediction based on H1 power
    theoretical_pw = calculate_theoretical_pw(h1_pw90, x_nucleus)
    
    # Use CP to generate signal
    array = np.linspace(theoretical_pw * 0.7, theoretical_pw * 1.3, 7)
    
    # Acquire nutation or direct excitation after CP
    signal = acquire_x_nutation_via_cp(array, cp_optimized)
    
    # Fit and extract
    x_pw90 = fit_nutation_curve(signal)
    
    # Validate against theoretical
    expected_ratio = get_gyromagnetic_ratio(x_nucleus) / get_gyromagnetic_ratio("1H")
    actual_ratio = x_pw90 / h1_pw90
    
    if abs(actual_ratio - expected_ratio) / expected_ratio < 0.30:
        log_info(f"{x_nucleus} pulse width: {x_pw90:.2f} μs ✓")
        return x_pw90, "PASS"
    else:
        log_warning(f"{x_nucleus} pulse width differs from expected ratio")
        return x_pw90, "WARNING"
```

#### Step 5: Fine CP Optimization
```python
def fine_tune_cp(initial_conditions, h1_pw90, x_pw90):
    """
    Refine CP with known pulse widths and explore ramp parameters
    """
    results = []
    
    for cond in initial_conditions:
        # For each ramp configuration
        for ramp_type in ["up", "down", "from", "to"]:
            optimized = opto_refine_cp(
                initial_condition=cond,
                h1_pw90=h1_pw90,
                x_pw90=x_pw90,
                ramp_type=ramp_type,
                ramp_fraction=0.15,  # 15% ramp range
                n_steps=20
            )
            
            results.append({
                "condition": cond["condition"],
                "ramp": ramp_type,
                "efficiency": optimized["efficiency"],
                "parameters": optimized
            })
    
    # Find best overall
    best = max(results, key=lambda x: x["efficiency"])
    
    # Update database with successful condition
    if best["efficiency"] > 0.60:  # 60% threshold
        update_database_defaults(
            spinning_rate=current_spinning_rate,
            field=current_field,
            condition=best
        )
    
    return best
```

#### Step 6: Validation & Reporting
```python
def validate_calibrations(calibrations, database):
    """
    Compare all parameters to theoretical models and database
    """
    report = {
        "timestamp": datetime.now(),
        "warnings": [],
        "alerts": [],
        "pass": True
    }
    
    # Check H1 pulse width
    h1_stats = database.get_statistics("H1_pw90", probe=current_probe)
    if calibrations["H1_pw90"] > h1_stats["mean"] + 2*h1_stats["std"]:
        report["warnings"].append(
            "¹H pulse width is 2σ above average - check tuning"
        )
    
    # Check CP efficiency
    if calibrations["CP_efficiency"] < 0.30:
        report["alerts"].append(
            "CP efficiency <30% - possible mismatch or pulse width error"
        )
        report["pass"] = False
    
    # Check consistency between nuclei
    expected_ratio = GYROMAGNETIC_RATIOS["13C"] / GYROMAGNETIC_RATIOS["1H"]
    actual_ratio = calibrations["C13_pw90"] / calibrations["H1_pw90"]
    
    if abs(actual_ratio - expected_ratio) / expected_ratio > 0.40:
        report["warnings"].append(
            "Pulse width ratio inconsistent - check RF calibration"
        )
    
    # Generate QC report
    generate_qc_report(calibrations, report)
    
    return report
```

---

## Warning System

### Severity Levels & Actions

#### 🟢 Info (No Action Required)
- **Trigger**: Within ±20% of expected
- **Message**: "Parameter within normal range"
- **Action**: Log to database, continue

#### 🟡 Warning (Attention Recommended)
- **Trigger**: 20-50% deviation from expected
- **Message**: "Parameter deviation detected"
- **Details**: 
  - "May indicate tuning drift"
  - "Could be sample property effect (e.g., high salt)"
  - "Consider re-checking probe match"
- **Action**: Continue but flag in report

#### 🟠 Alert (Action Recommended)
- **Trigger**: 50-100% deviation from expected
- **Message**: "Significant deviation - likely problem"
- **Details**:
  - "Check probe tuning and matching"
  - "Verify cable connections"
  - "Check amplifier output"
  - "Inspect sample for conductivity issues"
- **Action**: Pause workflow, require user acknowledgment

#### 🔴 Stop (Action Required)
- **Trigger**: 
  - >100% deviation from expected
  - Calibration failure (no signal, unstable)
  - Dangerous power levels (risk of probe damage)
- **Message**: "STOP - Intervention required"
- **Details**: Specific diagnosis based on failure mode
- **Action**: Stop all automation, require troubleshooting

### Common Warning Scenarios

#### Pulse Width Too Short
```
🟠 ALERT: ¹H pulse width = 1.2 μs (expected ~2.5 μs)

Possible causes:
1. Over-coupling on probe
2. Cable/connector problem (high resistance)
3. Amplifier gain too high
4. Sample causing field enhancement (unlikely)

Recommended actions:
1. Check probe tuning/matching
2. Verify cable integrity
3. Reduce amplifier gain if recently changed

Continue anyway? [Yes] [No] [Troubleshoot]
```

#### Pulse Width Too Long
```
🟠 ALERT: ¹³C pulse width = 8.5 μs (expected ~4.0 μs)

Possible causes:
1. Under-coupling on probe
2. Amplifier not delivering full power
3. Cable loss
4. Conductive sample (high salt)

Recommended actions:
1. Re-tune probe for deeper dip
2. Check amplifier output power
3. Verify cable connections
4. If sample is very salty, this may be normal

Continue anyway? [Yes] [No] [Troubleshoot]
```

#### Low CP Efficiency
```
🔴 STOP: CP efficiency = 18% (minimum threshold 30%)

Possible causes:
1. Hartmann-Hahn mismatch (incorrect spinning rate?)
2. Incorrect pulse widths used in CP
3. Poor T1ρ (mismatched decoupling)
4. Probe tuning shifted during experiment

Recommended actions:
1. Verify spinning rate is stable
2. Re-verify ¹H and ¹³C pulse widths
3. Check probe tuning before CP optimization
4. Try different CP match condition

[Re-run Calibrations] [Try Alternative Condition] [Expert Mode]
```

#### High Variability
```
🟡 WARNING: Repeated measurements show high variability

¹H pulse width: 2.4 ± 0.3 μs (12% RSD)

Possible causes:
1. Temperature not stable
2. Spinning rate fluctuating
3. Probe tuning drifting
4. Sample heating/cooling during experiment

Recommended actions:
1. Allow more temperature stabilization time
2. Check bearing air pressure
3. Wait 5 minutes and retry
4. Consider reducing RF power

[Retry] [Continue] [Abort]
```

---

## Data Mining Strategy

### Retrospective Analysis

#### Phase 1: Existing Opto Logs (Last 2 Years)

**Goal**: Extract patterns from successful optimizations

**Data Sources**:
- OPTO log files in: `/home/chad/opto_logs/`
- NMR data directories: `/opt/nmrdata/*/opto_*/`
- Spectrometer logs: `vnmrsys/data/studies/*/opto*/`

**Extraction Script**:
```python
import glob
import pandas as pd
import re
from pathlib import Path

def mine_opto_logs(log_directory):
    """
    Extract optimization results from historical OPTO logs
    """
    results = []
    
    for log_file in glob.glob(f"{log_directory}/**/*.log", recursive=True):
        try:
            data = parse_opto_log(log_file)
            
            # Extract key parameters
            entry = {
                "date": data["timestamp"],
                "probe": data["probe_id"],
                "spinning_rate": data["spinning_rate_hz"],
                "field_strength": data["b0_mhz"],
                "nucleus_x": data["x_nucleus"],
                "h1_pw90": data["h1_pulse_width_us"],
                "x_pw90": data["x_pulse_width_us"],
                "cp_condition": data["cp_match_condition"],
                "cp_h1_field": data["cp_h1_field_hz"],
                "cp_x_field": data["cp_x_field_hz"],
                "cp_efficiency": data["cp_efficiency_percent"],
                "ramp_type": data["ramp_configuration"],
                "contact_time": data["contact_time_ms"],
                "score": data["final_score"]
            }
            
            results.append(entry)
        
        except Exception as e:
            print(f"Failed to parse {log_file}: {e}")
            continue
    
    df = pd.DataFrame(results)
    return df

# Execute mining
historical_data = mine_opto_logs("/home/chad/opto_logs/")
historical_data.to_csv("opto_historical_database.csv")
```

**Analysis Goals**:
1. **Clustering by spinning rate**: Group into low/medium/high speed regimes
2. **CP condition preferences**: Which match conditions are most commonly successful?
3. **Ramp behavior**: Do certain ramp types consistently outperform?
4. **Probe-specific trends**: Are there probe-specific optimal conditions?
5. **Failure modes**: What conditions led to poor results?

#### Phase 2: NMR Data Directory Mining

**Goal**: Find completed experiments with parameter files

```python
def scan_nmr_directories(base_path="/opt/nmrdata/"):
    """
    Scan NMR data directories for completed OPTO studies
    """
    opto_dirs = []
    
    # Find all directories matching OPTO naming pattern
    pattern = "**/opto_*"
    
    for path in Path(base_path).glob(pattern):
        if path.is_dir():
            # Check for completion markers
            if (path / "procpar").exists() and (path / "fid").exists():
                opto_dirs.append(path)
    
    print(f"Found {len(opto_dirs)} completed OPTO studies")
    
    # Extract parameters from each
    database_entries = []
    for opto_dir in opto_dirs:
        params = parse_vnmr_procpar(opto_dir / "procpar")
        database_entries.append(extract_opto_params(params))
    
    return pd.DataFrame(database_entries)
```

**Metadata Extraction**:
- Experiment date/time
- User (if available)
- Sample name/description (if available)
- All acquisition parameters
- Final optimized values
- Processing results

#### Phase 3: Statistical Modeling

**Build Predictive Models**:

```python
import numpy as np
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split

def build_cp_predictor(historical_data):
    """
    Train ML model to predict optimal CP conditions
    """
    # Features
    X = historical_data[[
        "spinning_rate",
        "field_strength", 
        "h1_pw90",
        "x_pw90"
    ]].values
    
    # Targets
    y_h1 = historical_data["cp_h1_field"].values
    y_x = historical_data["cp_x_field"].values
    y_eff = historical_data["cp_efficiency"].values
    
    # Train models
    model_h1 = RandomForestRegressor(n_estimators=100)
    model_x = RandomForestRegressor(n_estimators=100)
    model_eff = RandomForestRegressor(n_estimators=100)
    
    model_h1.fit(X, y_h1)
    model_x.fit(X, y_x)
    model_eff.fit(X, y_eff)
    
    return {
        "h1_field_predictor": model_h1,
        "x_field_predictor": model_x,
        "efficiency_predictor": model_eff
    }

# Use for predictions
predictors = build_cp_predictor(historical_data)

def predict_cp_conditions(spinning_rate, field, h1_pw, x_pw):
    """
    Predict optimal CP starting point
    """
    features = np.array([[spinning_rate, field, h1_pw, x_pw]])
    
    predicted_h1 = predictors["h1_field_predictor"].predict(features)[0]
    predicted_x = predictors["x_field_predictor"].predict(features)[0]
    predicted_eff = predictors["efficiency_predictor"].predict(features)[0]
    
    return {
        "h1_field": predicted_h1,
        "x_field": predicted_x,
        "expected_efficiency": predicted_eff
    }
```

### Prospective Database

**Schema Design**:

```sql
CREATE TABLE calibration_runs (
    run_id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME NOT NULL,
    user TEXT,
    probe_id TEXT NOT NULL,
    probe_config TEXT,  -- e.g., "1.6mm HXY"
    sample_id TEXT,
    sample_description TEXT,
    
    -- Experimental conditions
    spinning_rate_hz REAL NOT NULL,
    spinning_stability_hz REAL,
    temperature_k REAL NOT NULL,
    temperature_stability_k REAL,
    b0_field_mhz REAL NOT NULL,
    
    -- Calibration results
    h1_pw90_us REAL,
    c13_pw90_us REAL,
    n15_pw90_us REAL,
    
    -- CP optimization
    cp_condition TEXT,  -- e.g., "n=1", "2/3-1/3"
    cp_h1_field_hz REAL,
    cp_x_field_hz REAL,
    cp_ramp_type TEXT,  -- "up", "down", "from", "to"
    cp_contact_time_ms REAL,
    cp_efficiency_percent REAL,
    
    -- Quality metrics
    h1_linewidth_hz REAL,
    signal_to_noise REAL,
    
    -- Validation flags
    validation_status TEXT,  -- "PASS", "WARNING", "ALERT", "FAIL"
    warnings TEXT,  -- JSON list of warning messages
    
    -- Notes
    notes TEXT,
    
    FOREIGN KEY(probe_id) REFERENCES probes(probe_id)
);

CREATE TABLE probes (
    probe_id TEXT PRIMARY KEY,
    probe_type TEXT,  -- "1.6mm", "3.2mm", etc.
    nuclei TEXT,  -- "HXY", "HX", etc.
    manufacturer TEXT,
    serial_number TEXT,
    install_date DATE,
    last_service_date DATE,
    notes TEXT
);

CREATE TABLE warnings (
    warning_id INTEGER PRIMARY KEY AUTOINCREMENT,
    run_id INTEGER,
    warning_type TEXT,  -- "INFO", "WARNING", "ALERT", "STOP"
    parameter TEXT,  -- "H1_pw90", "CP_efficiency", etc.
    expected_value REAL,
    actual_value REAL,
    deviation_percent REAL,
    message TEXT,
    timestamp DATETIME,
    
    FOREIGN KEY(run_id) REFERENCES calibration_runs(run_id)
);

CREATE INDEX idx_probe_date ON calibration_runs(probe_id, timestamp);
CREATE INDEX idx_spinning_rate ON calibration_runs(spinning_rate_hz);
CREATE INDEX idx_validation ON calibration_runs(validation_status);
```

**Automatic Logging**:
```python
def log_calibration_to_database(calibrations, validation_report):
    """
    Automatically log every calibration run to database
    """
    import sqlite3
    
    conn = sqlite3.connect("/home/chad/opto_database/calibrations.db")
    cursor = conn.cursor()
    
    # Insert main calibration record
    cursor.execute("""
        INSERT INTO calibration_runs (
            timestamp, user, probe_id, probe_config,
            spinning_rate_hz, temperature_k, b0_field_mhz,
            h1_pw90_us, c13_pw90_us, n15_pw90_us,
            cp_condition, cp_h1_field_hz, cp_x_field_hz,
            cp_ramp_type, cp_contact_time_ms, cp_efficiency_percent,
            validation_status, warnings
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    """, (
        calibrations["timestamp"],
        calibrations["user"],
        calibrations["probe_id"],
        calibrations["probe_config"],
        calibrations["spinning_rate"],
        calibrations["temperature"],
        calibrations["field"],
        calibrations["H1_pw90"],
        calibrations["C13_pw90"],
        calibrations["N15_pw90"],
        calibrations["cp_condition"],
        calibrations["cp_h1_field"],
        calibrations["cp_x_field"],
        calibrations["cp_ramp_type"],
        calibrations["cp_contact_time"],
        calibrations["cp_efficiency"],
        validation_report["pass"],
        json.dumps(validation_report["warnings"])
    ))
    
    run_id = cursor.lastrowid
    
    # Insert any warnings
    for warning in validation_report["warnings"] + validation_report["alerts"]:
        cursor.execute("""
            INSERT INTO warnings (
                run_id, warning_type, parameter,
                expected_value, actual_value, deviation_percent,
                message, timestamp
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            run_id,
            warning["type"],
            warning["parameter"],
            warning["expected"],
            warning["actual"],
            warning["deviation"],
            warning["message"],
            datetime.now()
        ))
    
    conn.commit()
    conn.close()
```

---

## Implementation Roadmap

### Phase 1: Core Functionality (Months 1-6)

**Deliverables**:
1. ✓ Basic dashboard with 4 main panels
2. ✓ Automated workflow engine
3. ✓ Warning system implementation
4. ✓ VnmrJ/OBJ integration
5. ✓ SQLite database setup

**Milestones**:
- **Month 1-2**: Dashboard UI design and implementation
  - HTML/CSS/JavaScript front-end
  - Flask/FastAPI backend
  - WebSocket for real-time updates
  
- **Month 3-4**: Workflow engine development
  - Python automation scripts
  - OPTO integration
  - VnmrJ macro interface
  
- **Month 5-6**: Testing and refinement
  - Beta testing with NMRFAM users
  - Bug fixes and optimization
  - Documentation (Quick Start Guide)

### Phase 2: Intelligence Layer (Months 6-12)

**Deliverables**:
1. ✓ Historical data mining pipeline
2. ✓ Database-driven default suggestions
3. ✓ CP condition explorer with branching
4. ✓ Statistical anomaly detection
5. ✓ Automated reporting

**Milestones**:
- **Month 7-8**: Data mining implementation
  - Parse historical OPTO logs
  - Extract NMR directory metadata
  - Build initial database
  
- **Month 9-10**: Predictive modeling
  - Train ML models on historical data
  - Implement smart defaults
  - Cross-validation testing
  
- **Month 11-12**: Advanced features
  - Parallel CP optimization branches
  - Trend analysis and visualization
  - Comprehensive QC reports

### Phase 3: Advanced Capabilities (Months 12-18)

**Deliverables**:
1. ✓ TopSpin integration
2. ✓ Dual-channel ramping (hardware permitting)
3. ✓ Machine learning for parameter prediction
4. ✓ Multi-user collaboration features
5. ✓ Cloud synchronization (optional)

**Milestones**:
- **Month 13-14**: TopSpin bridge development
  - Python-TopSpin API interface
  - Test on Bruker systems
  - Unified parameter format
  
- **Month 15-16**: Hardware enhancements
  - Dual-channel ramp firmware (if available)
  - Advanced pulse sequences
  - Real-time optimization feedback
  
- **Month 17-18**: Deployment and scaling
  - Multi-site installation (NMRFAM, Resynant, collaborators)
  - User training programs
  - Ongoing support infrastructure

---

## Quick Start Guide Structure

### 1. Installation & Setup (5 minutes)

**Prerequisites**:
- Python 3.8+
- VnmrJ 4.2+ or TopSpin 4.0+
- Network access to spectrometer

**Installation**:
```bash
# Clone repository
git clone https://github.com/nmrfam/opto-frontend.git
cd opto-frontend

# Install dependencies
pip install -r requirements.txt

# Configure spectrometer connection
python setup.py configure

# Initialize database
python setup.py init-db

# Launch dashboard
python app.py
```

**Browser**: Navigate to `http://localhost:5000`

---

### 2. First Run Tutorial (15 minutes)

**Step-by-Step Walkthrough with Test Sample**:

#### Scenario: Calibrating on microcrystalline ubiquitin

**2.1 Load Sample**
- Insert rotor with ubiquitin sample
- Spin up to 20 kHz
- Wait for stabilization (status indicator turns green)

**2.2 Initial Tuning**
- Dashboard will prompt: "Probe tuning required"
- Perform manual tuning on spectrometer
- Enter dip frequencies:
  - ¹H: 600.13 MHz, -45 dB
  - ¹³C: 150.90 MHz, -42 dB
  - ¹⁵N: 60.82 MHz, -38 dB
- Click "Confirm Tuning"

**2.3 Start Automated Workflow**
- Click "Start Calibration Workflow"
- Dashboard shows progress:
  ```
  ⏳ ¹H pulse width calibration (2 min)
  ⏳ Initial CP optimization (8 min)
  ⏳ ¹³C pulse width calibration (5 min)
  ⏳ CP fine-tuning (12 min)
  ```

**2.4 Monitor Progress**
- Live data displays as each step completes
- Click any acquisition to view spectrum
- Green checkmarks indicate successful steps

**2.5 Review Results**
```
✓ ¹H pulse width: 2.45 μs (within expected range)
✓ ¹³C pulse width: 3.82 μs (within expected range)
✓ CP efficiency: 72% (excellent)
✓ Optimal CP condition: 2/3-1/3 match, ramp down
```

**2.6 Export to Spectrometer**
- Click "Export Parameters"
- Parameters automatically loaded into VnmrJ/TopSpin
- Ready to start experiments!

---

### 3. Standard Operation

**Decision Tree Flowchart**:

```
┌─────────────────────────────────────────┐
│         New Sample Loaded?               │
└────────────┬────────────────────────────┘
             │
             ↓
        [Same Probe?]
        ├── Yes ──→ [Recent calibration (<4 hours)?]
        │           ├── Yes ──→ [Use previous, skip to experiments]
        │           └── No ───→ [Quick recalibration (10 min)]
        │
        └── No ───→ [Full calibration required (35 min)]
                    │
                    ↓
              [Probe tuned?]
              ├── Yes ──→ [Start workflow]
              └── No ───→ [Tune probe first]
                          │
                          ↓
                    [Temperature stable?]
                    ├── Yes ──→ [Proceed]
                    └── No ───→ [Wait for stabilization]
                                │
                                ↓
                          [Run automated workflow]
                                │
                                ↓
                          [Review validation report]
                          ├── PASS ──→ [Export & start experiments]
                          ├── WARNING ─→ [Check suggestions, proceed?]
                          └── FAIL ───→ [Troubleshoot issues]
```

---

### 4. Troubleshooting

**Common Issues & Solutions**:

#### Issue: "No signal detected"
**Symptoms**: 
- Zero or very weak signal during ¹H calibration
- All acquisition attempts fail

**Diagnosis**:
1. Check spinning - is rotor actually spinning?
2. Check tuning - probe matched on correct frequency?
3. Check receiver gain - set too low?
4. Check sample - is there actually sample in rotor?

**Solutions**:
```
1. Verify spinning status:
   → Go to spectrometer, check LED/display
   → Ensure bearing and drive air pressures correct
   
2. Re-tune probe:
   → Use wobble curve
   → Ensure dip is at correct frequency
   → Achieve at least -30 dB match
   
3. Increase receiver gain:
   → Start with high gain
   → Reduce if saturation occurs
   
4. Verify sample:
   → Remove rotor, inspect under microscope
   → Ensure sample is packed correctly
```

---

#### Issue: "Pulse width differs by >50%"
**Symptoms**:
- 🟠 Orange alert on calibration
- Measured pulse width far from expected

**Diagnosis**:
- Compare to database average
- Check probe tuning depth
- Inspect warning message details

**Solutions**:
```
If pulse width too SHORT:
→ Probe over-coupled (dip too deep)
→ Solution: Re-tune for shallower dip

If pulse width too LONG:
→ Probe under-coupled OR power issue
→ Solution: Re-tune for deeper dip
→ If still long, check amplifier output
→ Check cable connections
```

---

#### Issue: "CP efficiency <30%"
**Symptoms**:
- 🔴 Red stop on CP optimization
- Very weak cross-polarization signal

**Diagnosis**:
1. Check spinning stability
2. Verify pulse widths are correct
3. Confirm match condition is appropriate
4. Check sample has both nuclei

**Solutions**:
```
1. Verify spinning rate:
   → Should be stable within ±2 Hz
   → If unstable, check bearing gas pressure
   
2. Re-calibrate pulse widths:
   → ¹H pulse width first
   → Then X-nucleus via direct excitation
   
3. Try different match condition:
   → If n=1 fails, try n=2
   → If 2/3-1/3 fails, try 1/3-2/3
   
4. Check sample:
   → Natural abundance? (need longer CP time)
   → Labeled? (should work well)
   → Wet? (water can interfere with CP)
```

---

### 5. Advanced Features

**Customization Options**:

#### Custom CP Conditions
```python
# Define your own match conditions
custom_conditions = [
    {"H1": 0.55 * spinning_rate, "X": 0.45 * spinning_rate},
    {"H1": 0.60 * spinning_rate, "X": 0.40 * spinning_rate},
]

opto_frontend.add_custom_conditions(custom_conditions)
```

#### Batch Processing
```python
# Run calibrations on multiple samples sequentially
samples = ["ubiquitin", "GB1", "alpha_synuclein"]

for sample in samples:
    opto_frontend.run_workflow(
        sample_name=sample,
        auto_export=True,
        generate_report=True
    )
```

#### Parameter Constraints
```python
# Set bounds for optimization
opto_frontend.set_constraints(
    h1_power_max=50e3,  # Maximum 50 kHz ¹H field
    cp_contact_time_range=(0.5, 5.0),  # 0.5-5 ms
    safety_margin=0.9  # 90% of maximum safe power
)
```

---

### 6. Theory Appendix

**Why These Conditions Work**:

#### Hartmann-Hahn Matching
The CP transfer efficiency is maximized when the Hartmann-Hahn match condition is satisfied:

```
ω₁ᴴ ± ω₁ˣ = n·ωᵣ
```

Where:
- ω₁ᴴ = RF field strength on ¹H (in angular frequency)
- ω₁ˣ = RF field strength on X-nucleus (¹³C or ¹⁵N)
- n = sideband order (0, ±1, ±2, ...)
- ωᵣ = MAS spinning frequency (angular)

**Physical Interpretation**:
- At the match condition, the precession frequencies in the rotating frame are synchronized
- This allows efficient transfer of polarization from abundant ¹H to rare X-nuclei
- Sidebands (n ≠ 0) arise from rotational modulation of dipolar coupling

#### Spinning Rate Regimes

**High-Speed (>30 kHz)**:
- Sidebands are far apart
- n=0 (zero-quantum) condition becomes practical
- Low RF power requirements: 2/3-1/3 or 3/4-1/4 matches
- Advantages: Less sample heating, longer T₁ρ

**Medium-Speed (15-30 kHz)**:
- n=±1 matches are typical
- Requires moderate RF power (~ωᵣ on each channel)
- Good balance of efficiency and robustness

**Low-Speed (<15 kHz)**:
- Multiple sideband conditions accessible
- Higher RF power required
- Broader match manifold for optimization

#### Ramping Strategy

**Why ramp?**
- Match condition has finite width
- Ramping sweeps through optimum
- Compensates for RF inhomogeneity
- Averages over different crystallite orientations

**Ramp types**:
- **Up/Down**: Cross through match condition
- **From/To**: Start at match, ramp away (or vice versa)
- Optimal choice is empirically determined by OPTO

---

## API Documentation (For Developers)

### Python API

```python
from opto_frontend import OptoInterface

# Initialize interface
opto = OptoInterface(
    spectrometer="vnmrj",  # or "topspin"
    host="nmr1.chem.wisc.edu",
    port=5555
)

# Connect to spectrometer
opto.connect()

# Run full workflow
results = opto.run_calibration_workflow(
    probe="1.6mm_HXY",
    spinning_rate=20e3,  # 20 kHz
    temperature=298,  # K
    nuclei=["1H", "13C", "15N"],
    auto_export=True
)

# Access results
print(f"¹H pulse width: {results['H1_pw90']:.2f} μs")
print(f"CP efficiency: {results['CP_efficiency']:.1f}%")

# Get validation report
if results['validation']['pass']:
    print("✓ All calibrations passed validation")
else:
    print("⚠ Warnings detected:")
    for warning in results['validation']['warnings']:
        print(f"  - {warning}")

# Export to spectrometer
opto.export_parameters(results)
```

---

### REST API

**Endpoints**:

#### GET /api/status
Returns current workflow status
```json
{
  "status": "calibrating",
  "current_step": "H1_nutation",
  "progress": 0.45,
  "estimated_time_remaining": 180
}
```

#### POST /api/workflow/start
Start automated calibration workflow
```json
{
  "probe": "1.6mm_HXY",
  "spinning_rate": 20000,
  "temperature": 298,
  "nuclei": ["1H", "13C", "15N"]
}
```

#### GET /api/calibrations/recent
Get recent calibration history
```json
{
  "calibrations": [
    {
      "timestamp": "2025-11-09T14:32:15",
      "probe": "1.6mm_HXY",
      "H1_pw90": 2.45,
      "C13_pw90": 3.82,
      "CP_efficiency": 72.3,
      "validation": "PASS"
    },
    ...
  ]
}
```

#### GET /api/database/query
Query historical database
```json
{
  "query": {
    "probe": "1.6mm_HXY",
    "spinning_rate_min": 18000,
    "spinning_rate_max": 22000,
    "date_after": "2025-01-01"
  },
  "results": [...]
}
```

---

## Collaboration & Future Directions

### Integration with Other Systems

**Song Lin's Pulse Sequences**:
- Automatically export optimized parameters
- Pre-populate pulse programs with calibrated values
- Seamless transition from calibration to experiments

**BEFON Integration**:
- Use optimized CP conditions as input to BEFON simulations
- Validate theoretical predictions against experimental results
- Feedback loop: simulations guide optimization, experiments validate

**VFON Integration**:
- Replace NMRPipe functions with VFON calls
- Optimized processing of OPTO datasets
- GPU acceleration for faster optimization cycles

### Community Contributions

**Open Database**:
- Anonymous sharing of calibration data across labs
- Build collective wisdom on optimal conditions
- Privacy-preserving: sample details remain local

**Shared Protocols**:
- Repository of successful optimization strategies
- User-contributed pulse sequences
- Best practices documentation

### Commercial Potential (Resynant)

**Value Proposition**:
- Dramatically reduces setup time for new users
- Ensures optimal performance out of the box
- Differentiator vs. Bruker/Jeol (they lack this)

**Integration with Harmonizer**:
- Native support in Resynant consoles
- Pre-configured for common probes
- Automatic updates via cloud sync

**Training & Support**:
- Reduce customer support burden
- Self-service troubleshooting
- Remote monitoring capabilities

---

## Maintenance & Support

### Database Maintenance

**Regular Tasks**:
- Weekly backup of calibration database
- Monthly analysis of trends and anomalies
- Quarterly cleanup of orphaned records
- Annual archiving of old data

**Automated Jobs**:
```bash
# Daily backup
0 2 * * * /usr/local/bin/opto_backup.sh

# Weekly statistics update
0 3 * * 0 /usr/local/bin/opto_update_stats.py

# Monthly report generation
0 4 1 * * /usr/local/bin/opto_monthly_report.py
```

### Software Updates

**Version Control**:
- Git repository for all code
- Semantic versioning (MAJOR.MINOR.PATCH)
- Changelog documentation
- Automated testing before release

**Update Mechanism**:
```bash
# Check for updates
opto-frontend update check

# Install update
opto-frontend update install

# Rollback if needed
opto-frontend update rollback
```

---

## Conclusion

This comprehensive front-end interface for OPTO represents a significant advancement in making advanced NMR optimization accessible to a broader user base. By combining:

- **Intelligent automation** (context-aware defaults, workflow guidance)
- **Historical intelligence** (data mining, predictive models)
- **Safety systems** (warnings, validation, troubleshooting)
- **User-friendly design** (visual dashboard, clear feedback)

We can transform the calibration experience from an expert-only task into a routine procedure that consistently delivers optimal results.

The phased implementation approach ensures early value delivery while building toward the full vision over 18 months. The data-driven foundation creates a continuously improving system that learns from every calibration performed.

For NMRFAM, this reduces the training burden and ensures consistent high-quality results across all users. For Resynant, it provides a competitive advantage and reduces customer support requirements while enabling remote monitoring and assistance.

**Next Steps**:
1. Review and refine this document with key stakeholders
2. Prioritize Phase 1 deliverables
3. Begin UI mockup and workflow engine development
4. Start mining historical OPTO logs
5. Establish database schema and begin logging

---

## Appendix A: Technology Stack

**Front-End**:
- HTML5, CSS3, JavaScript
- React or Vue.js for reactive UI
- Chart.js or Plotly for visualizations
- WebSocket for real-time updates

**Back-End**:
- Python 3.8+
- Flask or FastAPI for REST API
- SQLite for local database
- pandas, numpy, scipy for data analysis
- scikit-learn for machine learning
- asyncio for concurrent operations

**Spectrometer Integration**:
- VnmrJ: tcsh/bash scripts, OBJ macros
- TopSpin: Python-TopSpin bridge
- SSH/SFTP for remote access

**Deployment**:
- Docker containers (optional)
- systemd for service management
- nginx for reverse proxy
- Let's Encrypt for HTTPS (if web-accessible)

---

## Appendix B: Sample Database Queries

**Find best CP condition for given spinning rate**:
```sql
SELECT 
    cp_condition,
    AVG(cp_efficiency_percent) as avg_efficiency,
    COUNT(*) as n_samples
FROM calibration_runs
WHERE 
    probe_id = '1.6mm_HXY'
    AND spinning_rate_hz BETWEEN 19000 AND 21000
    AND validation_status = 'PASS'
GROUP BY cp_condition
ORDER BY avg_efficiency DESC
LIMIT 3;
```

**Track probe performance over time**:
```sql
SELECT 
    DATE(timestamp) as date,
    AVG(h1_pw90_us) as avg_h1_pw,
    STDDEV(h1_pw90_us) as std_h1_pw,
    COUNT(*) as n_samples
FROM calibration_runs
WHERE 
    probe_id = '1.6mm_HXY'
    AND timestamp > DATE('now', '-90 days')
GROUP BY DATE(timestamp)
ORDER BY date;
```

**Identify anomalous calibrations**:
```sql
WITH stats AS (
    SELECT 
        AVG(cp_efficiency_percent) as mean_eff,
        STDDEV(cp_efficiency_percent) as std_eff
    FROM calibration_runs
    WHERE 
        probe_id = '1.6mm_HXY'
        AND spinning_rate_hz BETWEEN 19000 AND 21000
)
SELECT 
    c.run_id,
    c.timestamp,
    c.cp_efficiency_percent,
    ABS(c.cp_efficiency_percent - s.mean_eff) / s.std_eff as z_score
FROM calibration_runs c, stats s
WHERE 
    c.probe_id = '1.6mm_HXY'
    AND ABS(c.cp_efficiency_percent - s.mean_eff) > 2 * s.std_eff
ORDER BY z_score DESC;
```

---

## Document Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-11-09 | Chad Rienstra | Initial comprehensive design document |

---

**For questions or contributions, contact:**
- Chad Rienstra (chad.rienstra@wisc.edu)
- NMRFAM (www.nmrfam.wisc.edu)
- Resynant Inc. (www.resynant.com)
