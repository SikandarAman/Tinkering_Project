# To check a sudden change in the values measured by our sensors
class Outlier:
    def __init__(self):
        self.mean = 0
        self.var = 0
        self.num_samples = 0

    def update_stats(self, new_data):
        n2 = len(new_data)
        if n2 == 0:
            return

        mean2 = sum(new_data) / n2
        var2 = sum((x - mean2) ** 2 for x in new_data) / n2 

        if self.num_samples == 0:
            self.mean = mean2
            self.var = var2
            self.num_samples = n2
        else:
            n1 = self.num_samples
            mean1 = self.mean
            var1 = self.var

            new_mean = (n1 * mean1 + n2 * mean2) / (n1 + n2)
            new_var = (
                n1 * (var1 + (mean1 - new_mean) ** 2) +
                n2 * (var2 + (mean2 - new_mean) ** 2)
            ) / (n1 + n2)

            self.mean = new_mean
            self.var = new_var
            self.num_samples += n2

    def z_scores(self, data):
        if self.var == 0:
            raise ValueError("Variance must be non-zero to calculate z-scores.")
        std = self.var ** 0.5
        return [(x - self.mean) / std for x in data]

    def detect_outliers(self, data, threshold=2.5):
        z = self.z_scores(data)
        return [(i, x) for i, x, z_val in enumerate(zip(data, z)) if abs(z_val) > threshold]