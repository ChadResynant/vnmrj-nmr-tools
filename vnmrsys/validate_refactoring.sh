#!/bin/bash

# Validation script for NMR refactoring stages
# Run after each stage completes to verify changes

echo "========================================"
echo "NMR Refactoring Validation Report"
echo "Date: $(date)"
echo "========================================"
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Base directory
BASEDIR="/home/chad/github-staging/vnmrj-nmr-tools/vnmrsys"

# Stage 1 Validation: Check critical C-detected sequences for 5% duty cycle
echo "STAGE 1: Critical Safety Fixes"
echo "-------------------------------"
echo "Checking C-detected sequences for 5% duty cycle..."
echo ""

stage1_sequences=("hYXX.c" "hYXX_S.c" "hYXXsoft.c" "hhYXX_S.c" "hXYXX_4D_S.c")
stage1_pass=true

for seq in "${stage1_sequences[@]}"; do
    if [ -f "$BASEDIR/psglib/$seq" ]; then
        # Look for duty cycle setting
        duty_check=$(grep -E "(set_duty_limit|duty.*=.*0\.[0-9]+|DUTY_LIMIT.*0\.[0-9]+)" "$BASEDIR/psglib/$seq" 2>/dev/null | head -1)

        if [[ $duty_check == *"0.05"* ]]; then
            echo -e "${GREEN}✓${NC} $seq: 5% duty cycle found"
        elif [[ $duty_check == *"0.1"* ]] || [[ $duty_check == *"0.10"* ]]; then
            echo -e "${RED}✗${NC} $seq: DANGEROUS 10% duty cycle detected!"
            stage1_pass=false
        elif [[ $duty_check == *"0.2"* ]] || [[ $duty_check == *"0.20"* ]]; then
            echo -e "${RED}✗${NC} $seq: DANGEROUS 20% duty cycle detected!"
            stage1_pass=false
        else
            echo -e "${YELLOW}?${NC} $seq: Could not determine duty cycle"
        fi
    else
        echo -e "${YELLOW}?${NC} $seq: File not found"
    fi
done

echo ""
if $stage1_pass; then
    echo -e "${GREEN}Stage 1 PASSED: All critical sequences fixed${NC}"
else
    echo -e "${RED}Stage 1 FAILED: Critical safety issues remain!${NC}"
fi
echo ""

# Stage 2 Validation: Check for new architecture files
echo "STAGE 2: PulSeq-Inspired Architecture"
echo "--------------------------------------"
echo "Checking for architecture components..."
echo ""

architecture_files=(
    "psg/nmr_system_limits.h"
    "psg/nmr_make_functions.h"
    "psg/nmr_make_functions.c"
    "psg/nmr_validation.h"
    "psg/nmr_validation.c"
)

stage2_count=0
for file in "${architecture_files[@]}"; do
    if [ -f "$BASEDIR/$file" ]; then
        echo -e "${GREEN}✓${NC} Found: $file"
        ((stage2_count++))
    else
        echo -e "${YELLOW}○${NC} Missing: $file"
    fi
done

echo ""
if [ $stage2_count -eq ${#architecture_files[@]} ]; then
    echo -e "${GREEN}Stage 2 COMPLETE: All architecture files created${NC}"
elif [ $stage2_count -gt 0 ]; then
    echo -e "${YELLOW}Stage 2 PARTIAL: $stage2_count/${#architecture_files[@]} files created${NC}"
else
    echo -e "Stage 2 NOT STARTED: No architecture files found"
fi
echo ""

# Stage 3 Validation: Check for refactored sequences
echo "STAGE 3: High-Impact Sequence Refactoring"
echo "-----------------------------------------"
echo "Checking for refactored sequences..."
echo ""

# Check if refactored versions exist
refactored_sequences=("hX" "hXH" "mtune" "Htune")
stage3_count=0

for seq in "${refactored_sequences[@]}"; do
    # Check for various possible refactored versions
    if [ -f "$BASEDIR/psglib/${seq}_refactored.c" ]; then
        echo -e "${GREEN}✓${NC} Found: ${seq}_refactored.c"
        ((stage3_count++))
    elif [ -f "$BASEDIR/psglib/${seq}_new.c" ]; then
        echo -e "${GREEN}✓${NC} Found: ${seq}_new.c"
        ((stage3_count++))
    else
        # Check if original was modified recently (within last hour)
        if [ -f "$BASEDIR/psglib/${seq}.c" ]; then
            if [ $(find "$BASEDIR/psglib/${seq}.c" -mmin -60 | wc -l) -gt 0 ]; then
                echo -e "${GREEN}✓${NC} Recently modified: ${seq}.c"
                ((stage3_count++))
            else
                echo -e "${YELLOW}○${NC} Not refactored: ${seq}.c"
            fi
        else
            echo -e "${YELLOW}○${NC} Not found: ${seq}.c"
        fi
    fi
done

echo ""
if [ $stage3_count -eq ${#refactored_sequences[@]} ]; then
    echo -e "${GREEN}Stage 3 COMPLETE: All sequences refactored${NC}"
elif [ $stage3_count -gt 0 ]; then
    echo -e "${YELLOW}Stage 3 PARTIAL: $stage3_count/${#refactored_sequences[@]} sequences refactored${NC}"
else
    echo -e "Stage 3 NOT STARTED: No sequences refactored"
fi
echo ""

# Stage 4 Validation: Check for testing and documentation
echo "STAGE 4: Testing and Documentation"
echo "----------------------------------"
echo "Checking for test suite and documentation..."
echo ""

test_docs=(
    "tests/test_framework.sh"
    "MIGRATION_GUIDE.md"
    "CHANGELOG.md"
)

stage4_count=0
for file in "${test_docs[@]}"; do
    if [ -f "$BASEDIR/$file" ]; then
        echo -e "${GREEN}✓${NC} Found: $file"
        ((stage4_count++))
    else
        echo -e "${YELLOW}○${NC} Missing: $file"
    fi
done

echo ""
if [ $stage4_count -eq ${#test_docs[@]} ]; then
    echo -e "${GREEN}Stage 4 COMPLETE: Testing and documentation ready${NC}"
elif [ $stage4_count -gt 0 ]; then
    echo -e "${YELLOW}Stage 4 PARTIAL: $stage4_count/${#test_docs[@]} items completed${NC}"
else
    echo -e "Stage 4 NOT STARTED: No testing/documentation found"
fi
echo ""

# Overall Summary
echo "========================================"
echo "OVERALL STATUS SUMMARY"
echo "========================================"
echo ""

# Count warnings and errors
warnings=$(grep -c "YELLOW" /tmp/validation_$$.log 2>/dev/null || echo 0)
errors=$(grep -c "RED" /tmp/validation_$$.log 2>/dev/null || echo 0)

echo "Safety Critical Issues:"
if ! $stage1_pass; then
    echo -e "${RED}⚠ CRITICAL: Some C-detected sequences still have dangerous duty cycles!${NC}"
    echo -e "${RED}  DO NOT USE THESE SEQUENCES UNTIL FIXED${NC}"
else
    echo -e "${GREEN}✓ All critical safety issues resolved${NC}"
fi
echo ""

echo "Architecture Status:"
echo "  Stage 1 (Safety): $(if $stage1_pass; then echo -e "${GREEN}COMPLETE${NC}"; else echo -e "${RED}INCOMPLETE${NC}"; fi)"
echo "  Stage 2 (Architecture): $stage2_count/${#architecture_files[@]} files"
echo "  Stage 3 (Refactoring): $stage3_count/${#refactored_sequences[@]} sequences"
echo "  Stage 4 (Testing): $stage4_count/${#test_docs[@]} items"
echo ""

# Template Syntax Validation
echo "TEMPLATE SYNTAX VALIDATION"
echo "--------------------------"
echo "Checking VnmrJ template XML syntax..."
echo ""

template_errors=0
template_warnings=0

# Check for unescaped comparison operators
unescaped_gt=$(find "$BASEDIR/templates" -name "*.xml" -exec grep -l 'show=".*[^&]>[0-9]' {} \; 2>/dev/null | wc -l)
if [ $unescaped_gt -gt 0 ]; then
    echo -e "${RED}✗${NC} Found $unescaped_gt files with unescaped > operator"
    find "$BASEDIR/templates" -name "*.xml" -exec grep -l 'show=".*[^&]>[0-9]' {} \; 2>/dev/null | sed 's|^|  |'
    ((template_errors++))
else
    echo -e "${GREEN}✓${NC} No unescaped > operators in show attributes"
fi

# Check for incorrect string comparison syntax
incorrect_string_comp=$(find "$BASEDIR/templates" -name "*.xml" -exec grep -l 'show=".*\$SHOW=([A-Za-z]*=' {} \; 2>/dev/null | wc -l)
if [ $incorrect_string_comp -gt 0 ]; then
    echo -e "${RED}✗${NC} Found $incorrect_string_comp files with incorrect string comparison syntax"
    find "$BASEDIR/templates" -name "*.xml" -exec grep -l 'show=".*\$SHOW=([A-Za-z]*=' {} \; 2>/dev/null | sed 's|^|  |'
    ((template_errors++))
else
    echo -e "${GREEN}✓${NC} All string comparisons use correct if-then-else syntax"
fi

# Check for use of != instead of <>
incorrect_ne=$(find "$BASEDIR/templates" -name "*.xml" -exec grep -l "show=.*!=" {} \; 2>/dev/null | wc -l)
if [ $incorrect_ne -gt 0 ]; then
    echo -e "${RED}✗${NC} Found $incorrect_ne files using != instead of &lt;&gt;"
    find "$BASEDIR/templates" -name "*.xml" -exec grep -l "show=.*!=" {} \; 2>/dev/null | sed 's|^|  |'
    ((template_errors++))
else
    echo -e "${GREEN}✓${NC} No files using != operator (should use &lt;&gt;)"
fi

# Validate XML syntax
echo ""
echo "Checking XML well-formedness..."
xml_invalid=0
for xmlfile in $(find "$BASEDIR/templates" -name "*.xml" 2>/dev/null); do
    if command -v xmllint &> /dev/null; then
        if ! xmllint --noout "$xmlfile" 2>/dev/null; then
            echo -e "${RED}✗${NC} Invalid XML: $xmlfile"
            ((xml_invalid++))
        fi
    fi
done

if [ $xml_invalid -eq 0 ]; then
    if command -v xmllint &> /dev/null; then
        echo -e "${GREEN}✓${NC} All XML files are well-formed"
    else
        echo -e "${YELLOW}○${NC} xmllint not available, skipping XML validation"
    fi
else
    echo -e "${RED}✗${NC} Found $xml_invalid invalid XML files"
    ((template_errors++))
fi

echo ""
if [ $template_errors -eq 0 ]; then
    echo -e "${GREEN}Template Syntax PASSED: All templates have correct VnmrJ syntax${NC}"
else
    echo -e "${RED}Template Syntax FAILED: $template_errors syntax issues found!${NC}"
    echo -e "${RED}Templates with syntax errors will not display correctly in VnmrJ${NC}"
fi
echo ""

# Git status
echo "Git Status:"
git_status=$(cd $BASEDIR && git status --porcelain | wc -l)
if [ $git_status -gt 0 ]; then
    echo -e "${YELLOW}  $git_status uncommitted changes${NC}"
    echo "  Run 'git status' to review changes"
    echo "  Run 'git diff' to see modifications"
else
    echo -e "${GREEN}  All changes committed${NC}"
fi
echo ""

echo "========================================"
echo "Validation complete: $(date)"
echo "========================================"