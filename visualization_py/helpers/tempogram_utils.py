import numpy as np


def subtract_mean_clean(s):
    ret = s - np.mean(s, axis=0)
    ret *= ret > 0
    ret_sum = np.sum(ret, 0)
    ret_sum[ret_sum == 0] = 1
    return ret / ret_sum


def max_bucket(s, axis_lut=None):
    bins = np.argmax(s, axis=0)
    if axis_lut is None: return bins
    return np.array([axis_lut[bins[i]] for i in range(len(bins))])


def extract_confidence(s, rollover=False):
    median = np.sum(s, axis=0) / 2
    maxes = np.argmax(s, axis=0)
    n_bins = int(s.shape[0])
    half_bins = int(s.shape[0] / 2)
    ret = np.ones(s.shape[1])
    for i in range(s.shape[1]):
        acc = s[maxes[i], i]
        for k in range(1, half_bins):
            if rollover: acc += s[(maxes[i] + k) % n_bins, i]
            elif 0 <= maxes[i] + k < n_bins: acc += s[maxes[i] + k, i]
            if 0 <= maxes[i] - k < n_bins or rollover: acc += s[maxes[i] - k, i]

            if acc >= median[i]:
                ret[i] = k / half_bins
                break

    return 1 - ret

