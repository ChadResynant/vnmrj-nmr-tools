# VNMRJ NMR Tools

**PUBLIC REPOSITORY** - Open source NMR utilities and workflows

## Overview

Collection of macros, Python scripts, and utilities for NMR data acquisition, processing, and analysis using OpenVNMRJ/VNMRJ. This repository contains general-purpose tools developed across multiple NMR facilities and spectrometer systems.

**Status**: Active development
**Version**: 1.0
**Last Updated**: 2025-11-04

## Features

- **OPTO workflows** - Optimal-parameter NMR automation
- **Acquisition utilities** - Custom macros for routine and advanced experiments
- **Processing scripts** - Python tools for data conversion and analysis
- **File organization** - Utilities for managing large NMR datasets
- **Multi-system support** - Code tested across 400-750 MHz spectrometers

## Quick Start

### Installation

```bash
# Clone repository
git clone https://github.com/YOUR_USERNAME/vnmrj-nmr-tools.git
cd vnmrj-nmr-tools

# Copy macros to OpenVNMRJ
cp macros/*/*.* ~/vnmrsys/maclib/

# Copy Python scripts
cp python/*/*.py ~/vnmrsys/bin/
chmod +x ~/vnmrsys/bin/*.py

# Add to Python path (optional, for processing-only workstations)
echo "export PYTHONPATH=\$PYTHONPATH:$(pwd)/python" >> ~/.bashrc
```

### Requirements

- **OpenVNMRJ** 1.1A or later (or VNMRJ 4.2+)
- **Python** 3.6+ with NumPy, SciPy
- **Optional**: NMRPipe, TopSpin (for conversion utilities)

## Repository Structure

```
vnmrj-nmr-tools/
├── macros/
│   ├── opto/                         # OPTO workflow macros
│   │   ├── gosopto                   # OPTO submission macro
│   │   └── opto_fidarea_measure      # FID area measurement
│   │
│   ├── acquisition/                  # Acquisition utilities
│   │   ├── mydsa                     # Custom dsa (array setup)
│   │   └── gos-default               # Modified gos (go and save)
│   │
│   └── processing/                   # Processing macros
│       └── [to be collected]
│
├── python/
│   ├── conversion/                   # Format conversion scripts
│   │   └── [to be collected]
│   │
│   ├── analysis/                     # Analysis tools
│   │   └── [to be collected]
│   │
│   └── utilities/                    # General utilities
│       ├── organize_nmr_data.py      # File organization
│       ├── organize_nmr_data_v2.py   # Enhanced version
│       ├── supercleanup.py           # Data cleanup
│       └── superclean_v2.py          # Enhanced cleanup
│
├── pulsesequences/                   # Custom pulse sequences
│   └── [to be collected]
│
├── docs/                             # Documentation
│   └── [to be added]
│
├── examples/                         # Example workflows
│   └── [to be added]
│
├── .gitignore
└── README.md                         # This file
```

## Components

### OPTO Workflows

OPTO (Optimal Parameter Estimation) automation for high-throughput NMR.

**Files**:
- `macros/opto/gosopto` - Submit jobs to OPTO queue
- `macros/opto/opto_fidarea_measure` - Measure FID integral for optimization

**Usage**:
```bash
# In OpenVNMRJ:
gosopto  # Submit current experiment to OPTO
```

**Note**: Full OPTO infrastructure to be collected from NMRFAM systems.

### Acquisition Macros

Custom acquisition macros for specialized experiments.

**Files**:
- `macros/acquisition/mydsa` - Custom array setup (dsa replacement)
- `macros/acquisition/gos-default` - Modified gos with enhanced features

**Usage**:
```bash
# In OpenVNMRJ:
mydsa     # Set up arrayed experiment
gos       # Go and save with enhancements
```

### Python Utilities

General-purpose Python scripts for NMR data management.

**organize_nmr_data.py** - Organize and archive NMR datasets
```bash
python3 organize_nmr_data.py --source ~/vnmrsys --dest ~/data/archive/
```

**supercleanup.py** - Clean up temporary files and organize workspace
```bash
python3 supercleanup.py ~/vnmrsys/
```

## System Compatibility

This code has been tested and used across multiple NMR systems:

| System | Field | Location | Status |
|--------|-------|----------|--------|
| Ayrshire | 750 MHz | NMRFAM | In use |
| Ribeye | 600 MHz | NMRFAM | In use |
| Taurus | 600 MHz | NMRFAM | In use |
| Fyfe | 400 MHz | Resynant | In use |
| Fyddle | 600 MHz | Resynant | In use |
| Eagle | - | Home | Processing only |

## Development

### Contributing

Contributions welcome! This repository collects useful NMR tools from the community.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-tool`)
3. Add your tool with documentation
4. Test on your system
5. Submit a pull request

### Code Standards

- **Macros**: Document with header comments, include usage examples
- **Python**: PEP 8 style, include docstrings
- **Documentation**: README for each tool/workflow
- **Testing**: Include example data or test cases when possible

### Adding New Tools

```bash
# Add a new macro
cp your_macro ~/github/vnmrj-nmr-tools/macros/processing/
# Document in README
# Test deployment to ~/vnmrsys/maclib/
# Commit and push

# Add a new Python script
cp your_script.py ~/github/vnmrj-nmr-tools/python/analysis/
chmod +x ~/github/vnmrj-nmr-tools/python/analysis/your_script.py
# Add docstrings and usage examples
# Commit and push
```

## Collection Status

This repository is actively being populated with code from 6 different NMR workstations. See `COLLECTION_NEEDED.txt` in the staging area for tracking.

**Current status** (as of 2025-11-04):
- ✅ Eagle (home processing workstation) - Initial collection complete
- ⏳ NMRFAM systems - Collection in progress
- ⏳ Resynant systems - Collection planned
- ⏳ Pulse sequences - To be collected
- ⏳ Complete OPTO infrastructure - To be collected

## Roadmap

### Version 1.0 (Current)
- [x] Basic OPTO macros
- [x] File organization utilities
- [ ] Complete OPTO infrastructure
- [ ] Custom pulse sequence collection

### Version 1.1 (Planned)
- [ ] Data conversion utilities (FID → NMRPipe, nmrML, etc.)
- [ ] Standard processing pipelines
- [ ] Enhanced documentation with examples
- [ ] Automated testing framework

### Version 2.0 (Future)
- [ ] Web interface for job submission
- [ ] Automated quality control
- [ ] Integration with LIMS systems
- [ ] Machine learning tools for spectral analysis

## Documentation

- **README.md** (this file) - Overview and quick start
- **docs/** - Detailed documentation for each tool (to be added)
- **examples/** - Example workflows and use cases (to be added)

## Troubleshooting

### Macros don't work after copying
- Ensure macros are executable: `chmod +x ~/vnmrsys/maclib/macro_name`
- Check for path dependencies in macro code
- Verify OpenVNMRJ version compatibility

### Python scripts fail with import errors
- Install required packages: `pip3 install numpy scipy`
- Check Python version: `python3 --version` (need 3.6+)
- Verify script has execute permissions

### OPTO workflows fail
- Check OPTO server configuration
- Verify network connectivity to OPTO system
- Ensure proper authentication/credentials

## Support

- **Issues**: File bug reports or feature requests in GitHub Issues
- **Discussions**: Use GitHub Discussions for questions and community support
- **Contributing**: See CONTRIBUTING.md for development guidelines

## License

MIT License - See LICENSE file for details

This is open source software. Use freely for research and analysis. Attribution appreciated but not required.

## Acknowledgments

Code developed and tested at:
- **NMRFAM** (National Magnetic Resonance Facility at Madison)
- **Resynant**
- Various home/remote processing workstations

Thanks to the NMR community for tools, techniques, and inspiration.

## Citation

If you use these tools in publications, please cite:
```
[To be determined - add DOI/citation after publication]
```

## Contact

**Maintainer**: Chad
**Status**: Active development
**Repository**: https://github.com/YOUR_USERNAME/vnmrj-nmr-tools

---

**Last Updated**: 2025-11-04
**Version**: 1.0-beta
**Status**: Collection in progress
