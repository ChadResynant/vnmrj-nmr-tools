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
INTERMEDIATE_DIRS = {"lp", "ft", "ft12", "ft123", "ft1234", "smile"}
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
        file_size = os.path.getsize(file_path)
        filename = os.path.basename(file_path)
        
        # Show progress for files larger than 50MB
        if file_size > 50 * 1024 * 1024:
            print(f"  📝 Computing checksum for {filename} ({format_size(file_size)})... ", end="", flush=True)
            start_time = time.time()
        
        with open(file_path, "rb") as f:
            for chunk in iter(lambda: f.read(4096), b""):
                hash_md5.update(chunk)
        
        result = hash_md5.hexdigest()
        
        # Show completion for large files
        if file_size > 50 * 1024 * 1024:
            elapsed = time.time() - start_time
            print(f"✅ ({elapsed:.1f}s)")
            
        return result
    except Exception as e:
        if file_size > 50 * 1024 * 1024:
            print(f"❌ Error")
        return f"Error: {e}"

def get_dir_size_and_count(path):
    total_size = 0
    file_count = 0
    print(f"  📊 Analyzing directory: {os.path.basename(path)}... ", end="", flush=True)
    start_time = time.time()
    
    for dirpath, _, filenames in os.walk(path):
        for f in filenames:
            try:
                fp = os.path.join(dirpath, f)
                total_size += os.path.getsize(fp)
                file_count += 1
            except:
                continue
    
    elapsed = time.time() - start_time
    print(f"✅ {file_count} files, {format_size(total_size)} ({elapsed:.1f}s)")
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
    """Get checksums for .ucsf files (processed spectra) - works for any dataset type"""
    checksums = []
    
    if skip_md5:
        return checksums  # Skip entirely if not requested
    
    try:
        for f in os.listdir(path):
            full_path = os.path.join(path, f)
            if os.path.isfile(full_path) and f.lower().endswith('.ucsf'):
                file_size = os.path.getsize(full_path)
                if file_size > 50 * 1024 * 1024:
                    print(f"    📝 Computing checksum for UCSF file: {f} ({format_size(file_size)})")
                checksum = get_md5(full_path)
                checksums.append((f, checksum))
    except:
        pass
    
    return checksums

def get_varian_metadata_checksums(path, skip_md5=False):
    """Get checksums for important Varian metadata files (excludes fid_sorted)"""
    checksums = []
    important_files = {'fid', 'procpar'}  # Key Varian metadata files - NOT fid_sorted
    
    if skip_md5:
        return checksums  # Skip entirely if not requested
    
    try:
        for f in os.listdir(path):
            full_path = os.path.join(path, f)
            if os.path.isfile(full_path) and f in important_files:
                # Never checksum fid_sorted - it's intermediate/regenerable
                if f == 'fid_sorted':
                    continue
                checksum = get_md5(full_path)
                checksums.append((f, checksum))
    except:
        pass
    
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

def is_varian_dataset(path):
    """Check if directory is a Varian dataset (.fid extension or contains Varian files)"""
    if not os.path.isdir(path):
        return False
    
    # Check if directory name ends with .fid (common Varian convention)
    if path.endswith('.fid'):
        return True
    
    # Check for typical Varian files
    try:
        contents = os.listdir(path)
        varian_files = {'fid', 'procpar', 'text', 'log', 'global'}
        if any(f in varian_files for f in contents):
            return True
        
        # Also check for .ucsf files (processed Varian data)
        if any(f.lower().endswith('.ucsf') for f in contents):
            return True
    except:
        pass
    
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
    
    print(f"    🔍 Scanning pdata: {os.path.basename(pdata_path)}")
    
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
                print(f"      🗑️  Will remove: {filename} ({format_size(file_size)})")
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
    
    print(f"🔬 Processing Bruker dataset: {os.path.basename(dataset_path)}")
    
    try:
        contents = os.listdir(dataset_path)
    except Exception as e:
        print(f"Error reading Bruker dataset: {e}")
        return removed_dirs, removed_files, total_size
    
    # Look for numbered experiment directories
    numbered_dirs = [d for d in contents if d.isdigit() and os.path.isdir(os.path.join(dataset_path, d))]
    print(f"  📂 Found {len(numbered_dirs)} experiment directories: {', '.join(numbered_dirs)}")
    
    for exp_num in numbered_dirs:
        exp_path = os.path.join(dataset_path, exp_num)
        print(f"  🧪 Processing experiment {exp_num}")
        
        # Check for pdata subdirectories
        try:
            exp_contents = os.listdir(exp_path)
            if 'pdata' in exp_contents:
                pdata_path = os.path.join(exp_path, 'pdata')
                
                # Process each processing result directory (1, 2, 3, etc.)
                try:
                    pdata_contents = os.listdir(pdata_path)
                    proc_dirs = [d for d in pdata_contents if d.isdigit()]
                    if proc_dirs:
                        print(f"    📁 Found processing directories: {', '.join(proc_dirs)}")
                    
                    for proc_num in proc_dirs:
                        proc_path = os.path.join(pdata_path, proc_num)
                        if os.path.isdir(proc_path):
                            files, size = cleanup_bruker_pdata(proc_path, dry_run)
                            removed_files.extend(files)
                            total_size += size
                except:
                    continue
                    
            # Clean up large files in experiment directory itself
            large_files = []
            for filename in exp_contents:
                file_path = os.path.join(exp_path, filename)
                
                if not os.path.isfile(file_path):
                    continue
                    
                # Skip essential files
                if should_preserve_bruker_file(filename):
                    continue
                
                try:
                    file_size = os.path.getsize(file_path)
                    if file_size > SIZE_100MB:  # Only show progress for large files
                        large_files.append((filename, file_size))
                except:
                    continue
            
            if large_files:
                print(f"    📋 Checking {len(large_files)} large files in experiment directory")
            
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
                        print(f"      🗑️  Will remove: {filename} ({format_size(file_size)})")
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
            varian_files = [f for f in removed_files if f.get('type', '') == 'varian']
            temp_files = [f for f in removed_files if f.get('type', '') == 'temp_transfer']
            other_files = [f for f in removed_files if f.get('type', '') not in ['varian', 'temp_transfer'] and not f.get('type', '').startswith('bruker')]
            
            if bruker_files:
                log.write("  Bruker Files:\n")
                for entry in bruker_files:
                    log.write(f"   - {entry['path']} ({format_size(entry['size'])})\n")
            
            if varian_files:
                log.write("  Varian/Intermediate Files:\n")
                for entry in varian_files:
                    log.write(f"   - {entry['path']} ({format_size(entry['size'])})\n")
            
            if temp_files:
                log.write("  Temporary Transfer Files:\n")
                for entry in temp_files:
                    log.write(f"   - {entry['path']} ({format_size(entry['size'])})\n")
            
            if other_files:
                log.write("  Other Files:\n")
                for entry in other_files:
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

        if dataset_type == "varian":
            if any(name for name, _ in checksums if not name.lower().endswith('.ucsf')):
                log.write("Varian Metadata Checksums:\n")
                for name, checksum in checksums:
                    if not name.lower().endswith('.ucsf'):
                        log.write(f" - {name}:  {checksum}\n")
                log.write("\n")
        
        # UCSF files can appear in any dataset type
        ucsf_checksums = [item for item in checksums if item[0].lower().endswith('.ucsf')]
        if ucsf_checksums:
            log.write("UCSF Spectrum File Checksums:\n")
            for name, checksum in ucsf_checksums:
                log.write(f" - {name}:  {checksum}\n")
            log.write("\n")

        log.write(f"Total {'simulated' if dry_run else 'actual'} space savings: {format_size(total_size)}\n")
        log.write("\nEnd of log.\n")

    return log_filename

def cleanup_dataset_dir(dir_path, dry_run=False, force_recent=False, compute_checksums=False):
    """Main cleanup function that detects dataset type and applies appropriate cleanup"""
    now = time.time()
    removed_dirs = []
    removed_files = []
    total_size = 0
    dataset_type = "unknown"

    # Make args accessible for force flag
    global args_global
    if 'args_global' not in globals():
        args_global = type('Args', (), {'force': force_recent})()
    else:
        args_global.force = force_recent

    # Detect dataset type
    if is_bruker_dataset(dir_path):
        dataset_type = "bruker"
        print(f"🔬 Detected Bruker dataset: {dir_path}")
        removed_dirs, removed_files, total_size = cleanup_bruker_dataset(dir_path, dry_run)
        scripts = get_script_files(dir_path, skip_md5=not compute_checksums)
        ucsf_checksums = get_ucsf_checksums(dir_path, skip_md5=not compute_checksums)
        varian_checksums = get_varian_metadata_checksums(dir_path, skip_md5=not compute_checksums) if compute_checksums else []
        
    elif is_varian_dataset(dir_path):
        dataset_type = "varian"
        print(f"🔬 Detected Varian dataset: {dir_path}")
        
        # Original UCSF/Varian cleanup logic (keeping your existing logic)
        try:
            filelist = os.listdir(dir_path)
            has_ucsf = any(
                f.lower().endswith(".ucsf") and os.path.isfile(os.path.join(dir_path, f))
                for f in filelist
            )
        except Exception as e:
            print(f"Error reading directory: {dir_path}\n{e}")
            return

        # Directories - improved intermediate directory cleanup logic
        print(f"  📂 Checking for intermediate directories: {', '.join(INTERMEDIATE_DIRS)}")
        for sub in filelist:
            sub_path = os.path.join(dir_path, sub)
            if not os.path.isdir(sub_path) or sub.lower() == "data":
                continue
            
            # Check if directory matches intermediate patterns
            is_intermediate = False
            matched_pattern = ""
            
            # Exact match
            if sub.lower() in INTERMEDIATE_DIRS:
                is_intermediate = True
                matched_pattern = f"exact match: {sub.lower()}"
            else:
                # Check for patterns like ft123_OW, lp_CMR, smile_processing, etc.
                for pattern in INTERMEDIATE_DIRS:
                    if sub.lower().startswith(pattern + "_") or sub.lower().startswith(pattern + "-"):
                        is_intermediate = True
                        matched_pattern = f"pattern match: {pattern}_* or {pattern}-*"
                        break
            
            if is_intermediate:
                print(f"    📁 Found intermediate directory: {sub} ({matched_pattern})")
                try:
                    age = now - os.path.getmtime(sub_path)
                    count, size = get_dir_size_and_count(sub_path)
                    
                    # Decision logic for directory cleanup
                    should_remove = False
                    reason = ""
                    
                    if age > CUTOFF_30_DAYS:
                        should_remove = True
                        reason = f"older than 30 days ({age/86400:.1f} days)"
                    elif age > CUTOFF_7_DAYS and size > SIZE_1GB:
                        should_remove = True
                        reason = f"older than 7 days ({age/86400:.1f} days) and large ({format_size(size)})"
                    elif hasattr(args_global, 'force') and args_global.force:
                        # Force flag: clean up ANY intermediate directory regardless of age/size
                        should_remove = True
                        reason = f"forced cleanup ({age/86400:.1f} days old, {format_size(size)})"
                    elif age <= CUTOFF_7_DAYS:
                        # Young directory - show what would happen with force
                        print(f"      ✅ Keeping recent directory: {sub} ({format_size(size)}, {age/86400:.1f} days old)")
                        if size > SIZE_1GB:
                            print(f"         💡 Use --force to clean this large recent directory")
                        else:
                            print(f"         💡 Use --force to clean this directory regardless of age/size")
                    
                    if should_remove:
                        print(f"      🗑️  Will remove directory: {sub} ({count} files, {format_size(size)}) - {reason}")
                        removed_dirs.append({"path": sub_path, "count": count, "size": size})
                        total_size += size
                        if not dry_run:
                            shutil.rmtree(sub_path)
                        
                except Exception as e:
                    print(f"Error accessing {sub_path}: {e}")
            else:
                # Debug: show directories that weren't matched
                if any(pattern in sub.lower() for pattern in INTERMEDIATE_DIRS):
                    print(f"    ❓ Directory {sub} contains intermediate keywords but wasn't matched")

        # Files - restore original Varian cleanup logic
        for f in filelist:
            f_path = os.path.join(dir_path, f)
            if not os.path.isfile(f_path):
                continue
            
            # Check for intermediate files to remove
            should_remove = False
            
            if (f.startswith("fid_") and f != "fid"):
                should_remove = True
            elif f == "fid_sorted":  # Large sorted FID files
                should_remove = True
            elif f.lower().endswith(".ft123") or f.lower().endswith(".ft1234"):
                should_remove = True
            elif "ft" in f.lower() and any(f.lower().endswith(ext) for ext in [".ft", ".ft12"]):
                should_remove = True
                
            # Check for intermediate files to remove
            should_remove = False
            
            if (f.startswith("fid_") and f != "fid"):
                should_remove = True
            elif f == "fid_sorted":  # Large sorted FID files - always remove when found
                should_remove = True
            elif f.lower().endswith((".ft12", ".ft123", ".ft1234")):
                should_remove = True
            elif "ft" in f.lower() and any(f.lower().endswith(ext) for ext in [".ft"]):
                should_remove = True
                
            if should_remove:
                try:
                    age = now - os.path.getmtime(f_path)
                    size = os.path.getsize(f_path)
                    
                    # fid_sorted files: always remove regardless of age if force flag is used
                    if f == "fid_sorted" and hasattr(args_global, 'force') and args_global.force:
                        print(f"      🗑️  Will remove file: {f} ({format_size(size)}) - forced removal of regenerable file")
                        removed_files.append({"path": f_path, "size": size, "type": "varian"})
                        total_size += size
                        if not dry_run:
                            os.remove(f_path)
                    # Other files: use normal age/size criteria
                    elif age > CUTOFF_30_DAYS or (age > CUTOFF_7_DAYS and size > SIZE_1GB):
                        print(f"      🗑️  Will remove file: {f} ({format_size(size)})")
                        removed_files.append({"path": f_path, "size": size, "type": "varian"})
                        total_size += size
                        if not dry_run:
                            os.remove(f_path)
                    elif f == "fid_sorted":
                        print(f"      ⚠️  Large fid_sorted file found: {f} ({format_size(size)}, {age/86400:.1f} days old)")
                        print(f"         💡 Use --force to remove fid_sorted files regardless of age")
                except:
                    continue
                    
        scripts = get_script_files(dir_path, skip_md5=not compute_checksums)
        ucsf_checksums = get_ucsf_checksums(dir_path, skip_md5=not compute_checksums)
        varian_checksums = []  # No Varian-specific metadata for Bruker
        
    else:
        print(f"⚠️  Unknown dataset type: {dir_path}")
        print("    (No Bruker acqus files or Varian .fid directories/files detected)")
        return

    # Generate log and summary
    timestamp = datetime.now().strftime("%Y-%m-%dT%H%M")
    
    # Combine checksums for logging
    all_checksums = ucsf_checksums + varian_checksums
    
    log_path = write_cleanup_log(dir_path, removed_dirs, removed_files, scripts, all_checksums, total_size, dry_run, timestamp, dataset_type)

    print(f"\n{'✅ Dry-run' if dry_run else '🧹 Cleanup'} completed for: {dir_path}")
    print(f"Dataset type: {dataset_type.upper()}")
    print(f"{'Simulated' if dry_run else 'Actual'} deletions: {len(removed_dirs)} directories, {len(removed_files)} files")
    print(f"📦 {'Estimated' if dry_run else 'Total'} space savings: {format_size(total_size)}")
    print(f"Log file written to: {log_path}")
    
    if compute_checksums:
        if ucsf_checksums:
            print(f"📝 UCSF checksums computed for {len(ucsf_checksums)} spectrum files")
        if varian_checksums:
            print(f"📝 Varian metadata checksums computed for {len(varian_checksums)} files")
        if not ucsf_checksums and not varian_checksums:
            print(f"📝 No checksum files found in this directory")
    elif any(f.lower().endswith('.ucsf') for f in os.listdir(dir_path)):
        print(f"💡 Use --checksums flag to compute UCSF file verification checksums")
    
    print()  # Add blank line for readability

# --- Main with wildcard support ---
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Cleanup intermediate NMR files for Bruker and Varian formats.",
        epilog="""
Examples:
  python cleanup.py /path/to/data/*.fid --dry-run    # Varian datasets
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
    parser.add_argument("--force", action="store_true",
                       help="Force cleanup of ALL intermediate directories regardless of age or size")
    parser.add_argument("--checksums", action="store_true",
                       help="Compute checksums for important files (slow for large files)")
    parser.add_argument("--varian-only", action="store_true",
                       help="Only process directories detected as Varian datasets")
    parser.add_argument("--ucsf-only", action="store_true",
                       help="Only process directories detected as UCSF datasets (deprecated, use --varian-only)")
    
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
                if (args.varian_only or args.ucsf_only) and not is_varian_dataset(path):
                    print(f"Skipping (not Varian): {path}")
                    continue
                    
                cleanup_dataset_dir(path, dry_run=args.dry_run, force_recent=args.force, compute_checksums=args.checksums)
                processed += 1
            else:
                print(f"Skipping (not a directory): {path}")
                
        print(f"\n🎯 Processed {processed} dataset directories")
