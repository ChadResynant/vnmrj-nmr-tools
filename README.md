# NMR Tools

**Platform-agnostic NMR utilities and optimization tools**

## Overview

Comprehensive collection of NMR tools, utilities, and workflows for data acquisition, optimization, and analysis. This repository contains platform-agnostic tools (OPTO, BPHON, etc.) alongside platform-specific implementations for both Varian/OpenVNMRJ and Bruker/Topspin systems.

**Licensed Technology**: All NMRFAM technology licensed to Resynant through WARF
**Status**: Active development
**Version**: 2.0-alpha
**Last Updated**: 2025-11-09

## Repository Structure

```
nmr-tools/
├── varian/                         # Varian/OpenVNMRJ specific
│   ├── vnmrsys/                   # Pulse sequences, macros
│   └── macros/                    # Custom acquisition macros
│       ├── opto/                  # OPTO integration
│       └── acquisition/           # Acquisition utilities
│
├── bruker/                         # Bruker/Topspin specific
│   └── topspin/
│       ├── installation/          # Installation guides
│       ├── pulse-sequences/       # Bruker pulse programs
│       ├── au-programs/           # AU automation programs
│       └── docs/                  # Topspin-specific documentation
│
├── opto/                          # Platform-agnostic OPTO
│   ├── utilities/                 # OPTO tools
│   │   └── opto_stripchart4.py   # Real-time monitoring
│   ├── examples/
│   │   ├── job-files/            # Example OPTO job files
│   │   └── parameters/           # Parameter sets (CP conditions)
│   └── docs/                     # OPTO documentation
│       ├── manual/               # User manual
│       ├── troubleshooting/      # Common issues & solutions
│       ├── quick-start/          # Getting started guides
│       ├── beta-testing/         # Beta tester feedback
│       ├── workarounds/          # Known issues & workarounds
│       └── platform-specific/    # Platform differences
│
├── bphon/                         # BPHON tools
│   ├── utilities/
│   └── docs/
│
├── sparky/                        # Sparky extensions
│   └── docs/
│
├── nmrsimulator/                  # NMR simulator
│   └── docs/
│
├── python/                        # Shared Python utilities
│   └── utilities/                # General NMR utilities
│       ├── organize_nmr_data.py  # Data organization
│       └── superclean_v2.py      # Workspace cleanup
│
├── docs/                          # Top-level documentation
│   └── platform-guides/          # Cross-platform guides
│
├── README.md                      # This file
└── LICENSE
```

## Core Tools

### OPTO - Optimal Parameter Estimation

Platform-agnostic NMR parameter optimization using Bayesian methods.

**Features**:
- CP optimization (cross-polarization conditions)
- Shimming automation
- Magic angle setting
- Real-time monitoring with stripchart utility
- Multi-pass optimization strategies

**Quick Start**:
```bash
# Monitor OPTO optimization in real-time
./opto/utilities/opto_stripchart4.py . --monitor-dir -p

# See platform-specific setup:
# - Varian: varian/macros/opto/
# - Bruker: bruker/topspin/docs/
```

**Documentation**: See `opto/docs/` for comprehensive guides

### BPHON - Biomolecular Proton Detection

Tools for optimizing biomolecular NMR experiments.

**Status**: Coming soon

### Platform-Specific Tools

#### Varian/OpenVNMRJ
- Custom macros for acquisition (`varian/macros/`)
- Pulse sequence library (`varian/vnmrsys/psglib/`)
- OPTO integration macros

#### Bruker/Topspin
- Installation guides (`bruker/topspin/installation/`)
- OPTO manual v1.2.34 (`bruker/topspin/docs/`)
- AU programs (coming soon)

## Installation

### OPTO Stripchart Utility

```bash
# Make executable
chmod +x opto/utilities/opto_stripchart4.py

# Install dependencies
pip3 install numpy matplotlib

# Run (monitors directory for latest OPTO files)
./opto/utilities/opto_stripchart4.py /path/to/opto/logs --monitor-dir -p
```

### Varian/OpenVNMRJ

```bash
# Copy macros
cp varian/macros/*/*.* ~/vnmrsys/maclib/

# Copy pulse sequences
cp varian/vnmrsys/psglib/*.c ~/vnmrsys/psglib/
```

### Bruker/Topspin

See `bruker/topspin/installation/` for platform-specific installation instructions.

## Beta Testing

**Active beta testers**:
- Paul Schanda's group (Grenoble) - Bruker NEO, Topspin 4.5, 55-100 kHz MAS
- NMRFAM (Madison) - Multiple systems

**Feedback & Issues**: See `opto/docs/beta-testing/` for current issues and resolutions

## System Compatibility

### Tested Systems

| System | Field | Platform | Location | Status |
|--------|-------|----------|----------|--------|
| Ayrshire | 750 MHz | Varian | NMRFAM | In use |
| Ribeye | 600 MHz | Varian | NMRFAM | In use |
| Taurus | 600 MHz | Varian | NMRFAM | In use |
| Girolando | 1.1 GHz | Bruker | NMRFAM | Testing |
| Schanda Group | 700 MHz | Bruker NEO | Grenoble | Beta testing |

### Platform Support

- **Varian/OpenVNMRJ**: 1.1A or later, VNMRJ 4.2+
- **Bruker/Topspin**: 4.x (NEO consoles), Alma Linux 9
- **Python**: 3.6+ with NumPy, SciPy, Matplotlib

## Documentation

### Quick Links

- **OPTO Manual**: `opto/docs/manual/`
- **Troubleshooting**: `opto/docs/troubleshooting/`
- **Beta Testing Issues**: `opto/docs/beta-testing/`
- **Example Parameters**: `opto/examples/parameters/`

### Key Documentation Files

1. **Getting Started**:
   - OPTO Quick Start: `opto/docs/quick-start/`
   - Platform Setup: `docs/platform-guides/`

2. **User Guides**:
   - OPTO Topspin Manual: `opto/docs/manual/OPTO_Topspin_Manual_1.2.34.docx`
   - Stripchart Utility: `opto/utilities/opto_stripchart4.py --help`

3. **Troubleshooting**:
   - Common Issues: `opto/docs/troubleshooting/`
   - Workarounds: `opto/docs/workarounds/`
   - Beta Tester Feedback: `opto/docs/beta-testing/`

## Development

### Branch Structure

- `main` - Stable releases
- `restructure-nmr-tools` - Current reorganization (v2.0)
- Development branches as needed

### Contributing

This is a commercial repository for Resynant. Development coordination with NMRFAM team (Barry Dezonia, Thiru).

### Roadmap

**Version 2.0 (In Progress)**:
- [x] Restructure repository for platform-agnostic organization
- [x] Add OPTO stripchart utility
- [ ] Complete OPTO documentation (troubleshooting, workarounds)
- [ ] Add default job files for common experiments
- [ ] Document beta tester issues and resolutions
- [ ] Add BPHON tools

**Version 2.1 (Planned)**:
- [ ] Bruker pulse sequence collection
- [ ] Enhanced CP optimization with HH intelligence
- [ ] Automated data saving for ML applications
- [ ] Web interface for OPTO monitoring

## Support

**Primary Contact**: Chad Rienstra
**Development Team**: NMRFAM (Barry Dezonia, Thiru)
**Beta Testers**: Paul Schanda (Grenoble), Petra Rovó

## License

MIT License - See LICENSE file

Licensed NMRFAM technology for commercial use by Resynant through WARF.

## Citation

```
[To be determined - pending publications]
```

## Acknowledgments

- **NMRFAM** (National Magnetic Resonance Facility at Madison)
- **Resynant**
- **Beta Testers**: Paul Schanda, Petra Rovó (Grenoble)
- **WARF** (Wisconsin Alumni Research Foundation)

---

**Repository**: https://github.com/ChadResynant/vnmrj-nmr-tools
**Last Updated**: 2025-11-09
**Version**: 2.0-alpha
