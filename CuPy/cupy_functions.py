import cupy as cp
import numpy as np

# Frequency filter using cupy
def cupy_fft_filter(input_array, freq, width):
    input_array_gpu = cp.asarray(input_array) 
    result_gpu = cp.fft.fft(input_array_gpu)
    result_cpu = cp.asnumpy(result_gpu)
    n_sample = len(input_array)
    freq = np.fft.fftfreq(n_sample)
    result_cpu[abs(abs(freq)-freq)<width] = 0
    filtered_waveform_gpu = cp.fft.ifft(cp.asarray(result_cpu)).real
    filtered_waveform_cpu = cp.asnumpy(filtered_waveform_gpu)

    return filtered_waveform_cpu
