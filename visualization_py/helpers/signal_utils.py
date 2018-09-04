import numpy as np
import math

from helpers.data_loading import Section


def generate_pulse(bpm, window_length, feature_rate):
    freq = bpm / 60
    T = np.arange(0, window_length, 1) / feature_rate
    twoPiT = 2 * math.pi * T
    twoPiFt = freq * twoPiT
    cosine = np.cos(twoPiFt)
    sine = np.sin(twoPiFt)
    return cosine, sine


def pulse_for_section(section: Section, feature_rate, multiple=1):
    offset_rel = section.offset - section.start
    cosine, sine = generate_pulse(section.bpm * multiple,
                                  int((section.end - section.start + offset_rel) * feature_rate), feature_rate)
    offset_samples = int(offset_rel * feature_rate)
    return cosine[offset_samples:], sine[offset_samples:]


def times_from_feature_rate(feature_rate, start, length):
    return [i / feature_rate for i in range(start, start + length)]
