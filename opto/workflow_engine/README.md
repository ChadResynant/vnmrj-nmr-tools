# OPTO Workflow Automation Engine

**Status**: Initial implementation complete, ready for testing
**Date**: November 9, 2025
**Version**: 0.1.0-alpha

## Overview

This module provides intelligent workflow orchestration for OPTO-based NMR calibration. It automates the complete calibration sequence from H1 pulse width through CP optimization with expert-guided failure recovery.

## Architecture

```
Python Workflow Engine
    ↓ (submits jobs via CLI)
OPTO Backend
    ↓ (writes log files)
Experiment Directory
    ↓ (monitored by)
Log Parser
    ↓ (results feed back to)
Workflow Orchestrator
```

**Key Principle**: OPTO CLI remains the primary interface. Python orchestrates job sequences and makes decisions based on parsed results.

## Module Structure

```
workflow_engine/
├── __init__.py                    # Package exports
├── opto_interface.py              # OPTO CLI wrapper (470 lines)
├── log_parser.py                  # Log file parser (355 lines)
├── workflow_orchestrator.py       # Calibration workflow (545 lines)
├── diagnostics.py                 # Hardware & CP diagnostics (523 lines)
└── README.md                      # This file
```

## Quick Start

### Basic Usage

```python
from opto.workflow_engine import WorkflowOrchestrator, WorkflowConfig

# Initialize orchestrator
workflow = WorkflowOrchestrator()

# Configure workflow
config = WorkflowConfig(
    nuclei=["1H", "13C", "15N"],
    spinning_rate=20000,  # Hz
    temperature=298,      # K
    probe_id="1.6mm_HXY"
)

# Run complete calibration
results = workflow.run_calibration(config)

# Check results
print(f"Status: {results['validation']['status']}")
print(f"H1 pw90: {results['calibrations']['H1_pw90']['value']:.2f} μs")
print(f"CP efficiency: {results['calibrations']['CP_final']['efficiency']:.1f}%")
```

### Monitoring OPTO Jobs

```python
from opto.workflow_engine import OptoInterface

# Initialize interface
opto = OptoInterface(
    opto_cli_path="/path/to/opto",
    log_dir="./opto_logs"
)

# Submit single job
from opto.workflow_engine.opto_interface import OptoJobConfig

job = OptoJobConfig(
    job_type="nutation",
    nucleus="1H",
    param1_name="pulse_width",
    param1_min=2.0,
    param1_max=3.0,
    param1_steps=7
)

result = opto.submit_job(job, wait_for_completion=True)
print(f"Optimized pw90: {result.optimized_params['pulse_width']:.2f} μs")
```

## Implementation Status

### ✅ Completed

- **OPTO Interface** (`opto_interface.py`)
  - Job submission via CLI
  - Log file monitoring
  - Result parsing
  - Active job tracking

- **Log Parser** (`log_parser.py`)
  - Structured log parsing
  - Partial result extraction (for running jobs)
  - Stripchart format export
  - Real-time monitoring support

- **Workflow Orchestrator** (`workflow_orchestrator.py`)
  - H1 pulse width calibration
  - Spinning-rate-aware CP optimization
  - X-nucleus calibration via CP
  - CP fine-tuning with ramp exploration
  - Automatic failure recovery
  - Sequential decision logic

- **Diagnostics** (`diagnostics.py`)
  - Pulse width validation
  - Pulse width ratio checks (gyromagnetic ratios)
  - Variability detection (RSD thresholds)
  - CP pathology detection
  - Multi-level validation (Green/Yellow/Orange/Red)
  - Hardware diagnostic recommendations

### 🚧 Needs Testing/Refinement

1. **OPTO CLI Command Syntax** (`opto_interface.py:_build_opto_command`)
   - Currently placeholder implementation
   - **Action Required**: Update with actual OPTO CLI syntax
   - Location: Lines 230-275 in `opto_interface.py`

2. **Log File Format Parsing** (`log_parser.py:_extract_iterations`)
   - Simplified parser based on assumed format
   - **Action Required**: Adjust regex patterns for actual OPTO log format
   - Location: Lines 185-250 in `log_parser.py`

3. **Database Integration** (currently placeholders)
   - `_get_h1_initial_guess()` - needs database query
   - `_get_expected_pulse_width()` - needs historical data
   - **Action Required**: Implement SQLite database queries

## Testing Plan

### Phase 1: Component Testing (Tomorrow Morning)

**Test 1: OPTO CLI Interface**
```bash
cd /Users/crienstra/vnmrj-nmr-tools
python3 -c "
from opto.workflow_engine import OptoInterface
opto = OptoInterface()
print('Interface initialized successfully')
"
```

**Test 2: Log Parser**
```python
# Test with actual OPTO log file
from opto.workflow_engine.log_parser import OptoLogParser

parser = OptoLogParser()
result = parser.parse_log('/path/to/actual/opto/log.txt')
print(result)
```

**Test 3: Diagnostics**
```python
from opto.workflow_engine.diagnostics import HardwareDiagnostics

hw = HardwareDiagnostics()
validation = hw.validate_pulse_width(
    nucleus="1H",
    measured_pw=2.5,
    probe_id="1.6mm_HXY"
)
print(validation)
```

### Phase 2: Integration Testing

**Test 4: Single OPTO Job Submission**
- Submit H1 nutation job via interface
- Monitor log file parsing
- Verify result extraction

**Test 5: Complete Workflow**
- Run full calibration on test sample
- Monitor each step
- Validate final results

### Phase 3: Validation

**Test 6: Compare with Manual OPTO**
- Run same optimization manually
- Compare automated vs manual results
- Verify parameter accuracy

**Test 7: Failure Recovery**
- Intentionally provide poor initial conditions
- Verify systematic exploration kicks in
- Confirm convergence to good result

## Known Issues & TODOs

### Critical (Blocks Testing)

1. **OPTO CLI Command Syntax Unknown**
   - File: `opto_interface.py`
   - Method: `_build_opto_command()`
   - Status: Placeholder implementation
   - **Action**: Get actual OPTO CLI help/documentation

2. **Log File Format Unknown**
   - File: `log_parser.py`
   - Method: `_extract_iterations()`
   - Status: Assumed format based on typical OPTO output
   - **Action**: Parse actual OPTO log files to determine format

### Important (Affects Functionality)

3. **Database Not Implemented**
   - Missing: Historical calibration lookup
   - Impact: No intelligent initial guesses
   - Workaround: Using hardcoded typical values
   - **Action**: Implement SQLite database (Phase 2)

4. **No Job File Template System**
   - Missing: Pre-configured OPTO job templates
   - Impact: All parameters specified programmatically
   - **Action**: Create job file templates in `opto/examples/job-files/`

### Nice to Have (Future Enhancements)

5. **Real-time Progress Updates**
   - Current: Polling-based monitoring
   - Desired: WebSocket or event-driven updates
   - **Action**: Integrate with future web frontend

6. **Parallel Job Execution**
   - Current: Sequential job submission
   - Desired: Parallel exploration of multiple CP conditions
   - **Action**: Add concurrent job management

## Expert Knowledge Encoded

### CP Optimization Strategy

**High-speed regime (>30 kHz)**:
```python
# Try these match conditions in order
conditions = [
    (2/3 * ωr, 1/3 * ωr),  # Primary
    (3/4 * ωr, 1/4 * ωr),  # Alternative 1
    (1/3 * ωr, 2/3 * ωr),  # Alternative 2 (reversed)
]
```

**Medium-speed regime (15-30 kHz)**:
```python
# Explore n = ±1, ±2 sideband matches
for n in [1, -1, 2, -2]:
    h1_field = spinning_rate + n * offset
    x_field = spinning_rate
```

**Low-speed regime (<15 kHz)**:
```python
# Broader exploration
for n in range(-3, 4):
    # Calculate match conditions
```

### Hardware Diagnostic Thresholds

**Pulse Width Validation**:
- Green: ±20% of expected
- Yellow: 20-50% deviation
- Orange: 50-100% deviation
- Red: >100% deviation

**Pulse Width Ratios** (expected):
- C13/H1 ≈ 4.0 (±40% tolerance)
- N15/H1 ≈ 10.0 (±40% tolerance)
- P31/H1 ≈ 2.5 (±40% tolerance)

**Variability Thresholds**:
- Pulse widths: RSD <10%
- CP efficiency: RSD <15%

### Failure Recovery

**CP Efficiency <30%**:
1. Systematically explore all theoretical match conditions
2. If all fail, re-verify pulse widths
3. If still failing, flag for manual intervention

**High Variability Detected**:
1. Wait for temperature stabilization
2. Check spinning rate stability (±2 Hz required)
3. Verify bearing air pressure
4. Reduce RF power if sample heating suspected

## Configuration

### Environment Variables

```bash
# Optional: Override default paths
export OPTO_CLI_PATH=/usr/local/bin/opto
export OPTO_LOG_DIR=~/opto_logs
export OPTO_DB_PATH=~/opto_database/calibrations.db
```

### Python API Configuration

```python
from opto.workflow_engine import OptoInterface

# Custom paths
opto = OptoInterface(
    opto_cli_path="/custom/path/to/opto",
    experiment_dir="~/vnmrsys/exp1",
    log_dir="./my_opto_logs",
    polling_interval=2.0  # seconds
)
```

## Dependencies

**Required**:
- Python 3.8+
- NumPy (for data analysis)
- SciPy (future: for statistical analysis)

**Optional**:
- Matplotlib (for plotting, if visualization added)
- Pandas (if database analysis expanded)

**Install**:
```bash
pip3 install numpy scipy
```

## Debugging

### Enable Verbose Logging

```python
import logging
logging.basicConfig(level=logging.DEBUG)

from opto.workflow_engine import WorkflowOrchestrator
workflow = WorkflowOrchestrator()
# Now see detailed debug output
```

### Inspect Job Results

```python
result = opto.submit_job(job_config, wait_for_completion=True)

# Check result details
print(f"Status: {result.status}")
print(f"Score: {result.score}")
print(f"Optimized params: {result.optimized_params}")
print(f"Warnings: {result.warnings}")
print(f"Log file: {result.log_file_path}")
```

### Manual Log Parsing

```python
from opto.workflow_engine.log_parser import OptoLogParser

parser = OptoLogParser()
data = parser.parse_log('/path/to/log.txt')

# Inspect parsed data
for key, value in data.items():
    print(f"{key}: {value}")
```

## Next Session Checklist

### Before Testing

- [ ] Verify Python dependencies installed
- [ ] Locate OPTO CLI executable
- [ ] Find example OPTO log files
- [ ] Identify test sample for calibration

### During First Test

- [ ] Run component tests (Phase 1)
- [ ] Update OPTO CLI command syntax
- [ ] Adjust log parser for actual format
- [ ] Test single job submission

### After Initial Success

- [ ] Run complete workflow on test sample
- [ ] Document any issues encountered
- [ ] Refine based on real OPTO behavior
- [ ] Update this README with findings

## Support & Documentation

**Related Documentation**:
- Design spec: `../docs/opto_frontend_design.md`
- Reference parameters: `../examples/parameters/schanda_cp_parameters.md`
- Main repo README: `/Users/crienstra/vnmrj-nmr-tools/README.md`

**Contact**:
- Primary: Chad Rienstra (chad.rienstra@wisc.edu)
- NMRFAM: Barry Dezonia, Thiru

---

**Last Updated**: November 9, 2025
**Status**: Ready for testing on campus
**Next Steps**: Push to GitHub, test with actual OPTO CLI
