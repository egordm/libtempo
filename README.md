# Tempo Estimation
C++ / Python library with signal processing and tempo estimation utilities.  
Still WIP.

# Demo: Dragonforce - Valley of the Damned
### Novelty Curve
![Novely Curve](https://raw.githubusercontent.com/EgorDm/tempo_estimation/master/docs/assets/valley_novelty.png)
### Tempogram
![Tempogram](https://raw.githubusercontent.com/EgorDm/tempo_estimation/master/docs/assets/valley_tempogram.png)
### Cyclic Tempogram
![Cyclic Tempogram](https://raw.githubusercontent.com/EgorDm/tempo_estimation/master/docs/assets/valley_cyclic_tempogram.png)

## Dependencies
Libraries needed to compile the project.
* [libsndfile 1.0.28](http://www.mega-nerd.com/libsndfile/)
* [OpenBLAS 0.2.14](https://www.openblas.net/)
* [Armadillo 8.600.0](http://arma.sourceforge.net/)
* [FFTW 3.3.8](http://www.fftw.org/)
* [SIGPACK 1.2.4](http://sigpack.sourceforge.net/)
* [pybind11 2.2.3](https://github.com/pybind/pybind11)
* [GTEST 1.8.0](https://github.com/google/googletest)

#### Few useful cmake flags
`DGMOCK_ROOT, Dpybind11_INCLUDE_DIR, DBLAS_LIBRARY, DLAPACK_LIBRARY, DARMADILLO_INCLUDE_DIR, DARMADILLO_LIBRARY, 
DSIGPACK_INCLUDE_DIR, DFFTW_ROOT, DPYTHON_EXECUTABLE, DPYTHON_LIBRARY, DPYTHON_INCLUDE_DIRS`

## Build targets
`use cmake --build . --target my_target here`
* `main` - command line app for tempo extraction. (WIP)
* `tempo_estimation_py` - python library for interfacting the tempo estimation library (Usable, but WIP)
* `tempo_estimation` - library with utilities for tempo extraction. (Usable, but WIP)

Use `tempo_estimation` if you want to use this library in your project.

## Documentation
Applies to both C++ and Python API. In Python use numpy arrays and matrices instead.

### audio_to_novelty_curve
**Arguments**
- signal: wavefrom of audio signal
- sr: sampling rate of the audio (Hz)
- window_length: window length for STFT (in samples)
- hop_length: stepsize for the STFT
- compression_c: constant for log compression
- log_compression: enable/disable log compression
- resample_feature_rate: feature rate of the resulting novelty curve (resampled, independent of stepsize)

**Returns**  
Tuple(novelty_curve, feature_rate)

**Description**
Computes a novelty curve (onset detection function) for the input audio signal. This implementation is a
variant of the widely used spectral flux method with additional bandwise processing and a logarithmic intensity
compression. This particularly addresses music with weak onset information (e.g., exhibiting string instruments.)


### novelty_curve_to_tempogram_dft
**Arguments**
- novelty_curve: a novelty curve indicating note onset positions
- bpm: vector containing BPM values to compute
- feature_rate: feature rate of the novelty curve (Hz). This needs to be set to allow for setting other parameters in seconds!
- tempo_window: Analysis window length in seconds
- hop_length: window hop length in frames (of novelty curve)

**Returns**  
Tuple(tempogram, time vector, bpm vector)

**Description**
Computes a complex valued fourier tempogram for a given novelty curve
indicating note onset candidates in the form of peaks.
This implementation provides parameters for chosing fourier coefficients in a frequency range corresponding to 
musically meaningful tempo values in bpm.

### normalize_feature
**Arguments**
- feature: matrix
- p
- threshold

**Returns**  
Normalized feature matrix

**Description**  
Normalizes a feature sequence according to the l^p norm
If the norm falls below threshold for a feature vector, then the normalized feature vector is set to be the
unit vector.

### compute_fourier_coefficients
**Arguments**
- s time domain signal
- window vector containing window function
- n_overlap overlap given in samples
- f vector of frequencies values of fourier coefficients, in Hz
- sr sampling rate of signal s in Hz

**Returns**  
Tuple(complex fourier coefficients, frequency vector, time vector)

**Description**  
Function that calculates a fourier coefficient with frequency f.  

### stft
**Arguments**
- signal: wavefrom of audio signal
- sr: sample rate
- window
- coefficient_range
- n_fft: window length
- hop_length

**Returns**  
Tuple(spectrogram, frequency vector, time vector)

**Description**  
Computes aspectrogram using a STFT (short-time fourier transform)


## References
* [Tempogram & novelty curve calculation](http://resources.mpi-inf.mpg.de/MIR/tempogramtoolbox/) [[1]](https://ieeexplore.ieee.org/document/5654580/) [[2]](http://resources.mpi-inf.mpg.de/MIR/tempogramtoolbox/2010_GroscheMuellerKurth_TempogramCyclic_ICASSP.pdf) [[3]](http://resources.mpi-inf.mpg.de/MIR/tempogramtoolbox/2009_GroscheMueller_PredominantLocalPeriodicy_WASPAA.pdf)
* [Modified signal resampling code](https://github.com/terrygta/SignalResampler)
