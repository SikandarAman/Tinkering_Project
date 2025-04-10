import numpy as np
from scipy.signal import detrend

class PhysiologicalSignalAnalyzer:
    def __init__(self, sampling_rate):
        """
        Initialize the analyzer with the sampling rate of the signals.

        Parameters:
        - sampling_rate (float): Sampling rate in Hz (samples per second).
        """
        self.sampling_rate = sampling_rate
        self.heart_rate_data = []
        self.oxygen_saturation_data = []

    def add_heart_rate_data(self, data):
        """
        Add heart rate data to the analyzer.

        Parameters:
        - data (list or np.array): Heart rate data points.
        """
        self.heart_rate_data.extend(data)

    def add_oxygen_saturation_data(self, data):
        """
        Add oxygen saturation data to the analyzer.

        Parameters:
        - data (list or np.array): Oxygen saturation data points.
        """
        self.oxygen_saturation_data.extend(data)

    def compute_fft(self, data):
        """
        Compute the FFT of the provided data.

        Parameters:
        - data (list or np.array): The input signal data.

        Returns:
        - freqs (np.array): Array of frequency bins.
        - power_spectrum (np.array): Power spectrum of the signal.
        """
        n = len(data)
        detrended_data = detrend(data)  # Remove linear trend
        fft_result = np.fft.rfft(detrended_data)
        power_spectrum = np.abs(fft_result) ** 2
        freqs = np.fft.rfftfreq(n, d=1/self.sampling_rate)
        return freqs, power_spectrum

    def analyze_heart_rate_variability(self):
        """
        Analyze heart rate variability using FFT.

        Returns:
        - results (dict): Dictionary containing frequency domain metrics.
        """
        if len(self.heart_rate_data) < 2:
            raise ValueError("Not enough heart rate data for analysis.")

        freqs, power_spectrum = self.compute_fft(self.heart_rate_data)

        # Define frequency bands
        vlf_band = (0.003, 0.04)
        lf_band = (0.04, 0.15)
        hf_band = (0.15, 0.4)

        # Calculate power in each band
        vlf_power = self.band_power(freqs, power_spectrum, vlf_band)
        lf_power = self.band_power(freqs, power_spectrum, lf_band)
        hf_power = self.band_power(freqs, power_spectrum, hf_band)

        # Calculate LF/HF ratio
        lf_hf_ratio = lf_power / hf_power if hf_power != 0 else np.inf

        results = {
            'VLF Power': vlf_power,
            'LF Power': lf_power,
            'HF Power': hf_power,
            'LF/HF Ratio': lf_hf_ratio
        }
        return results

    def band_power(self, freqs, power_spectrum, band):
        """
        Calculate the power within a specific frequency band.

        Parameters:
        - freqs (np.array): Array of frequency bins.
        - power_spectrum (np.array): Power spectrum of the signal.
        - band (tuple): Frequency band (low_freq, high_freq).

        Returns:
        - power (float): Power within the specified band.
        """
        band_indices = np.logical_and(freqs >= band[0], freqs <= band[1])
        band_power = np.sum(power_spectrum[band_indices])
        return band_power

    def assess_sleep_apnea_risk(self, oxygen_desaturation_threshold=3):
        """
        Assess the risk of sleep apnea based on oxygen saturation levels.

        Parameters:
        - oxygen_desaturation_threshold (float): Percentage drop to count as a desaturation event.

        Returns:
        - desaturation_events (int): Number of desaturation events detected.
        """
        if len(self.oxygen_saturation_data) < 2:
            raise ValueError("Not enough oxygen saturation data for analysis.")

        desaturation_events = 0
        for i in range(1, len(self.oxygen_saturation_data)):
            drop = self.oxygen_saturation_data[i-1] - self.oxygen_saturation_data[i]
            if drop >= oxygen_desaturation_threshold:
                desaturation_events += 1

        return desaturation_events
