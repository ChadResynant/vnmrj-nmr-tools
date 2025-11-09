# Contributing to VNMRJ NMR Tools

Thank you for your interest in contributing! This document provides guidelines for adding your NMR tools and workflows to this repository.

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/vnmrj-nmr-tools.git
   cd vnmrj-nmr-tools
   ```
3. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-tool-name
   ```

## What to Contribute

We welcome contributions of:
- **Macros**: Custom OpenVNMRJ/VNMRJ macros for acquisition or processing
- **Pulse sequences**: Custom pulse sequences (.c files)
- **Python scripts**: Data analysis, conversion, or utility scripts
- **Shell scripts**: Automation and workflow scripts
- **Documentation**: Improvements to existing docs or new guides
- **Examples**: Working examples and use cases
- **Bug fixes**: Corrections to existing code

## Directory Structure

Place your contributions in the appropriate directory:

```
macros/
├── opto/           # OPTO-related macros
├── acquisition/    # Acquisition setup macros
└── processing/     # Processing and analysis macros

python/
├── conversion/     # Format conversion tools
├── analysis/       # Analysis scripts
└── utilities/      # General utility scripts

pulsesequences/     # Pulse sequence .c files

docs/              # Documentation
examples/          # Example workflows
```

## Code Standards

### Macros (MAGICAL/OpenVNMRJ)

```bash
" Macro Name: my_macro
" Author: Your Name
" Date: YYYY-MM-DD
" Purpose: Brief description of what this macro does
"
" Usage: my_macro [arguments]
"
" Example:
"   my_macro('setup')
"   my_macro('process', 1.5)

" Your code here
write('line3', 'Macro executed successfully')
```

**Requirements**:
- Header comment block with name, author, date, purpose, usage
- Clear variable names
- Inline comments for complex logic
- Error handling where appropriate

### Python Scripts

```python
#!/usr/bin/env python3
"""
Script name: my_script.py
Author: Your Name
Date: YYYY-MM-DD

Description:
    Brief description of what this script does.

Usage:
    python3 my_script.py --input file.fid --output file.txt

Requirements:
    - numpy
    - scipy
"""

import argparse
import numpy as np

def main():
    """Main function with clear docstring."""
    pass

if __name__ == "__main__":
    main()
```

**Requirements**:
- PEP 8 style (use `flake8` or `black`)
- Docstrings for all functions and classes
- Argparse for command-line arguments
- Error handling
- Requirements documented in docstring

### Pulse Sequences

```c
/*
 * Pulse Sequence: my_sequence.c
 * Author: Your Name
 * Date: YYYY-MM-DD
 *
 * Description:
 *   Brief description of the pulse sequence
 *
 * Parameters:
 *   pw - 90 degree pulse width
 *   d1 - relaxation delay
 *   ...
 *
 * Reference:
 *   Citation if based on published work
 */
```

**Requirements**:
- Header comment with description and parameters
- Well-commented code
- Include parameter documentation
- Cite references if applicable

## Testing Your Contribution

### For Macros
1. Copy to `~/vnmrsys/maclib/` on your system
2. Test with representative data
3. Document any dependencies (other macros, parameters)
4. Include example usage in comments

### For Python Scripts
1. Test with example data
2. Include command-line help: `--help` flag
3. Handle common error cases
4. Document Python version and dependencies

### For Pulse Sequences
1. Test on actual spectrometer (if possible)
2. Document hardware requirements (probes, gradients, etc.)
3. Include example parameter set
4. Note any system-specific issues

## Submitting Your Contribution

1. **Commit your changes**:
   ```bash
   git add .
   git commit -m "Add [feature]: brief description"
   ```

2. **Push to your fork**:
   ```bash
   git push origin feature/your-tool-name
   ```

3. **Create a Pull Request**:
   - Go to the original repository on GitHub
   - Click "New Pull Request"
   - Select your fork and branch
   - Fill in the PR template (see below)

## Pull Request Template

```markdown
## Description
Brief description of what this PR adds or fixes.

## Type of Change
- [ ] New macro
- [ ] New Python script
- [ ] New pulse sequence
- [ ] Bug fix
- [ ] Documentation update
- [ ] Other (describe)

## Testing
Describe how you tested this:
- System: (e.g., 600 MHz Varian)
- Software: (e.g., OpenVNMRJ 1.1A)
- Test cases: (what you tested)

## Checklist
- [ ] Code follows style guidelines
- [ ] Comments/documentation added
- [ ] Tested on actual system
- [ ] Dependencies documented
- [ ] Example usage provided

## Additional Notes
Any other information relevant to reviewers.
```

## Code Review Process

All contributions will be reviewed for:
- **Functionality**: Does it work as described?
- **Documentation**: Is it well-documented?
- **Style**: Does it follow conventions?
- **Safety**: No dangerous operations without warnings
- **Compatibility**: Works with standard OpenVNMRJ installations

## Questions or Problems?

- **Issues**: File an issue on GitHub
- **Discussions**: Use GitHub Discussions for questions
- **Contact**: Reach out to repository maintainers

## License

By contributing, you agree that your contributions will be licensed under the MIT License (same as the repository).

## Acknowledgments

Contributors will be acknowledged in:
- README.md (Contributors section)
- Release notes for version where contribution was merged
- Code comments (author attribution)

## Attribution

If your contribution is based on or inspired by others' work:
- Cite the original work in comments
- Include appropriate license information
- Give credit in documentation

## Examples of Good Contributions

See the `examples/` directory for well-documented contributions that follow these guidelines.

---

Thank you for contributing to the NMR community! 🔬
