import numpy as np
from collections import deque

class RollingMean:
    def __init__(self, window_size, age):
        self.window_size = window_size
        self.data = deque(maxlen=window_size)
        self.age = age

    def add_data_point(self, value):
        self.data.append(value)

    def get_rolling_mean(self):
        if not self.data:
            return None
        return np.mean(self.data)
    
    def classify_activity(self):
        rolling_mean_hr = self.get_rolling_mean()
        if rolling_mean_hr is None:
            return "Insufficient data"
        
        max_heart_rate = 220 - self.age
        if rolling_mean_hr < 60:
            return "Sleeping"
        elif rolling_mean_hr < 100:
            return "Sitting"
        elif rolling_mean_hr < 0.7 * max_heart_rate:
            return "Walking"
        else:
            return "Running"
