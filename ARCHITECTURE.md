# Architecture Overview - VNMRJ NMR Tools

Technical architecture documentation for the VNMRJ NMR Tools repository.

## System Overview

This repository provides a collection of tools for NMR data acquisition, processing, and analysis across multiple spectrometer systems. The tools are designed to work with OpenVNMRJ/VNMRJ and integrate into existing NMR workflows.

## Repository Structure

```
vnmrj-nmr-tools/
├── macros/                    # OpenVNMRJ MAGICAL macros
│   ├── opto/                  # OPTO workflow automation
│   ├── acquisition/           # Acquisition utilities
│   └── processing/            # Processing macros
│
├── python/                    # Python analysis tools
│   ├── conversion/            # Format conversion (FID, NMRPipe, etc.)
│   ├── analysis/              # Data analysis scripts
│   └── utilities/             # File management utilities
│
├── pulsesequences/            # Custom pulse sequences (.c files)
│
├── docs/                      # Documentation
│   ├── ARCHITECTURE.md        # This file
│   ├── INSTALLATION.md        # Installation guide
│   └── CONTRIBUTING.md        # Contribution guidelines
│
├── examples/                  # Example workflows and data
│
├── .gitignore                 # Git ignore rules
├── LICENSE                    # MIT License
└── README.md                  # Main documentation
```

## Component Architecture

### 1. OPTO Workflow System

**Purpose**: Automation of optimal parameter determination

**Components**:
- `gosopto` - Job submission macro
- `opto_fidarea_measure` - FID integral measurement
- Job templates (to be added)
- Server communication layer

**Workflow**:
```
User Setup
    ↓
gosopto (submit job)
    ↓
OPTO Server (optimization)
    ↓
opto_fidarea_measure (feedback)
    ↓
Results & Parameters
```

**Dependencies**:
- OpenVNMRJ running
- OPTO server accessible
- Network connectivity

### 2. Acquisition Utilities

**Purpose**: Enhanced experiment setup and execution

**Components**:
- `mydsa` - Custom array setup (dsa replacement)
- `gos-default` - Enhanced go-and-save with features:
  - Temperature monitoring
  - Automatic experiment saving
  - NUS schedule handling
  - Pulse sequence archiving

**Integration Points**:
- OpenVNMRJ parameter system
- Experiment directories (`exp*/`)
- Pulse sequence files

### 3. Python Processing Layer

**Purpose**: Data analysis, conversion, and file management

**Architecture**:
```
Python Scripts
    ├── Conversion Layer (FID formats)
    ├── Analysis Layer (spectral analysis)
    └── Utility Layer (file operations)
```

**Key Scripts**:
- `organize_nmr_data.py` - File organization and archiving
- `supercleanup.py` - Workspace cleanup and management

**Data Flow**:
```
Raw NMR Data (*.fid)
    ↓
Conversion Scripts
    ↓
Analysis Scripts
    ↓
Results/Reports
```

### 4. Pulse Sequence Library

**Purpose**: Custom pulse sequences for specialized experiments

**Structure**:
```c
// Standard OpenVNMRJ pulse sequence structure
#include <standard.h>

pulsesequence() {
    // Parameter setup
    // Pulse sequence code
    // Acquisition
}
```

**Compilation**:
- Source: `~/vnmrsys/psglib/*.c`
- Compiled: Automatic via `seqgen()`
- Usage: Available in OpenVNMRJ experiments

## System Integration

### OpenVNMRJ Integration

```
OpenVNMRJ Core
    ├── Macro System
    │   └── vnmrj-nmr-tools macros
    │
    ├── Pulse Sequences
    │   └── Custom sequences
    │
    └── User Scripts
        └── Python tools
```

**Integration Points**:
1. `~/vnmrsys/maclib/` - Macro installation
2. `~/vnmrsys/psglib/` - Pulse sequence source
3. `~/vnmrsys/bin/` - Python script installation
4. `~/vnmrsys/data/` - Data processing

### Multi-System Architecture

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Ayrshire   │     │   Ribeye    │     │   Taurus    │
│  750 MHz    │     │   600 MHz   │     │   600 MHz   │
│  (NMRFAM)   │     │  (NMRFAM)   │     │  (NMRFAM)   │
└──────┬──────┘     └──────┬──────┘     └──────┬──────┘
       │                   │                    │
       └───────────────────┼────────────────────┘
                           │
                    ┌──────▼──────┐
                    │   Bernard   │
                    │   Gateway   │
                    └──────┬──────┘
                           │
       ┌───────────────────┼────────────────────┐
       │                   │                    │
┌──────▼──────┐     ┌──────▼──────┐     ┌──────▼──────┐
│    Fyfe     │     │   Fyddle    │     │    Eagle    │
│   400 MHz   │     │   600 MHz   │     │ Processing  │
│ (Resynant)  │     │ (Resynant)  │     │   (Home)    │
└─────────────┘     └─────────────┘     └─────────────┘
```

**Code Distribution**:
- Each system: Local copy of tools in `~/vnmrsys/`
- Centralized: GitHub repository for version control
- Updates: Pull from GitHub, redeploy locally

## Data Flow Architecture

### Typical Workflow

```
1. Experiment Design
        ↓
2. Parameter Setup (mydsa, custom macros)
        ↓
3. OPTO Optimization (optional)
        ↓
4. Data Acquisition (gos-default)
        ↓
5. Processing (OpenVNMRJ + custom macros)
        ↓
6. Analysis (Python scripts)
        ↓
7. Organization (organize_nmr_data.py)
        ↓
8. Archival/Backup
```

### File Locations

**During Acquisition**:
```
~/vnmrsys/exp*/
    ├── acqfil/         # Raw FID data
    ├── fid             # FID file
    ├── procpar         # Parameters
    └── log             # Acquisition log
```

**After Processing**:
```
~/vnmrsys/exp*/
    ├── *.fid           # Processed data
    ├── phasefile       # Phasing parameters
    └── [analysis outputs]
```

**After Organization**:
```
~/data/
    ├── [date]/
    │   ├── [sample]/
    │   │   └── [nucleus]/
    │   │       └── data.fid
```

## Technology Stack

### Programming Languages

1. **MAGICAL** (OpenVNMRJ macro language)
   - Macro scripting
   - Parameter manipulation
   - Experiment control

2. **C** (Pulse sequences)
   - Low-level pulse programming
   - Hardware control
   - Timing-critical operations

3. **Python 3.6+**
   - Data analysis
   - File operations
   - Format conversion
   - Reporting

4. **Bash** (Shell scripts)
   - Automation
   - System integration
   - Batch processing

### Dependencies

**Required**:
- OpenVNMRJ 1.1A+
- Python 3.6+
- NumPy
- SciPy

**Optional**:
- NMRPipe (for conversion)
- Matplotlib (for plotting)
- Pandas (for data manipulation)

## Performance Considerations

### Macro Execution
- Macros run in OpenVNMRJ process
- Minimal overhead
- Real-time parameter access

### Python Scripts
- Separate processes
- Can be CPU-intensive
- Use NumPy for performance

### File I/O
- FID files can be large (MB to GB)
- Use streaming for large files
- Consider storage locations (local vs. network)

## Security Architecture

### Access Control
```
System Level
    ├── User permissions (Unix)
    ├── File ownership
    └── Directory permissions

Network Level
    ├── SSH keys (for remote access)
    ├── Gateway servers (Bernard)
    └── Firewall rules
```

### Data Protection
- No passwords in code
- SSH key-based authentication
- Secure file permissions (700 for sensitive)
- Git excludes data files (.gitignore)

## Extensibility

### Adding New Components

**New Macro**:
1. Create in `macros/[category]/`
2. Follow documentation standards
3. Test on representative system
4. Commit to repository

**New Python Script**:
1. Create in `python/[category]/`
2. Add docstrings and argparse
3. Add to requirements if needed
4. Test and commit

**New Pulse Sequence**:
1. Develop and test on spectrometer
2. Add to `pulsesequences/`
3. Document parameters
4. Provide example usage

### Plugin Architecture

Tools are modular:
- Macros can call other macros
- Python scripts are standalone
- Pulse sequences are independent

No hard dependencies between components (except where noted).

## Testing Strategy

### Manual Testing
- Test on actual NMR systems
- Validate with known data
- Check edge cases

### Automated Testing (Future)
- Unit tests for Python scripts
- Integration tests for workflows
- Continuous integration on GitHub

## Deployment Architecture

### Development → Production

```
Development
    ├── Local edits
    ├── Testing
    └── Commit to Git
        ↓
GitHub Repository
        ↓
Production Systems
    ├── Pull from Git
    ├── Deploy to ~/vnmrsys/
    └── Validate
```

### Version Control

**Branches**:
- `main` - Stable, production-ready
- `develop` - Integration branch
- `feature/*` - New features

**Tags**:
- `v1.0`, `v1.1`, etc. - Release versions
- `production-YYYYMMDD` - Production deployments

## Troubleshooting Architecture

### Log Locations

```
OpenVNMRJ Logs:
    ~/vnmrsys/VnmrjMsgLog

Python Script Logs:
    [script output to stdout/stderr]

Acquisition Logs:
    ~/vnmrsys/exp*/log

System Logs:
    /var/log/messages (if root access)
```

### Debug Modes
- Macro verbosity levels
- Python verbose flags (`-v`)
- OpenVNMRJ debug parameters

## Future Architecture Considerations

### Planned Enhancements
1. **Database Backend**
   - Store reference data
   - Query previous experiments
   - Track sample metadata

2. **Web Interface**
   - Job submission
   - Result viewing
   - Parameter sharing

3. **Machine Learning Integration**
   - Spectral classification
   - Parameter optimization
   - Quality control

4. **Cloud Integration**
   - Remote processing
   - Collaborative analysis
   - Backup and archival

---

## References

- OpenVNMRJ Documentation: `/vnmr/help/`
- Python NumPy Docs: https://numpy.org/doc/
- NMRPipe: https://www.ibbr.umd.edu/nmrpipe/

---

**Document Version**: 1.0
**Last Updated**: 2025-11-04
**Maintainer**: Chad
