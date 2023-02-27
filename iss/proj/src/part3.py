import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt

MIDITO = 108
MIDIFROM = 24
WHOLETONE_SEC = 2
SKIP_SEC = 0.25
HOWMUCH_SEC = 0.5

tones = np.arange(MIDIFROM, MIDITO+1)
s, Fs = sf.read("klavir.wav")
my_tones = [24, 59, 89]
tones_plot = [311, 312, 313]
tones_frecs = [32.70, 246.94, 1396.91]
tone_samples = WHOLETONE_SEC * Fs


for tone in reversed(tones):
    start = int(tone_samples * (tone - MIDIFROM) + SKIP_SEC*Fs)
    end = int(start + Fs)
    X = np.fft.fft(s[start:end])
    x = s[start:end]
    FREQRANGE = 50
    FREQPOINTS = 200
    N = Fs
    n = np.arange(0, N)
    kall = np.arange(0, int(N/2) + 1)
    Xmag = np.abs(X[kall])
    Xphase = np.angle(X[kall])
    f = kall / N * Fs

    
    fmax = f[np.argmax(Xmag)]
    Xmax = np.max(Xmag)
    ffrom = fmax-FREQRANGE
    fto = fmax+FREQRANGE
    fsweep = np.linspace(fmax-FREQRANGE, fmax+FREQRANGE, FREQPOINTS)

    # pocitani DTFT
    A = np.zeros([FREQPOINTS, N], dtype=complex)
    for k in np.arange(0, FREQPOINTS):
        # norm. omega = 2 * pi * f / Fs ...
        A[k, :] = np.exp(-1j * 2 * np.pi * fsweep[k] / Fs * n)
    Xdtft = np.matmul(A, x.T)

    precisefmax = fsweep[np.argmax(np.abs(Xdtft))]
    if tone < 38:
        print(str(tone) + "\t" + str(precisefmax / 2))
    elif tone < 41:
            print(str(tone) + "\t" + str(precisefmax / 3))
    else:
        print(str(tone) + "\t" + str(precisefmax))
