# LibTempo
C++ / Python library for signal processing and tempo information extraction.

## CLI
**MP3, FLAC and WAV audio formats are supported.**
### [Downloads](https://github.com/EgorDm/libtempo/releases)
### Examples
* [Megadeth - Five Magics](https://github.com/EgorDm/libtempo/raw/master/docs/assets/magics.png)
* [Led Zeppelin - Stairway To Heaven](https://github.com/EgorDm/libtempo/raw/master/docs/assets/stairway.png)
* [DragonForce - Valley of the Damned](https://github.com/EgorDm/libtempo/raw/master/docs/assets/valley_of.png)

### Usage
```bash
libtempo [audio] {OPTIONS}
```
```
    OPTIONS:

      -h, --help                        Display the help menu
      Basic
        -t[preferred_bpm],
        --preferred_bpm=[preferred_bpm]   (Default: 130) BPM around which the
                                          real bpm will be chosen.
        --bpm_rounding_precision=[bpm_rounding_precision]
                                          (Default: 0.500000) Precision of the
                                          BPM before correction..
        -s[smooth_length],
        --smooth_length=[smooth_length]   (Default: 20.000000) Length in seconds
                                          over which the tempogram will be
                                          stabilized to extract a steady tempo.
        -l[min_section_length],
        --min_section_length=[min_section_length]
                                          (Default: 10.000000) Minimum length
                                          for a tempo section in seconds.
        --max_section_length=[max_section_length]
                                          (Default: 40.000000) Maximum section
                                          length in seconds after which section
                                          is split in half.
      Advanced
        --bpm_window=[bpm_window]         BPM range which to check for tempo
                                          peaks.
        -m[tempo_multiples...],
        --tempo_multiples=[tempo_multiples...]
                                          Tempo multiples to consider when
                                          searching for correct offset
        --ref_tempo=[ref_tempo]           (Default: 60) Reference tempo defining
                                          the partition of BPM into tempo
                                          octaves for calculating cyclic
                                          tempogram.
        --tempo_window=[tempo_window]     (Default: 8) Analysis window length in
                                          seconds for calculating tempogram.
        --octave_divider=[octave_divider] (Default: 120) Number of tempo classes
                                          used for representing a tempo octave.
                                          This parameter controls the
                                          dimensionality of cyclic tempogram.
        --triplet_weight=[triplet_weight] (Default: 3.000000) Weight of the
                                          triplet intensity which will be adeed
                                          to its base intensity.
        --bpm_doubt_window=[bpm_doubt_window]
                                          (Default: 2.000000) Window around
                                          candidate bpm which to search for a
                                          more fine and correct bpm.
        --bpm_doubt_step=[bpm_doubt_step] (Default: 0.100000) Steps which to
                                          take inside the doubt window to fine
                                          tune the bpm.
        --click_track_subdivision=[click_track_subdivision]
                                          (Default: 8) Click subdivision for the
                                          click track.
      Output Flags
        -c[generate_click_track],
        --generate_click_track=[generate_click_track]
                                          (Default: 1) Wether or not a click
                                          track should be generated
        -o, --osu                         Wether or not to generate tempo data
                                          in osu format.
        --viz                             Saves a html file with useful graphs.
        -d, --dump                        Dump generated tempograms and novelty
                                          curves into a file.
      audio                             Audio file to extract tempo of.
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options
```

## Library
### [Building](https://github.com/EgorDm/libtempo/wiki/Building)
### [Documentation](https://github.com/EgorDm/libtempo/wiki/Documentation)

## Flaws / TODO's
* Find the right bpm multiple instead of taking a reference one and roudnign to it
* ~~Support mp3 files~~
* GUI
* Clean sections with small deviations and to only keep the essential ones
* Completely implement lib into python bindings
* Sometimes offset might be shifted by a 1/8th or 1/16th note

## References
* [Tempogram & novelty curve calculation](http://resources.mpi-inf.mpg.de/MIR/tempogramtoolbox/) [[1]](https://ieeexplore.ieee.org/document/5654580/) [[2]](http://resources.mpi-inf.mpg.de/MIR/tempogramtoolbox/2010_GroscheMuellerKurth_TempogramCyclic_ICASSP.pdf) [[3]](http://resources.mpi-inf.mpg.de/MIR/tempogramtoolbox/2009_GroscheMueller_PredominantLocalPeriodicy_WASPAA.pdf)
* [Modified signal resampling code](https://github.com/terrygta/SignalResampler)
