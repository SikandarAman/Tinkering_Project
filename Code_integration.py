import pandas as pd
import numpy as np
from Outlier_detection import Outlier
from Rolling_mean import RollingMean
from FFT import PhysiologicalSignalAnalyzer

# Step 1: Load the data
data = pd.read_csv('mitbih_test.csv')

# Initialize helper classes
outlier = Outlier()
rolling_mean = RollingMean(window_size=20, age=20)  # window_size and age chosen per prior setup
fft = PhysiologicalSignalAnalyzer(sampling_rate=125)  # ECG sampled at 125 Hz

# Sampling parameters
sampling_rate = 125  # Hz (samples per second)
interval = 1 / sampling_rate  # seconds between two samples

# Step 2: Select a single ECG recording and simulate continuous input
data_values = data.iloc[10].values  # pick one ECG record
long_data_values = np.tile(data_values, 30)  # repeat the record to simulate longer signal

# Step 3: Create heart rate data (simulating streaming HR signal)
hr_data = []
activity_list = []
for value in long_data_values:
    hr_data.append(value)
    rolling_mean.add_data_point(value)
    activity = rolling_mean.classify_activity()
    if activity not in activity_list:
        activity_list.append(activity)

print("Detected Activities:", activity_list)

# Step 4: Detect outliers in HR data
outlier.update_stats(hr_data)
outliers_list = outlier.detect_outliers(hr_data)
print("Outliers detected:", outliers_list)

# Get all outlier indices
outlier_indices = set(idx for idx, _ in outliers_list)

# Remove outliers from hr_data
filtered_hr_data = [val for idx, val in enumerate(hr_data) if idx not in outlier_indices]

# Now use filtered_hr_data for HRV feature calculation
hr_data_np = np.array(filtered_hr_data)* 1000
valid_hr_data_np = hr_data_np[hr_data_np > 0]  # additionally remove any 0 bpm just to be safe
filtered_hr_data = [val for idx, val in enumerate(valid_hr_data_np) if idx not in outlier_indices and 30 <= val <= 220]

if len(filtered_hr_data) == 0:
    print("No valid heart rate data after removing outliers.")
else:
    rr_intervals = filtered_hr_data

# Step 5: Frequency-based HRV Analysis
hrv_freq = fft.analyze_heart_rate_variability(hr_data)
print("Frequency domain HRV metrics:", hrv_freq)

# AVNN: Average of NN intervals
AVNN = np.mean(rr_intervals)

# SDNN: Standard deviation of NN intervals
SDNN = np.std(rr_intervals, ddof=1)

# RMSSD: Root mean square of successive differences
diff_rr_intervals = np.diff(rr_intervals)
RMSSD = np.sqrt(np.mean(diff_rr_intervals**2))

# pNN50: Percentage of RR differences > 50 ms
nn50 = np.sum(np.abs(diff_rr_intervals) > 50)
pNN50 = (nn50 / len(diff_rr_intervals)) * 100

# SDANN: Standard deviation of averages of NN intervals over 0.1-min windows
samples_per_min = 1 * 10 * sampling_rate  # 1 min × 10 seconds × 125 samples/sec
num_segments = len(rr_intervals) // samples_per_min

means_per_segment = []
for i in range(num_segments):
    segment = rr_intervals[i * samples_per_min : (i + 1) * samples_per_min]
    if len(segment) > 0:
        means_per_segment.append(np.mean(segment))

if len(means_per_segment) > 1:
    SDANN = np.std(means_per_segment, ddof=1)
else:
    SDANN = np.nan  # Not enough data

# Step 7: Print Final HRV Features
print(f"AVNN: {AVNN:.2f} ms")
print(f"SDNN: {SDNN:.2f} ms")
print(f"RMSSD: {RMSSD:.2f} ms")
print(f"pNN50: {pNN50:.2f} %")
print(f"SDANN: {SDANN:.2f} ms")
