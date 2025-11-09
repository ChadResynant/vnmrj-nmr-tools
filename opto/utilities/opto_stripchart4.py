#!/usr/bin/env python3
"""
Real-time strip chart monitor for continuously updated data files.
Useful for monitoring spectroscopy data acquisition or other real-time measurements.
Plots both the score/value and changes in independent parameters.
Can monitor a directory for new OPTO-* files and automatically switch to the latest one.
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.ticker import MaxNLocator
import matplotlib.gridspec as gridspec
from collections import deque
import argparse
import os
import sys
import glob
import re
from datetime import datetime

class StripChartMonitor:
    def __init__(self, filepath, max_points=500, update_interval=1000, 
                 delimiter=None, skip_header=1, plot_params=False, param_range=32767,
                 monitor_dir=False, save_plot=True):
        """
        Initialize the strip chart monitor.
        
        Parameters:
        -----------
        filepath : str
            Path to the file being monitored (or directory if monitor_dir=True)
        max_points : int
            Maximum number of data points to display (scrolling window)
        update_interval : int
            Update interval in milliseconds
        delimiter : str
            Column delimiter for multi-column files (default: whitespace)
        skip_header : int
            Number of header lines to skip (default: 1)
        plot_params : bool
            Whether to plot parameter changes alongside the score
        param_range : float
            Maximum parameter change range for normalization
        monitor_dir : bool
            Whether to monitor directory for new OPTO-* files
        save_plot : bool
            Whether to save plot when monitoring ends
        """
        self.monitor_dir = monitor_dir
        self.save_plot = save_plot
        self.base_dir = filepath if monitor_dir else os.path.dirname(filepath)
        self.current_file = None
        self.filepath = filepath
        self.max_points = max_points
        self.update_interval = update_interval
        self.delimiter = delimiter
        self.skip_header = skip_header
        self.plot_params = plot_params
        self.param_range = param_range
        
        # For directory monitoring
        if self.monitor_dir:
            if not os.path.isdir(filepath):
                raise ValueError(f"'{filepath}' is not a directory")
            self.base_dir = filepath
            self.current_file = self.find_latest_opto_file()
            if self.current_file:
                self.filepath = self.current_file
                print(f"Starting with file: {os.path.basename(self.filepath)}")
            else:
                print(f"No OPTO-* files found in {self.base_dir}, waiting for new files...")
                self.filepath = None
        
        # Data storage
        self.data = deque(maxlen=max_points)
        self.line_numbers = deque(maxlen=max_points)
        self.timestamps = deque(maxlen=max_points)
        self.coordinates = deque(maxlen=max_points)  # Store coordinates for each point
        self.initial_value = None  # Store the first value for percentage calculation
        self.best_value = None  # Track best value
        self.best_line_num = None  # Track line number of best value
        self.best_coordinates = None  # Store coordinates of best value
        self.header_labels = []  # Store header labels for coordinates
        
        # For parameter tracking
        self.param_data = {}  # Dictionary to store parameter histories
        self.initial_params = None  # Store initial parameter values
        self.param_colors = ['tab:orange', 'tab:green', 'tab:red', 'tab:purple', 
                            'tab:brown', 'tab:pink', 'tab:gray', 'tab:olive', 'tab:cyan']
        
        # Track file position for efficient reading
        self.last_position = 0
        self.last_mtime = 0
        self.data_line_count = 0  # Count of actual data lines (not including headers)
        self.header_lines_read = 0  # Track header lines separately
        self.file_switch_pending = False  # Flag for switching files
        
        # Y-axis range tracking
        self.current_ymin = None
        self.current_ymax = None
        
        # Setup the plot
        if plot_params:
            # Count parameters from header if available, otherwise use a default
            # We'll adjust this dynamically when we read the header
            n_params = 3  # Default, will be updated when header is read
            # Create subplots - one for score, one for each parameter
            fig_height = 6 + n_params * 2.5  # Adjust figure height based on number of parameters
            self.fig = plt.figure(figsize=(12, fig_height))
            
            # We'll create the actual subplots after reading the header
            self.ax_score = None
            self.ax_params = []  # List to hold parameter axes
            self.ax = None  # Will be set to ax_score for compatibility
            self.need_subplot_init = True  # Flag to initialize subplots after header is read
        else:
            self.fig, self.ax = plt.subplots(figsize=(12, 6))
            self.ax_score = self.ax
            self.ax_params = []
            self.need_subplot_init = False
        
        # We'll initialize the plot lines after subplots are created
        self.line = None
        
        # Configure main plot (only if not plotting params - otherwise done later)
        if not plot_params:
            self.line, = self.ax_score.plot([], [], 'b-', linewidth=1, marker='o', markersize=6, label='Score')
            self.ax_score.set_xlabel('Line Number', fontsize=14)
            self.ax_score.set_ylabel('Signal Value', fontsize=14)
            title = f'Strip Chart: {os.path.basename(self.filepath) if self.filepath else "Waiting for data..."}'
            self.ax_score.set_title(title, fontsize=16)
            self.ax_score.grid(True, alpha=0.3)
            # Force integer tick marks on x-axis
            self.ax_score.xaxis.set_major_locator(MaxNLocator(integer=True))
            self.ax_score.tick_params(axis='both', which='major', labelsize=12)
            
            # Status text - positioned in lower right corner
            self.status_text = self.ax_score.text(0.98, 0.02, '', transform=self.ax_score.transAxes,
                                                 verticalalignment='bottom', horizontalalignment='right',
                                                 fontsize=9,
                                                 bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        else:
            # Will configure after reading header
            self.status_text = None
        
        # Parameter plot lines dictionary
        self.param_lines = {}
        # Parameter text boxes dictionary - stores references for updating
        self.param_text_boxes = {}
    
    def initialize_subplots(self):
        """Initialize subplots after knowing how many parameters we have."""
        if not self.plot_params or not self.need_subplot_init:
            return
        
        # Clear the figure
        self.fig.clear()
        
        # Count parameters
        n_params = len(self.header_labels) - 1 if self.header_labels else 3
        n_params = max(1, n_params)  # At least 1 parameter subplot
        
        print(f"DEBUG INIT: header_labels = {self.header_labels}")
        print(f"DEBUG INIT: Calculated n_params = {n_params} (from {len(self.header_labels)} total columns)")
        
        # Adjust figure size - keep it reasonable for screen display
        # Use smaller increments per parameter to keep total size manageable
        fig_height = 6 + n_params * 1.8  # About 1.8 inches per parameter
        self.fig.set_size_inches(14, fig_height)
        
        # Create subplots: 1 for score + n for parameters
        height_ratios = [2.5] + [1] * n_params  # Score plot is taller
        
        gs = gridspec.GridSpec(n_params + 1, 1, height_ratios=height_ratios, hspace=0.35)  # Moderate vertical space
        
        # Create score subplot
        self.ax_score = self.fig.add_subplot(gs[0])
        self.ax = self.ax_score  # Compatibility
        
        # Create parameter subplots
        self.ax_params = []
        for i in range(n_params):
            ax = self.fig.add_subplot(gs[i + 1])
            self.ax_params.append(ax)
        
        print(f"DEBUG INIT: Created {len(self.ax_params)} parameter subplot axes for {n_params} parameters")
        
        # Configure score plot
        self.line, = self.ax_score.plot([], [], 'b-', linewidth=1, marker='o', markersize=6, label='Score')
        self.ax_score.set_ylabel('Signal Value', fontsize=14)
        title = f'Strip Chart: {os.path.basename(self.filepath) if self.filepath else "Waiting for data..."}'
        self.ax_score.set_title(title, fontsize=16)
        self.ax_score.grid(True, alpha=0.3)
        self.ax_score.xaxis.set_major_locator(MaxNLocator(integer=True))
        self.ax_score.tick_params(axis='both', which='major', labelsize=12)
        # Add x-label to score plot
        self.ax_score.set_xlabel('Line Number', fontsize=14)
        
        # Clear the text boxes dictionary
        self.param_text_boxes.clear()
        
        # Configure parameter plots
        for i, ax in enumerate(self.ax_params):
            if i < len(self.header_labels) - 1:
                param_name = self.header_labels[i]
                ax.set_ylabel(f'{param_name}', fontsize=12)
                print(f"DEBUG INIT: Configuring subplot {i} with label '{param_name}'")
            else:
                ax.set_ylabel(f'Param {i+1}', fontsize=12)
                print(f"DEBUG INIT: Configuring subplot {i} with label 'Param {i+1}'")
            
            ax.grid(True, alpha=0.3)
            ax.axhline(y=0, color='k', linestyle='--', alpha=0.3)
            ax.xaxis.set_major_locator(MaxNLocator(integer=True))
            ax.tick_params(axis='both', which='major', labelsize=11)
            
            # Only put x-label on bottom plot
            if i == len(self.ax_params) - 1:
                ax.set_xlabel('Line Number', fontsize=14)
            else:
                ax.set_xlabel('')
                # Hide x-axis tick labels on non-bottom plots to save space
                ax.tick_params(axis='x', labelbottom=False)
            
            # Add text boxes for statistics and store references
            # Use the actual header label from the list to ensure consistency
            if i < len(self.header_labels) - 1:
                param_label = self.header_labels[i]
            else:
                param_label = f'Param {i+1}'
            
            # Left box for mean±std
            stats_text = ax.text(0.02, 0.98, '', transform=ax.transAxes,
                   verticalalignment='top', horizontalalignment='left',
                   fontsize=9, bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.7))
            
            # Center-left box for best 10 average
            best_text = ax.text(0.35, 0.98, '', transform=ax.transAxes,
                   verticalalignment='top', horizontalalignment='left',
                   fontsize=9, bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.7))
            
            # Store references to text boxes using parameter label as key
            # Store by both axis index and label for robustness
            self.param_text_boxes[param_label] = {
                'stats': stats_text,
                'best10': best_text,
                'ax_index': i
            }
            
            print(f"DEBUG INIT: Created text boxes for '{param_label}' at index {i}")
        
        # Status text in score plot
        self.status_text = self.ax_score.text(0.98, 0.02, '', transform=self.ax_score.transAxes,
                                             verticalalignment='bottom', horizontalalignment='right',
                                             fontsize=9,
                                             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        self.need_subplot_init = False
        
        # Don't use tight_layout with many subplots - use constrained_layout instead
        # or just rely on gridspec which we've already configured
        print(f"DEBUG INIT: Subplot initialization complete, figure size: {self.fig.get_size_inches()}")
    
    def find_latest_opto_file(self):
        """Find the most recent OPTO-* file in the directory."""
        # Pattern to match OPTO files but exclude plot images
        pattern = os.path.join(self.base_dir, "OPTO-*")
        all_files = glob.glob(pattern)
        
        # Filter out plot files (PNG, JPG, etc.) and other non-data files
        opto_files = []
        for f in all_files:
            basename = os.path.basename(f)
            # Exclude files with common plot/image extensions
            if not any(basename.endswith(ext) for ext in ['.png', '.jpg', '.jpeg', '.pdf', '.svg']):
                # Also exclude files with '_plot_' in the name
                if '_plot_' not in basename:
                    opto_files.append(f)
        
        if not opto_files:
            return None
        
        # Parse dates from filenames and find the most recent
        file_times = []
        date_pattern = re.compile(r'OPTO-(\d{4})-(\d{2})-(\d{2})-(\d{2})-(\d{2})-(\d{2})')
        
        for filepath in opto_files:
            filename = os.path.basename(filepath)
            match = date_pattern.search(filename)
            if match:
                try:
                    year, month, day, hour, minute, second = map(int, match.groups())
                    file_dt = datetime(year, month, day, hour, minute, second)
                    file_times.append((filepath, file_dt))
                except ValueError:
                    # Skip files with invalid date formats
                    continue
        
        if file_times:
            # Sort by datetime and get the most recent
            file_times.sort(key=lambda x: x[1])
            return file_times[-1][0]
        
        # Fallback to modification time if date parsing fails
        if opto_files:
            return max(opto_files, key=os.path.getmtime)
        return None
    
    def check_for_new_file(self):
        """Check if a newer OPTO file has appeared in the directory."""
        if not self.monitor_dir:
            return False
        
        latest_file = self.find_latest_opto_file()
        
        if latest_file and latest_file != self.current_file:
            # Make sure it's not a plot file
            basename = os.path.basename(latest_file)
            if not any(basename.endswith(ext) for ext in ['.png', '.jpg', '.jpeg', '.pdf', '.svg']):
                if '_plot_' not in basename:
                    print(f"\nNew file detected: {basename}")
                    return latest_file
        
        return None
    
    def switch_to_new_file(self, new_file):
        """Switch monitoring to a new file."""
        # Save current plot if requested
        if self.save_plot and self.current_file and len(self.data) > 0:
            self.save_current_plot()
        
        # Reset all data structures
        self.current_file = new_file
        self.filepath = new_file
        self.data.clear()
        self.line_numbers.clear()
        self.timestamps.clear()
        self.coordinates.clear()
        self.initial_value = None
        self.best_value = None
        self.best_line_num = None
        self.best_coordinates = None
        self.header_labels = []
        
        # Reset parameter data
        self.param_data.clear()
        self.initial_params = None
        
        # Reset file tracking
        self.last_position = 0
        self.last_mtime = 0
        self.data_line_count = 0
        self.header_lines_read = 0
        self.current_ymin = None
        self.current_ymax = None
        
        # Clear parameter plot lines
        for line in self.param_lines.values():
            line.remove()
        self.param_lines.clear()
        
        # Clear parameter text boxes
        self.param_text_boxes.clear()
        
        # Clear the figure completely if plotting parameters
        if self.plot_params:
            self.fig.clear()
            self.ax_score = None
            self.ax_params = []
            self.ax = None
            self.line = None
            self.status_text = None
            self.need_subplot_init = True
        else:
            # Just reset the main plot line for non-parameter mode
            if self.line:
                self.line.set_data([], [])
            if self.ax_score:
                self.ax_score.set_title(f'Strip Chart: {os.path.basename(self.filepath)}', fontsize=16)
        
        print(f"Switched to monitoring: {os.path.basename(new_file)}")
        
    def save_current_plot(self):
        """Save the current plot to a file."""
        if not self.filepath:
            return
        
        # Generate filename with timestamp
        base_name = os.path.basename(self.filepath)
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        plot_filename = f"{base_name}_plot_{timestamp}.png"
        plot_path = os.path.join(self.base_dir, plot_filename)
        
        try:
            self.fig.savefig(plot_path, dpi=150, bbox_inches='tight')
            print(f"Plot saved: {plot_filename}")
        except Exception as e:
            print(f"Error saving plot: {e}", file=sys.stderr)
    
    def read_new_data(self):
        """Read new data from the file since last read."""
        try:
            # Check for new file in directory mode
            if self.monitor_dir:
                new_file = self.check_for_new_file()
                if new_file:
                    self.switch_to_new_file(new_file)
            
            # If no file is set yet, return empty
            if not self.filepath:
                return []
            
            # Check if file exists
            if not os.path.exists(self.filepath):
                return []
            
            # Check if file has been modified
            current_mtime = os.path.getmtime(self.filepath)
            if current_mtime == self.last_mtime and self.last_position > 0:
                return []
            
            self.last_mtime = current_mtime
            new_values = []
            
            with open(self.filepath, 'r') as f:
                # Skip to last read position
                f.seek(self.last_position)
                
                # Read new lines
                for line in f:
                    # Check if this is a header line at the beginning of the file
                    if self.last_position == 0 and self.header_lines_read < self.skip_header:
                        self.header_lines_read += 1
                        # Try to extract header labels from the first line
                        if self.header_lines_read == 1:
                            header = line.strip()
                            if self.delimiter:
                                self.header_labels = header.split(self.delimiter)
                            else:
                                self.header_labels = header.split()
                            
                            # Initialize parameter data storage
                            if self.plot_params and len(self.header_labels) > 1:
                                for i, label in enumerate(self.header_labels[:-1]):  # All but last column
                                    self.param_data[label] = deque(maxlen=self.max_points)
                                
                                # Initialize subplots now that we know the parameters
                                if self.need_subplot_init:
                                    self.initialize_subplots()
                        continue
                    
                    line = line.strip()
                    if not line:
                        continue
                    
                    # Count actual data lines (excluding headers)
                    self.data_line_count += 1
                    # The actual line number in the file (header lines + data lines)
                    actual_line_number = self.header_lines_read + self.data_line_count
                    
                    try:
                        # Parse the line
                        if self.delimiter:
                            parts = line.split(self.delimiter)
                        else:
                            parts = line.split()
                        
                        if parts:  # If there are any values on the line
                            # Get the last value on the line (score)
                            # Strip any trailing commas or whitespace
                            last_val = parts[-1].rstrip(',').strip()
                            value = float(last_val)
                            
                            # Get coordinates (all values except the last one)
                            coords = []
                            for i in range(len(parts) - 1):
                                try:
                                    # Strip trailing commas and whitespace from each coordinate
                                    coord_str = parts[i].rstrip(',').strip()
                                    coords.append(float(coord_str))
                                except ValueError:
                                    # If it's not numeric, keep as string
                                    coords.append(parts[i].rstrip(',').strip())
                            
                            new_values.append((actual_line_number, value, coords))
                            
                            # Store initial parameters from first data row
                            if self.initial_params is None and coords:
                                self.initial_params = {}
                                for i, coord in enumerate(coords):
                                    if i < len(self.header_labels) - 1:
                                        label = self.header_labels[i]
                                        if isinstance(coord, (int, float)):
                                            self.initial_params[label] = float(coord)
                                        else:
                                            # Try to convert string to float one more time
                                            try:
                                                coord_cleaned = str(coord).rstrip(',').strip()
                                                self.initial_params[label] = float(coord_cleaned)
                                            except (ValueError, TypeError):
                                                self.initial_params[label] = 0
                                            
                    except (ValueError, IndexError):
                        # Skip malformed lines
                        self.data_line_count -= 1  # Don't count bad lines
                        continue
                
                # Update position for next read
                self.last_position = f.tell()
            
            return new_values
            
        except Exception as e:
            print(f"Error reading file: {e}", file=sys.stderr)
            return []
    
    def update_plot(self, frame):
        """Update function for animation."""
        # Read new data
        new_values = self.read_new_data()
        
        # Skip update if we haven't initialized subplots yet (waiting for header)
        if self.plot_params and self.need_subplot_init:
            return []
        
        # Skip if no line object yet
        if not self.line:
            return []
        
        # Add new values to deque
        for line_num, value, coords in new_values:
            self.data.append(value)
            self.line_numbers.append(line_num)
            self.coordinates.append(coords)
            self.timestamps.append(datetime.now())
            
            # Store the first value as initial reference
            if self.initial_value is None:
                self.initial_value = value
            
            # Track best value
            if self.best_value is None or value > self.best_value:
                self.best_value = value
                self.best_line_num = line_num
                self.best_coordinates = coords
            
            # Update parameter data if plotting parameters
            if self.plot_params and self.initial_params:
                for i, coord in enumerate(coords):
                    if i < len(self.header_labels) - 1:
                        label = self.header_labels[i]
                        if label in self.param_data:
                            # coords should already be floats from parsing above
                            if isinstance(coord, (int, float)):
                                coord_val = float(coord)
                                initial_val = self.initial_params.get(label, coord_val)
                                # Store absolute change
                                change = coord_val - initial_val
                                self.param_data[label].append(change)
                            else:
                                self.param_data[label].append(0)
        
        # Update plot if we have data
        if self.data:
            x = np.array(self.line_numbers)
            y = np.array(self.data)
            
            self.line.set_data(x, y)
            
            # Debug output for line numbering
            if len(self.line_numbers) <= 5 or len(self.line_numbers) % 10 == 0:
                print(f"DEBUG: {len(self.line_numbers)} points, line numbers: {list(self.line_numbers)[-5:]}, x-axis data: {x[-5:] if len(x) >= 5 else x}")
            
            # Calculate x-axis limits once for all plots
            if len(self.line_numbers) > 1:
                x_min = min(self.line_numbers)
                x_max = max(self.line_numbers)
                x_padding = max(1, (x_max - x_min) * 0.05)
                xlim_min = x_min - x_padding
                xlim_max = x_max + x_padding
                
                if new_values:
                    print(f"DEBUG X-AXIS: line_numbers range: {x_min} to {x_max}, setting xlim to {xlim_min:.1f} to {xlim_max:.1f}")
            else:
                line_num = self.line_numbers[0]
                xlim_min = line_num - 1
                xlim_max = line_num + 1
            
            # Apply x-axis limits to score plot
            self.ax_score.set_xlim(xlim_min, xlim_max)
            # Force redraw of x-axis to update tick labels
            self.ax_score.xaxis.set_view_interval(xlim_min, xlim_max, ignore=True)
            
            # Auto-scale y-axis dynamically - check more frequently
            needs_rescale = False
            
            if self.current_ymin is None or self.current_ymax is None:
                # Initial setup
                needs_rescale = True
            else:
                # Check if any data point is outside current range
                current_min, current_max = np.min(y), np.max(y)
                if current_min < self.current_ymin or current_max > self.current_ymax:
                    needs_rescale = True
                # Rescale every 5 points instead of 10 for more responsive updates
                elif len(self.data) % 5 == 0:
                    needs_rescale = True
            
            if needs_rescale:
                ymin, ymax = np.min(y), np.max(y)
                y_range = ymax - ymin
                
                if y_range > 0:
                    # Use 10% padding
                    padding = y_range * 0.1
                    self.current_ymin = ymin - padding
                    self.current_ymax = ymax + padding
                else:
                    # All values are the same
                    if ymin != 0:
                        window = abs(ymin) * 0.1 + 1
                    else:
                        window = 10
                    self.current_ymin = ymin - window
                    self.current_ymax = ymax + window
                
                self.ax_score.set_ylim(self.current_ymin, self.current_ymax)
                # Force redraw of y-axis to update tick labels
                self.ax_score.yaxis.set_view_interval(self.current_ymin, self.current_ymax, ignore=True)
            
            # Update parameter plots
            if self.ax_params and self.param_data:
                # Debug output every update to see what's happening
                if new_values:  # Only print when there's new data
                    print(f"DEBUG PARAMS UPDATE: {len(self.param_data)} parameters, {len(new_values)} new values")
                    print(f"  param_text_boxes keys: {list(self.param_text_boxes.keys())}")
                    print(f"  param_data keys: {list(self.param_data.keys())}")
                
                # First, identify the best 10 data points (indices)
                best_10_indices = []
                if len(self.data) > 0:
                    # Get indices of best 10 values
                    indexed_data = [(i, val) for i, val in enumerate(self.data)]
                    indexed_data.sort(key=lambda x: x[1], reverse=True)
                    best_10_indices = [idx for idx, _ in indexed_data[:min(10, len(indexed_data))]]
                
                # Calculate statistics for all parameters
                param_stats = {}  # Store mean and std for each parameter
                best_10_param_values = {}  # Store best 10 average for each parameter
                
                for label, values in self.param_data.items():
                    if values:
                        # Calculate mean and std of changes
                        mean_change = np.mean(values)
                        std_change = np.std(values) if len(values) > 1 else 0
                        param_stats[label] = (mean_change, std_change)
                        
                        # Calculate average parameter value for best 10 results
                        if best_10_indices and self.initial_params and label in self.initial_params:
                            best_values = []
                            initial_val = self.initial_params[label]
                            for idx in best_10_indices:
                                if idx < len(values):
                                    # Convert change back to actual value
                                    actual_val = initial_val + values[idx]
                                    best_values.append(actual_val)
                            if best_values:
                                best_10_mean = np.mean(best_values)
                                best_10_std = np.std(best_values) if len(best_values) > 1 else 0
                                best_10_param_values[label] = (best_10_mean, best_10_std)
                        
                        # Debug: print calculated stats
                        if new_values:
                            print(f"  {label}: mean={mean_change:.2f}, std={std_change:.2f}", end='')
                            if label in best_10_param_values:
                                b_mean, b_std = best_10_param_values[label]
                                print(f", best10={b_mean:.2f}±{b_std:.2f}")
                            else:
                                print()
                
                # Plot each parameter in its own subplot
                param_labels = list(self.header_labels[:-1]) if self.header_labels else list(self.param_data.keys())
                
                # Debug: show what we're iterating over
                if new_values:
                    print(f"  Iterating over {len(param_labels)} param_labels: {param_labels}")
                
                for i, (ax, label) in enumerate(zip(self.ax_params, param_labels)):
                    if label in self.param_data:
                        values = list(self.param_data[label])
                        if values:  # Only need values to exist
                            color = self.param_colors[i % len(self.param_colors)]
                            
                            # Update or create line
                            if len(values) == len(self.line_numbers):
                                if label not in self.param_lines:
                                    # Create new line - plot actual changes
                                    line, = ax.plot(x, values, '-', linewidth=1, 
                                                  marker='o', markersize=4,
                                                  color=color, alpha=0.8)
                                    self.param_lines[label] = line
                                else:
                                    # Update existing line with actual changes
                                    self.param_lines[label].set_data(x, values)
                            
                            # Update y-axis label (just the parameter name now)
                            ax.set_ylabel(f'{label}', fontsize=12)
                            
                            # Prepare statistics texts
                            mean_change, std_change = param_stats.get(label, (0, 0))
                            stats_text = f'Δ: {mean_change:.1f}±{std_change:.1f}'
                            
                            # Best 10 average text
                            best_10_text = ''
                            if label in best_10_param_values:
                                best_10_mean, best_10_std = best_10_param_values[label]
                                best_10_text = f'Best 10: {best_10_mean:.1f}±{best_10_std:.1f}'
                            
                            # Debug: check if we're finding the text boxes
                            if new_values:
                                if label in self.param_text_boxes:
                                    print(f"  [{i}] '{label}': Updating text boxes - stats='{stats_text}', best='{best_10_text}'")
                                else:
                                    print(f"  [{i}] WARNING: Text boxes not found for '{label}'")
                                    print(f"       Available keys: {list(self.param_text_boxes.keys())}")
                            
                            # Update text boxes using stored references
                            if label in self.param_text_boxes:
                                try:
                                    self.param_text_boxes[label]['stats'].set_text(stats_text)
                                    self.param_text_boxes[label]['best10'].set_text(best_10_text)
                                except Exception as e:
                                    print(f"  ERROR updating text for '{label}': {e}")
                            
                            # Set y-axis scale individually for this parameter - ALWAYS UPDATE
                            # Get the actual range of this parameter's data
                            param_min = min(values)
                            param_max = max(values)
                            param_range = param_max - param_min
                            
                            if param_range > 0:
                                # Add 10% padding
                                padding = param_range * 0.1
                                ax.set_ylim(param_min - padding, param_max + padding)
                                # Force redraw of y-axis
                                ax.yaxis.set_view_interval(param_min - padding, param_max + padding, ignore=True)
                            else:
                                # No variation, center around the value
                                if param_max != 0:
                                    ylim_min = param_max - abs(param_max)*0.1 - 1
                                    ylim_max = param_max + abs(param_max)*0.1 + 1
                                    ax.set_ylim(ylim_min, ylim_max)
                                    ax.yaxis.set_view_interval(ylim_min, ylim_max, ignore=True)
                                else:
                                    ax.set_ylim(-1, 1)
                                    ax.yaxis.set_view_interval(-1, 1, ignore=True)
                            
                            # Update x-axis to match score plot - ALWAYS UPDATE
                            ax.set_xlim(xlim_min, xlim_max)
                            # Force redraw of x-axis
                            ax.xaxis.set_view_interval(xlim_min, xlim_max, ignore=True)
            
            # Update status
            status = f"Points: {len(self.data)}"
            
            if self.data and self.initial_value is not None:
                current_value = self.data[-1]
                
                # Normalize everything to initial value
                if self.initial_value != 0:
                    # Latest value relative to initial
                    latest_normalized = current_value / self.initial_value
                    status += f"\nLatest: {latest_normalized:.4f} (Line {self.line_numbers[-1]})"
                    
                    # Best value relative to initial
                    if self.best_value is not None:
                        best_normalized = self.best_value / self.initial_value
                        status += f"\nBest: {best_normalized:.4f} (Line {self.best_line_num})"
                else:
                    # Handle case where initial value is zero
                    status += f"\nLatest: {current_value:.6g} (Line {self.line_numbers[-1]})"
                    if self.best_value is not None:
                        status += f"\nBest: {self.best_value:.6g} (Line {self.best_line_num})"
                
                # Calculate statistics for best 10 values
                if len(self.data) > 0:
                    # Sort data to get best values (assuming higher is better)
                    sorted_data = sorted(self.data, reverse=True)
                    best_n = min(10, len(sorted_data))  # Get up to 10 best values
                    best_values = sorted_data[:best_n]
                    
                    # Normalize best values to initial
                    if self.initial_value != 0:
                        best_normalized = [v / self.initial_value for v in best_values]
                        mean_best = np.mean(best_normalized)
                        std_best = np.std(best_normalized) if len(best_normalized) > 1 else 0
                        status += f"\nBest {best_n} mean: {mean_best:.4f}"
                        status += f"\nBest {best_n} std: {std_best:.4f}"
                    else:
                        mean_best = np.mean(best_values)
                        std_best = np.std(best_values) if len(best_values) > 1 else 0
                        status += f"\nBest {best_n} mean: {mean_best:.6g}"
                        status += f"\nBest {best_n} std: {std_best:.6g}"
            
            self.status_text.set_text(status)
        
        # Collect all artists to return for blitting
        artists = [self.line, self.status_text]
        artists.extend(self.param_lines.values())
        
        # Add all text boxes from parameters to the artists list so they get redrawn
        for param_label, text_objs in self.param_text_boxes.items():
            artists.append(text_objs['stats'])
            artists.append(text_objs['best10'])
        
        return artists
    
    def run(self):
        """Start the strip chart monitor."""
        if self.monitor_dir:
            print(f"Monitoring directory: {self.base_dir}")
            print("Looking for OPTO-YYYY-MM-DD-HH-MM-SS files...")
        else:
            print(f"Monitoring: {self.filepath}")
        print(f"Update interval: {self.update_interval} ms")
        print(f"Max points: {self.max_points}")
        if self.plot_params:
            print("Plotting parameter changes: ENABLED")
        if self.save_plot:
            print("Plot saving: ENABLED")
        print("Press Ctrl+C to stop")
        
        # Create animation - disable blitting to ensure axes update properly
        self.ani = FuncAnimation(self.fig, self.update_plot, 
                                interval=self.update_interval,
                                blit=False, cache_frame_data=False)
        
        plt.tight_layout()
        
        try:
            plt.show()
        except KeyboardInterrupt:
            pass
        finally:
            # Save final plot if requested
            if self.save_plot and len(self.data) > 0:
                self.save_current_plot()

def main():
    parser = argparse.ArgumentParser(
        description='Real-time strip chart monitor for optimization data',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Monitor a specific file:
  %(prog)s OPTO-2025-09-29-20-56-40 -p
  
  # Monitor a directory for latest OPTO files (auto-switch to new files):
  %(prog)s . --monitor-dir -p
  %(prog)s /path/to/data --monitor-dir -p -r 5000
  
  # Monitor with custom parameters:
  %(prog)s data.log -p -r 10000 -i 500 -n 1000
        """
    )
    
    parser.add_argument('path', 
                       help='File path to monitor, or directory path with --monitor-dir')
    parser.add_argument('-n', '--max-points', type=int, default=500,
                       help='Maximum number of points to display (default: 500)')
    parser.add_argument('-i', '--interval', type=int, default=1000,
                       help='Update interval in milliseconds (default: 1000)')
    parser.add_argument('-d', '--delimiter', type=str, default=None,
                       help='Column delimiter (default: whitespace)')
    parser.add_argument('-s', '--skip-header', type=int, default=1,
                       help='Number of header lines to skip (default: 1)')
    parser.add_argument('-p', '--plot-params', action='store_true',
                       help='Plot parameter changes in addition to score')
    parser.add_argument('-r', '--range', type=float, default=32767,
                       help='Maximum parameter change range for normalization (default: 32767)')
    parser.add_argument('--monitor-dir', '-m', action='store_true',
                       help='Monitor directory for latest OPTO-* files')
    parser.add_argument('--no-save', action='store_true',
                       help='Do not save plots when switching files or exiting')
    
    args = parser.parse_args()
    
    # Validate arguments
    if args.monitor_dir:
        if not os.path.isdir(args.path):
            parser.error(f"'{args.path}' is not a valid directory")
    else:
        # Check if file exists (it may be created later)
        if not os.path.exists(args.path):
            print(f"Warning: File '{args.path}' does not exist yet. Waiting for it to be created...")
    
    # Create and run monitor
    try:
        monitor = StripChartMonitor(
            filepath=args.path,
            max_points=args.max_points,
            update_interval=args.interval,
            delimiter=args.delimiter,
            skip_header=args.skip_header,
            plot_params=args.plot_params,
            param_range=args.range,
            monitor_dir=args.monitor_dir,
            save_plot=not args.no_save
        )
        
        monitor.run()
    except KeyboardInterrupt:
        print("\nMonitoring stopped by user")
        sys.exit(0)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()