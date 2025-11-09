# Taurus Claude Code Setup Guide

**Purpose:** Run Claude Code directly on Taurus via VNC for testing
**Date:** November 9, 2025

---

## Step 1: Find or Clone the Repository on Taurus

Open a terminal in your VNC session and locate the vnmrj-nmr-tools repository:

```bash
# Option A: If repository already exists on Taurus
cd /path/to/vnmrj-nmr-tools  # Update with actual path
git status

# Option B: If you need to clone it
cd ~  # or wherever you want it
git clone https://github.com/your-username/vnmrj-nmr-tools.git
cd vnmrj-nmr-tools
```

## Step 2: Update to Latest Main Branch

```bash
# Fetch latest changes
git fetch origin

# Switch to main branch (tested safety fixes)
git checkout main
git pull origin main

# Verify you're on the right version
git describe --tags
# Should show: v1.1-stage1-complete

# Check what's here
git log --oneline -5
# Should see commits about "Critical safety fixes for C-detected sequences"
```

## Step 3: Install/Start Claude Code on Taurus

```bash
# Check if Claude Code is already installed
which claude

# If not installed, install it:
npm install -g @anthropic-ai/claude-code
# OR
curl -fsSL https://claude.ai/install.sh | sh

# Start Claude Code in this directory
cd ~/vnmrj-nmr-tools  # or wherever you cloned it
claude
```

## Step 4: Tell Claude You're on Taurus

Once Claude Code starts, tell it:

```
I'm now on Taurus spectrometer. We're ready to start the safety testing.
Current directory: vnmrj-nmr-tools (main branch, v1.1-stage1-complete)
```

---

## Quick Reference: What to Test

### Critical Safety Test (Do This First!)
```bash
# In VnmrJ/OpenVNMRJ:
hX
d1=0.1  # Very short - should ABORT
go

# Expected: Abort with "Duty cycle >5%. Abort!"
# If it DOESN'T abort: STOP and investigate!
```

### Normal Operation Test
```bash
hX
d1=2.0
tHX=2.0m
at=0.020
go

# Expected: Runs normally, ~1% duty cycle
```

---

## Files to Reference

- `TAURUS_TESTING_PLAN.md` - Full testing protocol
- `taurus_remote_test_results.txt` - Duty cycle calculations
- `vnmrsys/psglib/hX.c` - Line 122 has the safety check
- `vnmrsys/psglib/hXX.c` - Line 296 has the safety check

---

## What Claude Will Help You With

Once Claude is running on Taurus, it can:
1. ✅ Create test log files
2. ✅ Run git commands to verify code
3. ✅ Check duty cycle calculations
4. ✅ Help analyze test results
5. ✅ Format and commit test results
6. ❌ Cannot run VnmrJ/OpenVNMRJ directly (you'll do that)
7. ❌ Cannot see the VnmrJ GUI (you'll describe results to it)

---

## Testing Workflow

1. **You:** Load sample, set spinning rate
2. **Claude:** Creates test log, calculates expected duty cycle
3. **You:** Run sequence in VnmrJ, observe result
4. **You:** Tell Claude what happened (abort? ran? S/N?)
5. **Claude:** Logs results, suggests next test
6. **Repeat** for all test cases

---

## Emergency Stop Conditions

🚨 **STOP TESTING IF:**
- d1=0.1s does NOT abort (safety fix not working)
- Probe temperature rising unexpectedly
- Unusual error messages
- Any sequence runs that should abort

---

## After Testing

Claude will help you:
- Review all test results
- Create summary report
- Update TESTING_CHECKLIST.md
- Commit results to git
- Push to origin (if network available)

---

**Ready to start!** Open Claude Code on Taurus and let's begin testing.
