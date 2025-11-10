# VnmrJ Template XML Syntax Guide

## Overview

VnmrJ templates use XML with embedded VnmrJ command syntax in certain attributes. This guide documents the correct syntax patterns, common pitfalls, and best practices based on our template modernization effort.

## Critical Syntax Rules

### 1. HTML Entity Escaping in XML Attributes

**Rule**: All comparison operators in XML attributes MUST be HTML-escaped.

**Correct:**
```xml
show="$SHOW=(panellevel&gt;0)"
show="$SHOW=(dfrq3&gt;0)"
show="if (value&lt;100) then $SHOW=1 endif"
```

**Incorrect:**
```xml
show="$SHOW=(panellevel>0)"      <!-- XML parser error! -->
show="$SHOW=(dfrq3>0)"            <!-- Will break template rendering -->
```

**Required Escapes:**
- `>` → `&gt;`
- `<` → `&lt;`
- `&` → `&amp;` (if needed in literal text)
- `"` → `&quot;` (if needed within strings)

### 2. String Comparison Syntax

**Rule**: String equality comparisons require explicit if-then-else syntax. The shorthand `$SHOW=(string='value')` does NOT work for string comparisons.

**Correct:**
```xml
show="if Hseq='tppm' then $SHOW=1 else $SHOW=0 endif"
show="if echo='soft' then $SHOW=1 else $SHOW=0 endif"
show="if ((echo='hard')or(echo='soft')) then $SHOW=1 else $SHOW=0 endif"
```

**Incorrect:**
```xml
show="$SHOW=(Hseq='tppm')"        <!-- DOES NOT WORK! -->
show="$SHOW=(echo='soft')"         <!-- DOES NOT WORK! -->
```

**Note**: The shorthand syntax `$SHOW=(condition)` ONLY works for numeric comparisons with escaped operators.

### 3. Not-Equal Comparisons

**Rule**: Use `&lt;&gt;` for not-equal comparisons, not `!=`.

**Correct:**
```xml
show="if dn2&lt;&gt;'' then $SHOW=1 else $SHOW=0 endif"
show="if value&lt;&gt;0 then $SHOW=1 else $SHOW=0 endif"
```

**Incorrect:**
```xml
show="if dn2!='' then $SHOW=1 else $SHOW=0 endif"   <!-- Wrong operator! -->
show="$SHOW=(dn2!='')"                              <!-- Double wrong! -->
```

### 4. Combined Show and Label

**Rule**: You can set both `$SHOW` and `$LABEL` in a single if-then-else expression.

**Correct:**
```xml
show="if dn2&lt;&gt;'' then $SHOW=1 $LABEL='Decoupler 2 ('+dn2+')' else $SHOW=0 endif"
show="$LABEL='Transmitter ('+tn+')'"  <!-- Label only, always shown -->
```

**Pattern**: Multiple statements can be combined with space separation in then/else clauses.

## Common Patterns

### Numeric Comparisons

```xml
<!-- Panel level check -->
show="$SHOW=(panellevel&gt;0)"
show="$SHOW=(panellevel&gt;29)"

<!-- Frequency check (channel exists) -->
show="$SHOW=(dfrq3&gt;0)"
show="if dfrq3&gt;0 then $SHOW=1 else $SHOW=0 endif"  <!-- Equivalent -->

<!-- Range checks -->
show="if (value&gt;0)and(value&lt;100) then $SHOW=1 else $SHOW=0 endif"
```

### String Equality

```xml
<!-- Single string check -->
show="if Hseq='tppm' then $SHOW=1 else $SHOW=0 endif"
show="if cp='y' then $SHOW=1 else $SHOW=0 endif"

<!-- Multiple options (OR) -->
show="if ((echo='hard')or(echo='soft')) then $SHOW=1 else $SHOW=0 endif"
show="if ((Hseq='tppm')or(Hseq='spinal')) then $SHOW=1 else $SHOW=0 endif"
```

### Empty String Checks

```xml
<!-- Check if string parameter is non-empty -->
show="if dn2&lt;&gt;'' then $SHOW=1 else $SHOW=0 endif"

<!-- With dynamic label -->
show="if dn2&lt;&gt;'' then $SHOW=1 $LABEL='Decoupler 2 ('+dn2+')' else $SHOW=0 endif"
```

### Dynamic Labels

```xml
<!-- Label changes based on parameter value -->
show="$LABEL='Transmitter ('+tn+')'"
show="$LABEL='Decoupler 1 ('+dn+')'"
show="if dn2&lt;&gt;'' then $LABEL='Decoupler 2 ('+dn2+')' endif"
```

## Value Conversion Patterns

### Unit Conversion (time parameters)

```xml
<!-- Display milliseconds, store seconds -->
<entry
  vq="at"
  vc="at=$VALUE*0.001"          <!-- Convert ms input to seconds -->
  set="$VALUE=at*1000"           <!-- Display seconds as ms -->
  digits="2"
  />
<label label="ms" />

<!-- Unit menu for user preference -->
<menu
  vq="at"
  vc="at=$VALUE"
  set="$VALUE=at"
  >
  <mlabel label="ms" chval="at*1000" />
  <mlabel label="sec" chval="at" />
</menu>
```

### Parameter Units with parunits

```xml
<!-- Proper parameter unit handling -->
<entry
  vq="param"
  vc="parunits('set','param','$VALUE')"
  set="$VALUE=param"
  />

<menu
  vq="param"
  vc="parunits('set','param','$VALUE')"
  set="$VALUE=param"
  >
  <mlabel label="option1" chval="value1" />
  <mlabel label="option2" chval="value2" />
</menu>
```

## Debugging Templates

### Validation Checklist

1. **XML Well-formedness**: Run `xmllint --noout template.xml`
2. **Comparison Operators**: Search for unescaped `>`, `<` in show attributes
3. **String Comparisons**: Check for `$SHOW=(string='value')` patterns
4. **Not-Equal Operator**: Search for `!=` usage
5. **Quote Balancing**: Ensure all quotes are properly paired

### Testing in VnmrJ

1. Load the sequence that uses the template
2. Check that all tabs appear in the GUI
3. Verify conditional sections show/hide correctly
4. Test parameter changes trigger correct visibility updates
5. Check for console error messages

### Common Error Symptoms

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Tab doesn't appear | `panellevel>0` not escaped | Change to `panellevel&gt;0` |
| Conditional group doesn't hide | Incorrect string comparison syntax | Use if-then-else for strings |
| Template fails to load | XML syntax error | Run xmllint to find the error |
| Parameter not updating label | Missing `$LABEL=` statement | Add label assignment in show attribute |

## Template File Structure

### Standard 4-Tab Layout

Modern solid-state NMR templates use a consistent 4-tab structure:

```
sequence/
├── acq.xml              # Defines tab structure
├── Acquisition.xml      # Tab 1: Basic parameters, sample, receiver
├── PulseSequence.xml    # Tab 2: CP, mixing, sequence control
├── Decoupling.xml       # Tab 3: H/X/Y/Z decoupling parameters
└── Calibration.xml      # Tab 4: Power levels, pulse widths
```

### acq.xml Template

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<template name="acq" element="pages" type="acquisition" >
  <group size="650 285"
    style="PlainText"
    vq="panellevel"
    show="$SHOW=(panellevel&gt;0)"
    >
    <page title="Acquisition"
      use="vnmrsys/templates/layout/sequence/Acquisition.xml"
      />
    <page title="Pulse Sequence"
      use="vnmrsys/templates/layout/sequence/PulseSequence.xml"
      />
    <page title="Decoupling"
      use="vnmrsys/templates/layout/sequence/Decoupling.xml"
      />
    <page title="Calibration"
      use="vnmrsys/templates/layout/sequence/Calibration.xml"
      />
  </group>
</template>
```

## Best Practices

### 1. Consistency

- Use the same pattern for similar operations across all templates
- Place panellevel checks at the top-level group
- Group related parameters together

### 2. User Experience

- Show estimated experiment time: `(nt*(d1+at))/3600` hours
- Display duty cycle warnings for safety
- Use dynamic labels with nucleus names: `'Transmitter ('+tn+')'`
- Convert time parameters to ms for better readability

### 3. Safety Features

```xml
<!-- Duty cycle display with warning -->
<textmessage
  vq="tRF d1"
  set="if (d1&gt;0) then $VALUE=(tRF/d1)*100 else $VALUE=0 endif"
  digits="2"
  />
<label label="%" />

<textmessage
  vq="tRF d1"
  set="if (d1&gt;0) then $dc=(tRF/d1)*100 if ($dc&gt;25) then $safe15=tRF/0.15 $VALUE='Safe d1 ≥ '+$safe15+' s (15%)' else $VALUE='(target &lt; 25%)' endif else $VALUE='(target &lt; 25%)' endif"
  />
```

### 4. Documentation

- Add XML comments explaining complex sections
- Document parameter meanings in tooltips
- Include examples in template headers

## References

- **Working Examples**: See `vnmrsys/templates/layout/hX/` for complete modern templates
- **Validation Script**: Run `vnmrsys/validate_refactoring.sh` to check syntax
- **VnmrJ Documentation**: Varian/Agilent template documentation (if available)

## Revision History

- **2025-11-09**: Initial version documenting template syntax fixes
  - Fixed unescaped comparison operators in hX templates
  - Corrected string comparison syntax
  - Added comprehensive validation checks
