# OPTO Stripchart Utility - Real-Time Monitoring

**Tool**: `opto_stripchart4.py`
**Location**: `opto/utilities/opto_stripchart4.py`
**Purpose**: Real-time visualization of OPTO optimization progress
**Last Updated**: 2025-11-09

## Overview

The OPTO stripchart utility provides real-time graphical monitoring of optimization runs. It plots both the optimization score and parameter changes as OPTO progresses, making it easy to see convergence, detect problems, and analyze results.

### Key Features

- **Real-time updates**: Automatically refreshes as new data arrives
- **Parameter tracking**: Shows how each parameter changes during optimization
- **Statistics**: Calculates mean, std dev, and best N results
- **Directory monitoring**: Auto-switches to newest OPTO log file
- **Auto-save plots**: Saves plots when switching files or ending
- **Best value tracking**: Highlights best result and its parameters

---

## Quick Start

### Installation

```bash
# Navigate to repository
cd /path/to/nmr-tools

# Make executable
chmod +x opto/utilities/opto_stripchart4.py

# Install Python dependencies (if not already installed)
pip3 install numpy matplotlib
```

**Requirements**:
- Python 3.6+
- NumPy
- Matplotlib

### Basic Usage

**Monitor a specific OPTO log file**:
```bash
./opto/utilities/opto_stripchart4.py /path/to/OPTO-2025-11-09-14-30-00
```

**Monitor directory for latest files** (recommended):
```bash
./opto/utilities/opto_stripchart4.py /path/to/opto/logs --monitor-dir -p
```

**From current directory**:
```bash
./opto/utilities/opto_stripchart4.py . --monitor-dir -p
```

---

## Usage Modes

### Mode 1: Single File Monitoring

**When to use**:
- Analyzing completed optimization
- Reviewing historical run
- Debugging specific optimization

**Command**:
```bash
./opto/utilities/opto_stripchart4.py OPTO-2025-11-09-14-30-00
```

**What it does**:
- Plots data from specified file
- Updates if file is still being written to
- Shows final plot when file stops changing

### Mode 2: Directory Monitoring (Recommended)

**When to use**:
- Running OPTO optimization now
- Want real-time visualization
- Multiple optimizations planned

**Command**:
```bash
./opto/utilities/opto_stripchart4.py . --monitor-dir -p
```

**What it does**:
- Scans directory for `OPTO-YYYY-MM-DD-HH-MM-SS` files
- Starts with most recent file
- Auto-switches to new files as they appear
- Saves plot when switching files

**Workflow**:
1. Start stripchart in OPTO log directory
2. Start OPTO optimization (creates new log file)
3. Stripchart automatically detects and displays new file
4. Monitor progress in real-time
5. If you start another OPTO run, stripchart switches automatically
6. Previous run's plot is auto-saved

---

## Command-Line Options

### Essential Options

**`--monitor-dir` or `-m`**:
- Enable directory monitoring mode
- Auto-detect and switch to new OPTO files

**`--plot-params` or `-p`**:
- Plot parameter changes (highly recommended)
- Shows how each parameter evolves
- Essential for understanding optimization behavior

### Advanced Options

**`-n` or `--max-points NUM`**:
- Maximum points to display (scrolling window)
- Default: 500
- Example: `-n 1000` for longer optimizations

**`-i` or `--interval MS`**:
- Update interval in milliseconds
- Default: 1000 (1 second)
- Example: `-i 2000` for slower updates (less CPU usage)

**`-r` or `--range VAL`**:
- Parameter change range for normalization
- Default: 32767
- Rarely needs changing

**`--no-save`**:
- Disable automatic plot saving
- Use if you don't want PNG files cluttering directory

**`-s` or `--skip-header N`**:
- Number of header lines to skip
- Default: 1
- Change if OPTO log format changes

**`-d` or `--delimiter STR`**:
- Column delimiter
- Default: whitespace
- Example: `-d ","` for CSV files

### Help

```bash
./opto/utilities/opto_stripchart4.py --help
```

---

## Understanding the Plots

### Score Plot (Top Panel)

**Shows**:
- Blue line with markers: optimization score vs. line number
- Each point: one OPTO measurement
- Higher is better (maximizing signal)

**Information Box (Lower Right)**:
- **Points**: Total measurements so far
- **Latest**: Most recent score (normalized to initial)
- **Best**: Best score achieved (normalized to initial)
- **Best N mean**: Average of top 10 results
- **Best N std**: Standard deviation of top 10

**What to Look For**:
- ✅ Upward trend: optimization improving
- ✅ Plateau at high value: converged successfully
- ⚠️ Downward trend: possible problem (wrong integration range, parameter limits)
- ⚠️ Random fluctuations: low SNR, or not converging
- ⚠️ Spike at start: steady-state issue (see workarounds doc)

**Normalization**:
- Values shown as ratio to first measurement
- 1.0 = same as initial
- 1.2 = 20% improvement
- 0.9 = 10% degradation

### Parameter Plots (Lower Panels)

**One plot per parameter being optimized**

**Shows**:
- Parameter **change** from initial value (Δ = current - initial)
- Orange/green/red lines: different parameters
- Horizontal dashed line at y=0: no change from initial

**Information Boxes**:
- **Left box**: Δ: mean ± std (average change across all measurements)
- **Center box**: Best 10: mean ± std (average value for top 10 results)

**What to Look For**:
- ✅ Convergence: parameter settles to specific value
- ✅ Correlation with score: parameter changes when score improves
- ⚠️ Continuous drift: not converging, limits may be wrong
- ⚠️ Hitting limits: parameter at min or max (expand range?)
- ✅ Small std in "Best 10": consistent optimum
- ⚠️ Large std in "Best 10": optimum not well-defined

**Understanding "Best 10" Values**:
- These are the parameter values for the top 10 measurements
- **Recommended final parameters**: Use "Best 10 mean" values
- Low std → confident in recommendation
- High std → multiple local optima, or shallow optimum

---

## Example Session

### Setup

```bash
# On spectrometer workstation, in experiment directory:
cd /opt/topspin4.5.0/data/user/nmr/OPTO_logs

# In separate terminal or tmux session:
/path/to/nmr-tools/opto/utilities/opto_stripchart4.py . --monitor-dir -p
```

### Starting OPTO

```
# Stripchart output:
Monitoring directory: /opt/topspin4.5.0/data/user/nmr/OPTO_logs
Looking for OPTO-YYYY-MM-DD-HH-MM-SS files...
Update interval: 1000 ms
Max points: 500
Plotting parameter changes: ENABLED
Plot saving: ENABLED
Press Ctrl+C to stop
```

**Plot window opens** (empty, waiting for data)

### OPTO Run Starts

```
# Stripchart detects new file:
New file detected: OPTO-2025-11-09-15-45-32
Starting with file: OPTO-2025-11-09-15-45-32
```

**Plot begins updating**:
- Score plot shows first point
- Parameter plots initialize
- Updates every second

### Monitoring Progress

```
DEBUG: 5 points, line numbers: [2, 3, 4, 5, 6]
DEBUG: 10 points, line numbers: [2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
DEBUG: 20 points, line numbers: [2, 3, ..., 21]
...
```

**Watch the plots**:
- Score increasing? ✅ Good!
- Parameters converging? ✅ Good!
- Score decreasing? ⚠️ Check integration ranges
- Parameters hitting limits? ⚠️ Expand ranges

### Optimization Completes

```
# No new data for several update cycles
# Plot shows final state
```

**Final plot shows**:
- Total points: e.g., 87
- Best result: line 64, score 1.23 (23% improvement)
- Best 10 mean: 1.21 ± 0.02
- Parameter recommendations in "Best 10" boxes

### Starting Another Run

```
# Start new OPTO in Topspin
# Stripchart automatically detects:

New file detected: OPTO-2025-11-09-16-15-08
Plot saved: OPTO-2025-11-09-15-45-32_plot_20251109_161510.png
Switched to monitoring: OPTO-2025-11-09-16-15-08
```

**Previous plot saved**, new plot begins

### Stopping Stripchart

```
# Press Ctrl+C:
^C
Plot saved: OPTO-2025-11-09-16-15-08_plot_20251109_162245.png
Monitoring stopped by user
```

**Final plot auto-saved**

---

## Interpreting Results

### Successful Optimization Pattern

**Score plot**:
```
Score
 1.3 |                    ____
     |               ____/
 1.2 |          ____/
     |     ____/
 1.1 | ___/
     |/
 1.0 +-------------------------
     0    20    40    60    80
          Line Number
```

**Indicators**:
- Smooth increase
- Clear plateau
- Best 10 mean ≈ plateau value
- Low std in Best 10

**Parameters**:
- Converge to specific values
- Small variation in Best 10
- Clear trend toward optimum

### Failed Optimization Pattern

**Score plot**:
```
Score
 1.0 |\
     | \____
 0.9 |      \___
     |          \____
 0.8 |               \
     |                \___
 0.7 +-------------------------
     0    20    40    60    80
          Line Number
```

**Indicators**:
- Decreasing score
- No convergence
- High std in Best 10

**Likely Causes**:
1. Wrong integration range (optimizing noise or wrong peak)
2. Parameter limits incorrect (exploring bad parameter space)
3. Steady-state issue making first point artificially high
4. Starting point was local maximum, OPTO found worse region

**Actions**:
1. Stop OPTO ("Abort Nicely")
2. Check integration ranges (see `integration-ranges.md`)
3. Verify parameter limits match HH conditions
4. Check for steady-state issue (see `steady-state.md`)
5. Restart with corrected setup

### Noisy Optimization Pattern

**Score plot**:
```
Score
 1.2 |  *  * *    * *
     |   *   * * *  *  *
 1.1 | *  * *   *   *  *
     |*  *   *      *
 1.0 +*-------------------------
     0    20    40    60    80
          Line Number
```

**Indicators**:
- High variability
- No clear trend
- High std in Best 10
- Mean may show slight improvement

**Possible Causes**:
1. Low SNR sample
2. Insufficient scans (ns too low)
3. Integration range includes noise
4. Parameters in flat region (shallow optimum)

**Actions**:
- Increase `ns` (number of scans)
- Verify integration range
- Use "Best N mean" from stripchart (averages out noise)
- Consider longer optimization for better statistics

---

## Saved Plots

### File Naming

**Format**:
```
OPTO-YYYY-MM-DD-HH-MM-SS_plot_YYYYMMDD_HHMMSS.png
```

**Example**:
```
OPTO-2025-11-09-15-45-32_plot_20251109_161510.png
│                         │
│                         └─ Timestamp when saved
└─ Original OPTO log filename
```

### What's Saved

- All panels (score + all parameters)
- Current state when file switches or Ctrl+C pressed
- 150 DPI PNG (good quality, reasonable file size)

### Viewing Saved Plots

```bash
# Standard image viewer:
open OPTO-*_plot_*.png   # macOS
eog OPTO-*_plot_*.png    # Linux
```

### Including in Reports

- Publication quality at 150 DPI
- Can annotate with image editor
- Embed in presentations/reports
- Compare multiple runs side-by-side

---

## Tips and Tricks

### Tip 1: Use tmux for Remote Monitoring

```bash
# On remote spectrometer:
tmux new -s opto-monitor

# Start stripchart:
./opto_stripchart4.py . --monitor-dir -p

# Detach: Ctrl+B, then D

# Reattach later:
tmux attach -t opto-monitor
```

**Benefit**: Stripchart keeps running even if you disconnect

### Tip 2: Adjust Update Interval for CPU Usage

```bash
# Slower updates (less CPU):
./opto_stripchart4.py . -m -p -i 5000  # 5 second updates

# Faster updates (more responsive):
./opto_stripchart4.py . -m -p -i 500   # 0.5 second updates
```

### Tip 3: Save plots manually

Even with `--no-save`, you can screenshot the plot window at any time.

### Tip 4: Analyzing Old Runs

```bash
# Point to any old OPTO log:
./opto_stripchart4.py /archive/OPTO-2025-10-15-09-30-22 -p

# View final state, explore parameters
```

### Tip 5: Multiple Simultaneous Runs

**Run multiple stripcharts** in different terminals:
```bash
# Terminal 1: Monitor current run
./opto_stripchart4.py . -m -p

# Terminal 2: Review previous run
./opto_stripchart4.py ../old_logs/OPTO-2025-11-08-* -p
```

---

## Troubleshooting

### Problem: No plot appears

**Check**:
```bash
# Dependencies installed?
python3 -c "import numpy, matplotlib; print('OK')"

# File exists?
ls OPTO-2025-11-09-*

# Permissions?
ls -l opto_stripchart4.py  # Should be executable
```

**Solutions**:
```bash
# Install dependencies:
pip3 install --user numpy matplotlib

# Make executable:
chmod +x opto_stripchart4.py
```

### Problem: "No OPTO-* files found"

**In directory mode**, stripchart looks for files matching `OPTO-YYYY-MM-DD-HH-MM-SS`.

**Solutions**:
- Wait for OPTO to start (creates log file)
- Check you're in the right directory
- Verify OPTO is actually running
- Try specific file mode instead

### Problem: Plot shows but doesn't update

**Check**:
- Is OPTO actually running?
- Is file being written to?
```bash
# Watch file size:
watch -n 1 ls -lh OPTO-2025-11-09-*
```

**If file not growing**:
- OPTO may have stopped
- Check Topspin terminal for errors
- Check OPTO GUI status

### Problem: Too many parameters, plot is huge

**Default**: Figure height = 6 + n_params × 1.8 inches

**For many parameters** (>10):
- May not fit on screen
- Resize plot window
- Or modify script if needed

**Consider**: Optimize fewer parameters at once (multi-pass)

### Problem: Stripchart uses too much CPU

**Solutions**:
- Increase update interval: `-i 5000`
- Reduce max points: `-n 100`
- Close plot window when not actively monitoring

---

## Related Documentation

- **OPTO Manual**: General OPTO usage
- **Integration Ranges**: `opto/docs/troubleshooting/integration-ranges.md`
- **Schanda Issues**: `opto/docs/beta-testing/schanda-group-issues.md`
- **Log Analysis**: Manual sorting methods in Schanda doc

---

## Future Enhancements

**Planned** (not yet implemented):
- Web-based version (monitor from any browser)
- Multi-run comparison
- Export data to CSV
- Automatic recommendations
- Integration with ML predictions

---

## Support

**Questions or Issues?**
1. Check this documentation
2. Review example session above
3. Contact: Chad Rienstra

**Bug Reports**:
- Include: Python version, OS, error message
- Attach: OPTO log file that causes issue
- Send to: development team

---

**Tool Version**: 4.0
**Documentation Author**: Chad Rienstra
**Last Updated**: 2025-11-09
**Location**: `opto/utilities/opto_stripchart4.py`
