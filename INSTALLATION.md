# Installation Guide

Complete installation instructions for VNMRJ NMR Tools.

## Prerequisites

### System Requirements
- **Operating System**: Linux (tested on CentOS, Ubuntu, Debian)
- **OpenVNMRJ**: Version 1.1A or later (or VNMRJ 4.2+)
- **Python**: 3.6 or later
- **Disk Space**: ~100 MB for all tools

### Required Packages

#### Python Dependencies
```bash
# Install pip if not available
sudo apt-get install python3-pip  # Debian/Ubuntu
sudo yum install python3-pip      # CentOS/RHEL

# Install required packages
pip3 install numpy scipy matplotlib
```

#### Optional Dependencies
```bash
# For NMRPipe conversion (if using conversion tools)
# NMRPipe must be installed separately

# For advanced plotting
pip3 install seaborn pandas
```

## Installation Methods

### Method 1: Clone from GitHub (Recommended)

```bash
# Clone the repository
cd ~
git clone https://github.com/ChadResynant/vnmrj-nmr-tools.git
cd vnmrj-nmr-tools

# Run installation script
./install.sh
```

### Method 2: Manual Installation

#### Install Macros

```bash
# Copy all macros to OpenVNMRJ
cd ~/vnmrj-nmr-tools
cp macros/*/* ~/vnmrsys/maclib/

# Make them executable
chmod +x ~/vnmrsys/maclib/*

# OpenVNMRJ will automatically recognize them
```

#### Install Python Scripts

```bash
# Copy to bin directory
cp python/*/*.py ~/vnmrsys/bin/

# Make executable
chmod +x ~/vnmrsys/bin/*.py

# Add to PATH (if not already)
echo 'export PATH=$PATH:~/vnmrsys/bin' >> ~/.bashrc
source ~/.bashrc
```

#### Install Pulse Sequences

```bash
# Copy pulse sequences
cp pulsesequences/*.c ~/vnmrsys/psglib/

# Compile them (in OpenVNMRJ)
# For each sequence:
#   seqgen('your_sequence')
```

### Method 3: Selective Installation

If you only need specific tools:

```bash
# Just OPTO macros
cp macros/opto/* ~/vnmrsys/maclib/

# Just Python utilities
cp python/utilities/*.py ~/vnmrsys/bin/
chmod +x ~/vnmrsys/bin/*.py

# Just specific pulse sequence
cp pulsesequences/your_sequence.c ~/vnmrsys/psglib/
```

## Post-Installation

### Verify Installation

#### Test Macros
```bash
# In OpenVNMRJ command line:
macro?  # Should list installed macros
# Look for: gosopto, mydsa, gos-default, etc.
```

#### Test Python Scripts
```bash
# In terminal:
python3 ~/vnmrsys/bin/organize_nmr_data.py --help

# Should display help message without errors
```

#### Test Pulse Sequences
```bash
# In OpenVNMRJ:
exists('your_sequence', 'psglib'):$e
if ($e > 0) then
    write('line3', 'Pulse sequence installed correctly')
endif
```

### Configure for Your System

#### 1. Set System-Specific Paths

Some scripts may need path adjustments:

```bash
# Edit scripts to match your system
vim ~/vnmrsys/bin/your_script.py

# Update paths like:
VNMRSYS = os.path.expanduser('~/vnmrsys')
DATA_DIR = '/path/to/your/data'
```

#### 2. Configure OPTO (if using)

```bash
# Set OPTO server address in gosopto macro
vim ~/vnmrsys/maclib/gosopto

# Update OPTO_SERVER variable
$OPTO_SERVER = 'your.opto.server.edu'
```

#### 3. Set Up Permissions

```bash
# Ensure proper ownership
chown -R $USER:$USER ~/vnmrsys/maclib
chown -R $USER:$USER ~/vnmrsys/bin

# Set proper permissions
chmod 755 ~/vnmrsys/maclib/*
chmod 755 ~/vnmrsys/bin/*.py
```

## Installation for Multiple Users

### System-Wide Installation

```bash
# Install to system directories (requires root)
sudo cp macros/*/* /vnmr/maclib/
sudo cp python/*/*.py /vnmr/bin/
sudo chmod +x /vnmr/maclib/*
sudo chmod +x /vnmr/bin/*.py

# All users will have access
```

### Shared User Directory

```bash
# Install to shared location
SHARED_DIR=/shared/nmr/tools
mkdir -p $SHARED_DIR

cp -r vnmrj-nmr-tools/* $SHARED_DIR/

# Users add to their personal directories:
# In each user's ~/vnmrsys/maclib/:
ln -s $SHARED_DIR/macros/*/* .

# In each user's ~/vnmrsys/bin/:
ln -s $SHARED_DIR/python/*/*.py .
```

## Updating

### Update from Git

```bash
cd ~/vnmrj-nmr-tools
git pull origin main

# Re-run installation
./install.sh

# Or manually copy updated files
cp macros/*/* ~/vnmrsys/maclib/
cp python/*/*.py ~/vnmrsys/bin/
```

### Rollback to Previous Version

```bash
cd ~/vnmrj-nmr-tools

# See available versions
git tag

# Checkout specific version
git checkout v1.0

# Re-install
./install.sh
```

## Uninstallation

### Remove All Tools

```bash
# List installed files
cat ~/vnmrj-nmr-tools/INSTALLED_FILES.txt

# Remove macros (be careful - don't remove system macros!)
cd ~/vnmrsys/maclib
rm gosopto opto_fidarea_measure mydsa gos-default

# Remove Python scripts
cd ~/vnmrsys/bin
rm organize_nmr_data.py supercleanup.py

# Remove pulse sequences
cd ~/vnmrsys/psglib
rm your_custom_sequences.c
```

### Keep Configuration

```bash
# Backup your configurations before uninstalling
cp ~/vnmrsys/maclib/configured_macro ~/backup/
cp ~/vnmrsys/bin/configured_script.py ~/backup/
```

## Troubleshooting Installation

### Issue: Macros not found in OpenVNMRJ

**Solution**:
```bash
# Check if files are in correct location
ls -l ~/vnmrsys/maclib/gosopto

# Ensure executable
chmod +x ~/vnmrsys/maclib/gosopto

# Restart OpenVNMRJ if running
```

### Issue: Python scripts fail with import errors

**Solution**:
```bash
# Check Python version
python3 --version  # Should be 3.6+

# Install missing packages
pip3 install numpy scipy

# Check script permissions
chmod +x ~/vnmrsys/bin/your_script.py
```

### Issue: Pulse sequences won't compile

**Solution**:
```bash
# Check if source file exists
ls -l ~/vnmrsys/psglib/your_sequence.c

# Check for syntax errors
cat ~/vnmrsys/psglib/your_sequence.c

# Manually compile in OpenVNMRJ
seqgen('your_sequence')

# Check for error messages
cat ~/vnmrsys/seqlib/your_sequence.errors
```

### Issue: Permission denied errors

**Solution**:
```bash
# Fix ownership
chown -R $USER:$USER ~/vnmrsys

# Fix permissions
chmod -R u+rwx ~/vnmrsys/maclib
chmod -R u+rwx ~/vnmrsys/bin
```

## Platform-Specific Notes

### CentOS/RHEL 7
- Python 3.6+ may need to be installed from EPEL
- SELinux may interfere - check with `getenforce`

### Ubuntu/Debian
- Use `apt` instead of `yum` for package management
- Default Python 3 should work

### macOS (if running OpenVNMRJ)
- Use Homebrew for Python: `brew install python3`
- May need to adjust shebangs: `#!/usr/bin/env python3`

## Verification Checklist

After installation, verify:

- [ ] Macros appear in OpenVNMRJ macro list
- [ ] Python scripts run without import errors
- [ ] Pulse sequences compile successfully
- [ ] File permissions are correct
- [ ] PATH includes script directories
- [ ] Dependencies installed

## Getting Help

- **Documentation**: See `docs/` directory
- **Issues**: https://github.com/ChadResynant/vnmrj-nmr-tools/issues
- **Examples**: See `examples/` directory

---

**Installation complete!** 🎉 See README.md for usage instructions.
