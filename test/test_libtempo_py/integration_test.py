import numpy as np
import libtempo_py as lt
import urllib.request
import os
import math
import sys

SAVE_TEST_DATA_PATH = 'data'
TEST_FILES = [
    {
        'url': 'https://audionautix.com/Music/BeBop25.mp3',
        'name': 'BeBop25.mp3',
        'bpm': 100,
        'sr': 48000,
        'channels': 2
    }
]
REF_TEMPO = 60
BPM_RANGE = (30, 630)
MERGE_TRUTH_THRESHOLD = 10  # s
MERGE_LABELS_THRESHOLD = 4  # s
SECTION_LENGTH_THRESH = 60
OCTAVE_DIVIDER = 120
MAX_SECTION_LEN = 30
SECTION_MERGE_THRESH = 1.5


def download_audio(url, name):
    file_path = f'{SAVE_TEST_DATA_PATH}/{name}'
    if os.path.exists(file_path): return

    urllib.request.urlretrieve(url, f'{SAVE_TEST_DATA_PATH}/{name}')


def setup_tests():
    os.makedirs(SAVE_TEST_DATA_PATH, exist_ok=True)
    for file in TEST_FILES: download_audio(file['url'], file['name'])


def compare_assert(a, b, name='Values', epsilon=0.00001):
    if a != b:
        raise Exception(f'{name} does not match {a} vs {b}!')

def compare_num_assert(a, b, name='Values', epsilon=0.00001):
    if abs(a - b) > epsilon:
        raise Exception(f'{name} does not match {a} vs {b}!')


def integration_test1(file):
    file_path = f'{SAVE_TEST_DATA_PATH}/{file["name"]}'
    if not os.path.exists(file_path): raise Exception(f'File not found!')

    # Audio loading
    audio_file = lt.audio.AudioFile.open(file_path)
    compare_assert(audio_file.sr, file['sr'], 'Sample rate')
    compare_assert(audio_file.data.shape[0], file["channels"], 'Channels')

    audio_mono = np.mean(audio_file.data, axis=0)
    compare_assert(len(audio_mono.shape), 1, 'Mono Channels')

    # Novelty curve
    novelty_curve, novelty_curve_sr = lt.audio_to_novelty_curve(audio_mono, audio_file.sr)
    compare_num_assert(len(novelty_curve) / novelty_curve_sr, audio_file.data.shape[1] / audio_file.sr, 'Novelty curve size', epsilon=1)

    # Tempogram
    tempogram_y_axis = np.arange(*BPM_RANGE)
    tempogram, t = lt.novelty_curve_to_tempogram(novelty_curve, tempogram_y_axis, novelty_curve_sr, 8)
    compare_assert(tempogram.shape[0], len(tempogram_y_axis), 'Tempogram Y axis length')
    compare_assert(tempogram.shape[1], len(t), 'Tempogram X axis length')

    # Cyclic Tempogram
    cyclic_tempogram, cyclic_tempogram_y_axis = lt.tempogram_to_cyclic_tempogram(tempogram, tempogram_y_axis, ref_tempo=REF_TEMPO, octave_divider=OCTAVE_DIVIDER)
    compare_assert(cyclic_tempogram.shape, (OCTAVE_DIVIDER, tempogram.shape[1]), 'Cyclic tempogram shape')
    compare_assert(cyclic_tempogram_y_axis[0], 1, 'Cyclic tempogram Y axis start')

    # Smoothen tempogram
    smoothed_tempogram = lt.smoothen_tempogram(cyclic_tempogram, cyclic_tempogram_y_axis, t, 20)
    compare_assert(smoothed_tempogram.shape, cyclic_tempogram.shape, 'Smoothed tempogram shape')

    # Tempo curve
    tempo_curve = lt.tempogram_to_tempo_curve(smoothed_tempogram, cyclic_tempogram_y_axis)
    compare_assert(len(tempo_curve), cyclic_tempogram.shape[1], 'Tempo curve length')
    tempo_curve = lt.correct_tempo_curve(tempo_curve, t, 5) # TODO: test

    # Tempo sections
    tempo_sections = lt.curve_to_sections(tempo_curve, t, REF_TEMPO, MAX_SECTION_LEN, SECTION_MERGE_THRESH)
    last_bpm = -100
    last_len = 100000
    for s in tempo_sections:
        if s.end - s.start > MAX_SECTION_LEN:
            raise Exception('Section splitting is incorrect')
        if abs(last_bpm - s.bpm) < SECTION_MERGE_THRESH and abs(last_len + s.end - s.start) < MAX_SECTION_LEN:
            raise Exception(f'Section merging is incorrect {last_bpm} vs {s.bpm} and {s.end - s.start}')
        last_bpm = s.bpm

    if tempo_sections[-1].end > len(novelty_curve) / novelty_curve_sr: tempo_sections[-1].end -= 0.1

    compare_num_assert(tempo_sections[-1].end - tempo_sections[0].start, audio_file.data.shape[1] / audio_file.sr, 'Section spans', epsilon=1)

    tempo_sections = lt.sections_extract_offset(novelty_curve, tempo_sections, [1, 2, 4], novelty_curve_sr, bpm_doubt_window=5)
    for s in tempo_sections:
        compare_num_assert(s.bpm, file['bpm'], 'Section bpm', epsilon=1)

    # Click track
    click_track = np.zeros_like(audio_mono)
    for s in tempo_sections:
        length = int((s.end - s.start) * audio_file.sr)
        start = int(s.start * audio_file.sr)
        clicks = lt.audio.annotation.click_track_from_tempo(s.bpm, s.offset - s.start, length, 8, audio_file.sr)
        compare_num_assert(len(clicks)/audio_file.sr, s.end - s.start, 'Click length', epsilon=1)

        if len(clicks) + start >= len(click_track): clicks = clicks[0: len(click_track) - start]

        click_track[start: start + len(clicks)] = clicks

    # Save the click track
    audio_data = np.copy(audio_file.data)
    for c in range(audio_data.shape[0]):
        audio_data[c, :] = np.clip(audio_data[c, :] + click_track, -1, 1)

    audio_file.data = audio_data
    save_click_path = SAVE_TEST_DATA_PATH + '/' + os.path.splitext(file['name'])[0] + '_clicks.wav'
    audio_file.save(save_click_path)

    if not os.path.exists(save_click_path): raise Exception('Cant save click track!')

    print('integration_test1 succeeded for ' + file['name'])


def main():
    setup_tests()
    try:
        for file in TEST_FILES:
            integration_test1(file)
        print('All tests succeeded')
    except Exception as e:
        print(e)
        print('One or more tests failed')
        sys.exit(1)


if __name__ == "__main__":
    main()
