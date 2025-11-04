#!/usr/bin/env python3
import os
import shutil
import re
import datetime
from pathlib import Path

# Configuration
SOURCE_DIR = os.path.expanduser("~/nmrdata/fyddle/2025/")
TARGET_DIR = os.path.expanduser("~/nmrdata/fyddle/2025/organized")
# Date pattern to extract from filenames (will customize based on your format)
DATE_PATTERN = r"(\d{4})(\d{2})(\d{2})"  # Example: YYYYMMDD

def extract_date_from_filename(filename):
    """Extract date from filename using regex pattern."""
    match = re.search(DATE_PATTERN, filename)
    if match:
        year, month, day = match.groups()
        return datetime.date(int(year), int(month), int(day))
    return None

def identify_file_group(filepath):
    """Identify which group a file belongs to."""
    # This will be customized based on your grouping criteria
    # For example, files might be grouped by common prefix or experiment number
    filename = os.path.basename(filepath)
    # Example: extract experiment ID or sample name
    # group_id = re.search(r"sample_(\w+)", filename).group(1)
    # return group_id
    pass

def main():
    # Create target directory if it doesn't exist
    os.makedirs(TARGET_DIR, exist_ok=True)
    
    # Dictionary to store files by date and group
    files_by_date_and_group = {}
    
    # Scan all files in source directory
    for root, dirs, files in os.walk(SOURCE_DIR):
        for filename in files:
            filepath = os.path.join(root, filename)
            
            # Extract date from filename
            file_date = extract_date_from_filename(filename)
            if not file_date:
                # Handle files without date information
                continue
            
            # Identify group
            group_id = identify_file_group(filepath)
            
            # Add to dictionary
            date_str = file_date.strftime("%Y-%m-%d")
            key = (date_str, group_id)
            if key not in files_by_date_and_group:
                files_by_date_and_group[key] = []
            files_by_date_and_group[key].append(filepath)
    
    # Organize files by copying to target location with new structure
    for (date_str, group_id), filepaths in files_by_date_and_group.items():
        # Create directory structure: TARGET_DIR/YYYY-MM-DD/group_id/
        target_group_dir = os.path.join(TARGET_DIR, date_str, group_id)
        os.makedirs(target_group_dir, exist_ok=True)
        
        # Copy files to target location
        for filepath in filepaths:
            filename = os.path.basename(filepath)
            target_path = os.path.join(target_group_dir, filename)
            shutil.copy2(filepath, target_path)  # copy2 preserves metadata
            print(f"Copied: {filepath} → {target_path}")

if __name__ == "__main__":
    main()