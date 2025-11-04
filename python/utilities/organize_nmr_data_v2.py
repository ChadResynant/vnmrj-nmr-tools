#!/usr/bin/env python3
import os
import shutil
import re
import datetime
from pathlib import Path
import argparse
import sys

# Configuration
SOURCE_DIR = os.path.expanduser("~/nmrdata/fyddle/2025/")
TARGET_DIR = os.path.expanduser("~/nmrdata/fyddle_organized/2025/")

# Constants for organization
GROUP_SIZE = 99  # Number of spectra per group folder (001to099, 100to199, etc.)
MONTHS = {
    1: 'JAN', 2: 'FEB', 3: 'MAR', 4: 'APR', 5: 'MAY', 6: 'JUN',
    7: 'JUL', 8: 'AUG', 9: 'SEP', 10: 'OCT', 11: 'NOV', 12: 'DEC'
}

def get_nmr_acquisition_date(fid_dir):
    """
    Attempt to extract the acquisition date from a Varian .fid directory.
    First tries to read from procpar file, falls back to directory modification time.
    """
    procpar_path = os.path.join(fid_dir, 'procpar')
    acqfil_path = os.path.join(fid_dir, 'acqfil')
    log_path = os.path.join(fid_dir, 'log')
    
    # Try to parse date from procpar if it exists
    if os.path.exists(procpar_path):
        try:
            # Read first few lines of procpar looking for date information
            with open(procpar_path, 'r') as f:
                content = f.read(4000)  # Read first 4KB
                # Look for date patterns in the file
                date_match = re.search(r'date\s+\d+\s+\d+\s+"([^"]+)"', content)
                if date_match:
                    date_str = date_match.group(1)
                    try:
                        # Handle various date formats in Varian files
                        # Format like "Jun 15 2023"
                        return datetime.datetime.strptime(date_str, "%b %d %Y").date()
                    except ValueError:
                        pass
        except (IOError, UnicodeDecodeError):
            pass
    
    # Try to use file modification time as fallback
    try:
        for check_path in [procpar_path, acqfil_path, log_path, fid_dir]:
            if os.path.exists(check_path):
                mtime = os.path.getmtime(check_path)
                return datetime.datetime.fromtimestamp(mtime).date()
    except:
        pass
    
    # Final fallback: use today's date
    print(f"Warning: Could not determine acquisition date for {fid_dir}, using current date")
    return datetime.date.today()

def extract_spectrum_number(fid_name):
    """
    Extract the spectrum number from a .fid directory name.
    Examples: 001_hX.fid -> 1, 002_1D.fid -> 2, 123.fid -> 123
    """
    # Remove .fid extension
    base_name = fid_name.replace('.fid', '')
    
    # Try to extract the number prefix
    match = re.match(r'^(\d+)', base_name)
    if match:
        return int(match.group(1))
    
    return None

def determine_range_folder(spectrum_number):
    """
    Determine the range folder name for a spectrum number.
    For example: 37 -> '001to099', 123 -> '100to199'
    """
    if spectrum_number is None:
        return "unknown"
        
    start = ((spectrum_number - 1) // GROUP_SIZE) * GROUP_SIZE + 1
    end = start + GROUP_SIZE - 1
    return f"{start:03d}to{end:03d}"

def get_month_year_folder(date_obj):
    """
    Convert date to MONTHYEAR format (e.g., OCT5 for October 2025)
    Uses last digit of the year
    """
    month_str = MONTHS[date_obj.month]
    year_digit = str(date_obj.year)[-1]  # Get last digit of year
    return f"{month_str}{year_digit}"

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Organize NMR data by date and spectrum number")
    parser.add_argument("--dry-run", action="store_true", help="Show what would happen without actually copying files")
    parser.add_argument("--source", help="Source directory (overrides the script's SOURCE_DIR)")
    parser.add_argument("--target", help="Target directory (overrides the script's TARGET_DIR)")
    args = parser.parse_args()
    
    # Override directories if specified on command line
    source_dir = args.source if args.source else SOURCE_DIR
    target_dir = args.target if args.target else TARGET_DIR
    
    # Validate source directory
    if not os.path.exists(source_dir):
        print(f"Error: Source directory does not exist: {source_dir}")
        sys.exit(1)
    
    print(f"{'DRY RUN - ' if args.dry_run else ''}Organizing NMR data")
    print(f"Source: {source_dir}")
    print(f"Target: {target_dir}")
    
    # Create target directory if it doesn't exist and not in dry run mode
    if not args.dry_run:
        try:
            os.makedirs(target_dir, exist_ok=True)
        except PermissionError:
            print(f"Error: Permission denied when creating directory: {target_dir}")
            sys.exit(1)
    
    # Find all .fid directories
    fid_dirs = []
    try:
        for root, dirs, _ in os.walk(source_dir):
            for dir_name in dirs:
                if dir_name.endswith('.fid'):
                    fid_dir_path = os.path.join(root, dir_name)
                    fid_dirs.append(fid_dir_path)
    except PermissionError as e:
        print(f"Error: Permission denied when scanning directories: {str(e)}")
        sys.exit(1)
    
    print(f"Found {len(fid_dirs)} .fid directories to organize")
    
    # Process each .fid directory
    organized_count = 0
    for fid_dir_path in fid_dirs:
        try:
            fid_name = os.path.basename(fid_dir_path)
            
            # Get acquisition date
            acq_date = get_nmr_acquisition_date(fid_dir_path)
            
            # Extract spectrum number
            spectrum_number = extract_spectrum_number(fid_name)
            
            # Determine target directory structure
            year_folder = str(acq_date.year)
            month_year_folder = get_month_year_folder(acq_date)
            range_folder = determine_range_folder(spectrum_number)
            
            # Build target path
            target_subdir = os.path.join(year_folder, month_year_folder, range_folder)
            full_target_dir = os.path.join(target_dir, target_subdir)
            target_path = os.path.join(full_target_dir, fid_name)
            
            # Check if target already exists
            if os.path.exists(target_path):
                print(f"Warning: Target already exists, skipping: {target_path}")
                continue
                
            print(f"{'DRY RUN: Would organize' if args.dry_run else 'Organizing'}: {fid_dir_path} -> {target_path}")
            
            if not args.dry_run:
                try:
                    # Create target directory
                    os.makedirs(full_target_dir, exist_ok=True)
                    
                    # Copy the directory
                    shutil.copytree(fid_dir_path, target_path)
                    organized_count += 1
                except PermissionError:
                    print(f"Error: Permission denied when copying {fid_dir_path} to {target_path}")
                except Exception as e:
                    print(f"Error copying {fid_dir_path}: {str(e)}")
            else:
                # In dry run mode, just count it as organized
                organized_count += 1
            
        except Exception as e:
            print(f"Error processing {fid_dir_path}: {str(e)}")
    
    print(f"\n{'DRY RUN - ' if args.dry_run else ''}Organization complete.")
    print(f"{'Would organize' if args.dry_run else 'Organized'} {organized_count} out of {len(fid_dirs)} .fid directories.")
    print(f"Target directory: {target_dir}")
    
    if args.dry_run:
        print("\nThis was a dry run. No files were actually copied.")
        print("To perform the actual organization, run without the --dry-run flag")

if __name__ == "__main__":
    main()