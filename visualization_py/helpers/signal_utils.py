import numpy as np
import math

from helpers.data_loading import Section


def generate_pulse(bpm, window_length, feature_rate, shift: int =0):
    freq = bpm / 60
    T = np.arange(-shift, window_length - shift, 1) / feature_rate
    twoPiT = 2 * math.pi * T
    twoPiFt = freq * twoPiT
    cosine = np.cos(twoPiFt)
    sine = np.sin(twoPiFt)
    return cosine, sine


def pulse_for_section(section: Section, feature_rate, multiple=1):
    offset_rel = section.offset - section.start
    offset_samples = int(offset_rel * feature_rate)
    window_length = int((section.end - section.start) * feature_rate)
    return generate_pulse(section.bpm * multiple, window_length, feature_rate, shift=offset_samples)


def times_from_feature_rate(feature_rate, start, length):
    return [i / feature_rate for i in range(start, start + length)]
