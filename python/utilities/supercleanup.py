#!/usr/bin/env python3
import os
import hashlib
import time
import argparse
import glob
from datetime import datetime
import shutil
import re

# --- Config ---
INTERMEDIATE_DIRS = {"lp", "ft", "ft12", "ft123", "smile"}
SIZE_1GB = 1 * 1024 * 1024 * 1024
SIZE_100MB = 100 * 1024 * 1024
CUTOFF_30_DAYS = 30 * 86400
CUTOFF_7_DAYS = 7 * 86400

# Bruker-specific large files that can be safely removed
BRUKER_LARGE_FILES = {
    '2rr', '2ri', '2ir', '2ii',  # 2D processed data
    '1r', '1i',                  # 1D processed data
    'outd',                      # output data
    'fid.refscan',              # reference scan data
    'ser.refscan'               # reference scan data
}

# Bruker files to always preserve (critical metadata/parameters)
BRUKER_PRESERVE = {
    'acqus', 'acqu2s', 'acqu3s',     # acquisition parameters
    'procs', 'proc2s', 'proc3s',     # processing parameters
    'pulseprogram',                   # pulse sequence
    'spnam*', 'cpd*',                # shaped pulses
    'shimvalues',                     # shim values
    'prosol_History',                 # processing history
    'ased',                          # automatic shimming
    'audita.txt',                    # audit trail
    'format.temp', 'format.ased'     # format files
}

def get_md5(file_path):
    hash_md5 = hashlib.md5()
    try:
        with open(file_path, "rb") as f:
            for chunk in iter(lambda: f.read(4096), b""):
                hash_md5.update(chunk)
        return hash_md5.hexdigest()
    except Exception as e:
        return f"Error: {e}"

def get_dir_size_and_count(path):
    total_size = 0
    file_count = 0
    for dirpath, _, filenames in os.walk(path):
        for f in filenames:
            try:
                fp = os.path.join(dirpath, f)
                total_size += os.path.getsize(fp)
                file_count += 1
            except:
                continue
    return file_count, total_size

def get_script_files(path, skip_md5=False):
    scripts = []
    for f in os.listdir(path):
        if f.endswith(".sh") or f.endswith(".com"):
            full_path = os.path.join(path, f)
            try:
                mod_time = time.ctime(os.path.getmtime(full_path))
                checksum = "SKIPPED" if skip_md5 else get_md5(full_path)
                scripts.append((f, mod_time, checksum))
            except:
                continue
    return scripts

def get_ucsf_checksums(path, skip_md5=False):
    checksums = []
    for f in os.listdir(path):
        full_path = os.path.join(path, f)
        if os.path.isfile(full_path) and f.lower().endswith(".ucsf"):
            checksum = "SKIPPED" if skip_md5 else get_md5(full_path)
            checksums.append((f, checksum))
    return checksums

def is_bruker_dataset(path):
    """Check if directory is a Bruker dataset (contains acqus file or numbered experiment dirs)"""
    if not os.path.isdir(path):
        return False
    
    # Check for acqus file (TopSpin dataset)
    if os.path.exists(os.path.join(path, 'acqus')):
        return True
    
    # Check for numbered experiment directories
    try:
        contents = os.listdir(path)
        numbered_dirs = [d for d in contents if d.isdigit() and os.path.isdir(os.path.join(path, d))]
        if numbered_dirs:
            # Check if any numbered dir contains acqus
            for num_dir in numbered_dirs[:3]:  # Check first few
                if os.path.exists(os.path.join(path, num_dir, 'acqus')):
                    return True
    except:
        pass
    
    return False

def is_ucsf_dataset(path):
    """Check if directory contains UCSF format files"""
    try:
        return any(f.lower().endswith('.ucsf') for f in os.listdir(path))
    except:
        return False

def should_preserve_bruker_file(filename):
    """Check if a Bruker file should be preserved"""
    filename_lower = filename.lower()
    
    # Exact matches
    if filename in BRUKER_PRESERVE:
        return True
    
    # Pattern matches
    for pattern in BRUKER_PRESERVE:
        if '*' in pattern:
            pattern_regex = pattern.replace('*', '.*')
            if re.match(pattern_regex, filename, re.IGNORECASE):
                return True
    
    return False

def cleanup_bruker_pdata(pdata_path, dry_run=False):
    """Clean up a Bruker pdata directory, preserving essential files"""
    removed_files = []
    total_size = 0
    now = time.time()
    
    if not os.path.isdir(pdata_path):
        return removed_files, total_size
    
    try:
        files = os.listdir(pdata_path)
    except:
        return removed_files, total_size
    
    for filename in files:
        file_path = os.path.join(pdata_path, filename)
        
        if not os.path.isfile(file_path):
            continue
            
        # Skip if it's a file we should preserve
        if should_preserve_bruker_file(filename):
            continue
            
        try:
            file_size = os.path.getsize(file_path)
            file_age = now - os.path.getmtime(file_path)
            
            # Remove large processed data files
            should_remove = False
            
            if filename in BRUKER_LARGE_FILES:
                should_remove = True
            elif file_size > SIZE_100MB and file_age > CUTOFF_7_DAYS:
                should_remove = True
            elif file_age > CUTOFF_30_DAYS and file_size > SIZE_1GB:
                should_remove = True
            
            if should_remove:
                removed_files.append({
                    "path": file_path,
                    "size": file_size,
                    "type": "bruker_pdata"
                })
                total_size += file_size
                
                if not dry_run:
                    os.remove(file_path)
                    
        except Exception as e:
            print(f"Error processing {file_path}: {e}")
            
    return removed_files, total_size

def cleanup_bruker_dataset(dataset_path, dry_run=False):
    """Clean up a Bruker dataset directory"""
    removed_dirs = []
    removed_files = []
    total_size = 0
    now = time.time()
    
    print(f"Processing Bruker dataset: {dataset_path}")
    
    try:
        contents = os.listdir(dataset_path)
    except Exception as e:
        print(f"Error reading Bruker dataset: {e}")
        return removed_dirs, removed_files, total_size
    
    # Look for numbered experiment directories
    numbered_dirs = [d for d in contents if d.isdigit() and os.path.isdir(os.path.join(dataset_path, d))]
    
    for exp_num in numbered_dirs:
        exp_path = os.path.join(dataset_path, exp_num)
        
        # Check for pdata subdirectories
        try:
            exp_contents = os.listdir(exp_path)
            if 'pdata' in exp_contents:
                pdata_path = os.path.join(exp_path, 'pdata')
                
                # Process each processing result directory (1, 2, 3, etc.)
                try:
                    pdata_contents = os.listdir(pdata_path)
                    for proc_num in pdata_contents:
                        if proc_num.isdigit():
                            proc_path = os.path.join(pdata_path, proc_num)
                            if os.path.isdir(proc_path):
                                files, size = cleanup_bruker_pdata(proc_path, dry_run)
                                removed_files.extend(files)
                                total_size += size
                except:
                    continue
                    
            # Clean up large files in experiment directory itself
            for filename in exp_contents:
                file_path = os.path.join(exp_path, filename)
                
                if not os.path.isfile(file_path):
                    continue
                    
                # Skip essential files
                if should_preserve_bruker_file(filename):
                    continue
                    
                try:
                    file_size = os.path.getsize(file_path)
                    file_age = now - os.path.getmtime(file_path)
                    
                    # Remove large fid files from old experiments or temp files
                    should_remove = False
                    
                    if filename == 'fid' and file_size > SIZE_1GB and file_age > CUTOFF_30_DAYS:
                        should_remove = True
                    elif filename == 'ser' and file_size > SIZE_1GB and file_age > CUTOFF_30_DAYS:
                        should_remove = True
                    elif filename.startswith('fid.') and file_age > CUTOFF_7_DAYS:
                        should_remove = True
                    elif filename.startswith('ser.') and file_age > CUTOFF_7_DAYS:
                        should_remove = True
                    elif file_size > SIZE_1GB and file_age > CUTOFF_7_DAYS:
                        should_remove = True
                        
                    if should_remove:
                        removed_files.append({
                            "path": file_path,
                            "size": file_size,
                            "type": "bruker_exp"
                        })
                        total_size += file_size
                        
                        if not dry_run:
                            os.remove(file_path)
                            
                except Exception as e:
                    print(f"Error processing {file_path}: {e}")
                    
        except:
            continue
    
    return removed_dirs, removed_files, total_size

def format_size(size_bytes):
    """Format file size in human-readable units"""
    if size_bytes >= 1024**3:  # GB
        return f"{size_bytes / (1024**3):.2f} GB"
    elif size_bytes >= 1024**2:  # MB
        return f"{size_bytes / (1024**2):.1f} MB"
    elif size_bytes >= 1024:  # KB
        return f"{size_bytes / 1024:.1f} KB"
    else:
        return f"{size_bytes} bytes"

def write_cleanup_log(dir_path, removed_dirs, removed_files, scripts, checksums, total_size, dry_run, timestamp, dataset_type="unknown"):
    log_type = "DRYRUN" if dry_run else "CLEANUP"
    log_filename = os.path.join(dir_path, f"cleanup_log_{timestamp}_{log_type}.txt")
    
    with open(log_filename, "w") as log:
        log.write(f"{'🚫 DRY RUN' if dry_run else '🧹 REAL CLEANUP'} - {timestamp}\n")
        log.write(f"Dataset Type: {dataset_type.upper()}\n")
        log.write("="*40 + "\n\n")

        if removed_dirs:
            log.write("Removed Directories:\n")
            for entry in removed_dirs:
                log.write(f" - {entry['path']} ({entry['count']} files, {format_size(entry['size'])})\n")
        else:
            log.write("Removed Directories: None\n")
        log.write("\n")

        if removed_files:
            log.write("Removed Files:\n")
            # Group by type for better organization
            bruker_files = [f for f in removed_files if f.get('type', '').startswith('bruker')]
            ucsf_files = [f for f in removed_files if f.get('type', '') == 'ucsf' or 'type' not in f]
            
            if bruker_files:
                log.write("  Bruker Files:\n")
                for entry in bruker_files:
                    log.write(f"   - {entry['path']} ({format_size(entry['size'])})\n")
            
            if ucsf_files:
                log.write("  UCSF/Other Files:\n")
                for entry in ucsf_files:
                    log.write(f"   - {entry['path']} ({format_size(entry['size'])})\n")
        else:
            log.write("Removed Files: None\n")
        log.write("\n")

        log.write("Preserved Processing Scripts:\n")
        for name, mod_time, checksum in scripts:
            log.write(f" - {name} (modified: {mod_time}, MD5: {checksum})\n")
        if not scripts:
            log.write(" - None\n")
        log.write("\n")

        if dataset_type == "ucsf":
            log.write("UCSF File Checksums:\n")
            for name, checksum in checksums:
                log.write(f" - {name}:  {checksum}\n")
            if not checksums:
                log.write(" - None\n")
            log.write("\n")

        log.write(f"Total {'simulated' if dry_run else 'actual'} space savings: {format_size(total_size)}\n")
        log.write("\nEnd of log.\n")

    return log_filename

def cleanup_dataset_dir(dir_path, dry_run=False):
    """Main cleanup function that detects dataset type and applies appropriate cleanup"""
    now = time.time()
    removed_dirs = []
    removed_files = []
    total_size = 0
    dataset_type = "unknown"

    # Detect dataset type
    if is_bruker_dataset(dir_path):
        dataset_type = "bruker"
        print(f"🔬 Detected Bruker dataset: {dir_path}")
        removed_dirs, removed_files, total_size = cleanup_bruker_dataset(dir_path, dry_run)
        scripts = get_script_files(dir_path, skip_md5=dry_run)
        checksums = []  # Bruker doesn't use UCSF checksums
        
    elif is_ucsf_dataset(dir_path):
        dataset_type = "ucsf"
        print(f"🔬 Detected UCSF dataset: {dir_path}")
        
        # Original UCSF cleanup logic
        try:
            filelist = os.listdir(dir_path)
            has_ucsf = any(
                f.lower().endswith(".ucsf") and os.path.isfile(os.path.join(dir_path, f))
                for f in filelist
            )
        except Exception as e:
            print(f"Error reading directory: {dir_path}\n{e}")
            return

        # Directories
        for sub in filelist:
            sub_path = os.path.join(dir_path, sub)
            if not os.path.isdir(sub_path) or sub.lower() == "data":
                continue
            if sub.lower() in INTERMEDIATE_DIRS:
                try:
                    age = now - os.path.getmtime(sub_path)
                    count, size = get_dir_size_and_count(sub_path)
                    if has_ucsf and (age > CUTOFF_30_DAYS or (age > CUTOFF_7_DAYS and size > SIZE_1GB)):
                        removed_dirs.append({"path": sub_path, "count": count, "size": size})
                        total_size += size
                        if not dry_run:
                            shutil.rmtree(sub_path)
                except Exception as e:
                    print(f"Error accessing {sub_path}: {e}")

        # Files
        for f in filelist:
            f_path = os.path.join(dir_path, f)
            if not os.path.isfile(f_path):
                continue
            if (f.startswith("fid_") and f != "fid") or f.lower().endswith(".ft123") or "ft" in f.lower():
                try:
                    age = now - os.path.getmtime(f_path)
                    size = os.path.getsize(f_path)
                    if age > CUTOFF_30_DAYS or (age > CUTOFF_7_DAYS and size > SIZE_1GB):
                        removed_files.append({"path": f_path, "size": size, "type": "ucsf"})
                        total_size += size
                        if not dry_run:
                            os.remove(f_path)
                except:
                    continue
                    
        scripts = get_script_files(dir_path, skip_md5=dry_run)
        checksums = get_ucsf_checksums(dir_path, skip_md5=dry_run)
        
    else:
        print(f"⚠️  Unknown dataset type: {dir_path}")
        print("    (No Bruker acqus files or UCSF files detected)")
        return

    # Generate log and summary
    timestamp = datetime.now().strftime("%Y-%m-%dT%H%M")
    log_path = write_cleanup_log(dir_path, removed_dirs, removed_files, scripts, checksums, total_size, dry_run, timestamp, dataset_type)

    print(f"\n{'✅ Dry-run' if dry_run else '🧹 Cleanup'} completed for: {dir_path}")
    print(f"Dataset type: {dataset_type.upper()}")
    print(f"{'Simulated' if dry_run else 'Actual'} deletions: {len(removed_dirs)} directories, {len(removed_files)} files")
    print(f"📦 {'Estimated' if dry_run else 'Total'} space savings: {format_size(total_size)}")
    print(f"Log file written to: {log_path}\n")

# --- Main with wildcard support ---
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Cleanup intermediate NMR files for both UCSF and Bruker formats.",
        epilog="""
Examples:
  python cleanup.py /path/to/data/*.fid --dry-run    # UCSF datasets
  python cleanup.py /path/to/bruker/*/[0-9]*         # Bruker experiment dirs
  python cleanup.py /path/to/nmr/data/*              # Auto-detect format
        """
    )
    parser.add_argument("dataset_dirs", nargs="+", 
                       help="Path(s) or wildcards for NMR directories (.fid for UCSF, experiment dirs for Bruker)")
    parser.add_argument("--dry-run", action="store_true", 
                       help="Simulate deletion without removing files")
    parser.add_argument("--bruker-only", action="store_true",
                       help="Only process directories detected as Bruker datasets")
    parser.add_argument("--ucsf-only", action="store_true",
                       help="Only process directories detected as UCSF datasets")
    
    args = parser.parse_args()

    expanded_paths = []
    for pattern in args.dataset_dirs:
        matches = glob.glob(pattern)
        expanded_paths.extend(matches)

    if not expanded_paths:
        print("No matching NMR directories found.")
    else:
        processed = 0
        for path in sorted(expanded_paths):
            if os.path.isdir(path):
                # Apply type filters if specified
                if args.bruker_only and not is_bruker_dataset(path):
                    print(f"Skipping (not Bruker): {path}")
                    continue
                if args.ucsf_only and not is_ucsf_dataset(path):
                    print(f"Skipping (not UCSF): {path}")
                    continue
                    
                cleanup_dataset_dir(path, dry_run=args.dry_run)
                processed += 1
            else:
                print(f"Skipping (not a directory): {path}")
                
        print(f"\n🎯 Processed {processed} dataset directories")
