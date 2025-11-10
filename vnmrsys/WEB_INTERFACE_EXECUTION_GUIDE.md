# Web Interface Execution Guide for NMR Refactoring

## Optimal Strategy for Claude Code Web Interface

### Key Principles for Web Interface

1. **Use Clear, Self-Contained Prompts**
   - Each task should have ALL necessary context
   - Don't assume memory between sessions
   - Include file paths and specific requirements

2. **Leverage Parallel Execution**
   - Web interface handles multiple agents well
   - Group independent tasks for parallel execution
   - Use "Run agents in parallel" explicitly

3. **Provide Context Files**
   - Reference SAFETY_STANDARDS.md for all safety work
   - Reference REFACTORING_IMPLEMENTATION_PLAN.md for roadmap
   - Include specific file paths in prompts

## Staged Execution Plan

### Stage 1: Fix 5 Critical C-Detected Sequences [URGENT - Do First]

**Copy this exact prompt to web interface:**

```
Please fix critical safety issues in 5 C-detected NMR sequences that are using dangerous 10% duty cycles. Run agents in parallel to fix these sequences:

Context files:
- Safety standards: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/SAFETY_STANDARDS.md
- Required updates: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/SAFETY_UPDATES_REQUIRED.md

Fix these sequences in /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/:
1. hYXX.c - Change duty cycle from 10% to 5%
2. hYXX_S.c - Change duty cycle from 10% to 5%
3. hYXXsoft.c - Change duty cycle from 10% to 5%
4. hhYXX_S.c - Change duty cycle from 10% to 5%
5. hXYXX_4D_S.c - Investigate 20% usage, change to 5% unless documented exception

Each fix should:
- Locate duty cycle setting (usually set_duty_limit or similar)
- Change to 0.05 (5%) for C-detected sequences
- Add comment explaining: "// 5% duty cycle for C-detected (high-power decoupling) per SAFETY_STANDARDS.md"
- Report what was changed

These are CRITICAL safety fixes to prevent probe damage.
```

### Stage 2: Create PulSeq-Inspired Architecture [After Stage 1]

**Copy this exact prompt to web interface:**

```
Create PulSeq-inspired architecture for NMR sequences. Run agents in parallel for these three tasks:

Working directory: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/
Reference: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/REFACTORING_IMPLEMENTATION_PLAN.md (Phase 2)

Task 1: Create NMRSystemLimits structure
- Create psg/nmr_system_limits.h
- Define structure with RF limits, gradient limits, probe limits
- Include channel-specific limits (obs, dec, dec2)
- Add probe-specific duty cycle limits
- Follow structure in REFACTORING_IMPLEMENTATION_PLAN.md Section 2.1

Task 2: Implement make functions pattern
- Create psg/nmr_make_functions.h and .c
- Implement: makePulse(), makeShapedPulse(), makeCP(), makeMixing(), makeDecoupling()
- Each function validates parameters at creation time
- Return validated pulse/block structures
- Follow pattern in REFACTORING_IMPLEMENTATION_PLAN.md Section 2.2

Task 3: Build validation framework
- Create psg/nmr_validation.h and .c
- Implement three-tier validation: creation, sequence, runtime
- Include power-dependent duty cycle calculation
- Add clear error messages with solutions
- Follow SAFETY_STANDARDS.md validation requirements

All code should be production-ready with comprehensive documentation.
```

### Stage 3: Refactor High-Impact Sequences [After Stage 2]

**Copy this exact prompt to web interface:**

```
Refactor the highest-impact NMR sequences covering 50% of all usage. Run agents in parallel for these sequence families:

Working directory: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/
Use new architecture from: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psg/
Reference safety: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/SAFETY_STANDARDS.md

Task 1: Refactor hX family (28% of usage, 16,915 experiments)
- Refactor hX.c using new architecture
- Maintain 5% duty cycle for C-detected
- Use NMRSystemLimits and make functions
- Preserve all functionality
- Create hX_test.c for validation

Task 2: Refactor hXH family (10% of usage, 5,808 experiments)
- Refactor hXH.c using new architecture
- Implement power-dependent duty cycle (15% for low-power H)
- Use new validation framework
- Optimize for H-detected experiments
- Create hXH_test.c for validation

Task 3: Refactor calibration sequences
- Refactor mtune.c (3,052 uses)
- Refactor Htune.c (1,835 uses)
- These are critical for daily operations
- May have special duty cycle requirements for calibration

Each refactored sequence should:
1. Use new NMRSystemLimits structure
2. Use make functions for all pulses
3. Implement three-tier validation
4. Maintain backward compatibility
5. Reduce code by >40% through reuse
```

### Stage 4: Testing and Documentation [After Stage 3]

**Copy this exact prompt to web interface:**

```
Create comprehensive testing framework and documentation for refactored sequences. Run agents in parallel:

Working directory: /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/

Task 1: Build automated test suite
- Create tests/test_framework.sh
- Test parameter ranges for all refactored sequences
- Verify duty cycle calculations
- Check phase cycling correctness
- Validate timing accuracy
- Generate test report

Task 2: Create migration documentation
- Create MIGRATION_GUIDE.md
- Document changes from original to refactored sequences
- Include parameter mapping
- Add troubleshooting section
- Create quick reference card

Task 3: Update repository documentation
- Update main README.md with refactoring status
- Create CHANGELOG.md with all changes
- Update sequence documentation headers
- Add performance benchmarks
- Document known issues and solutions
```

## Best Practices for Web Interface

### DO:
- ✅ Use "Run agents in parallel" for independent tasks
- ✅ Provide complete file paths
- ✅ Include context files in prompts
- ✅ Ask for progress reports
- ✅ Request git commits after each stage

### DON'T:
- ❌ Assume context from previous messages
- ❌ Use relative paths without base directory
- ❌ Skip safety validation steps
- ❌ Deploy without testing

## Monitoring Progress

After each stage, ask Claude to:
1. Summarize what was completed
2. Show git diff of changes
3. Run validation tests
4. Commit with descriptive message
5. Report any issues found

## Example Follow-up Prompts

After Stage 1:
```
Show me the git diff for all safety fixes. Then commit the changes with a descriptive message about fixing critical duty cycle issues.
```

After Stage 2:
```
Demonstrate the new architecture by showing how to create a simple CP sequence using the make functions. Include validation.
```

After Stage 3:
```
Compare the original hX.c with the refactored version. Show code reduction metrics and verify backward compatibility.
```

## Emergency Rollback

If issues arise:
```
git reset --hard v1.0-safety-fixes
```

This returns to the tagged safe state we just created.

## Success Criteria

Each stage should achieve:
1. **Stage 1**: All 5 sequences at safe 5% duty cycle
2. **Stage 2**: Complete architecture in psg/ directory
3. **Stage 3**: 3 sequence families refactored, >40% code reduction
4. **Stage 4**: All tests passing, documentation complete

## Timeline Estimate

- Stage 1: 15-20 minutes (critical fixes)
- Stage 2: 30-45 minutes (architecture)
- Stage 3: 45-60 minutes (sequence refactoring)
- Stage 4: 30-45 minutes (testing/docs)

Total: 2-3 hours of web interface time

---

**Ready to execute via web interface!**
Copy each stage prompt exactly as shown above.