import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt
import scipy.signal as scipsig

MIDITO = 108
MIDIFROM = 24
WHOLETONE_SEC = 2
SKIP_SEC = 0.25
HOWMUCH_SEC = 0.5
s, Fs = sf.read("klavir.wav")
tone_samples = WHOLETONE_SEC * Fs
plt.figure()


for i in reversed(range(MIDIFROM, MIDITO + 1)):
    start = int(tone_samples * (i - MIDIFROM) + SKIP_SEC*Fs)
    end = int(start + Fs)
    mod = np.abs(np.fft.fft(s[start: end]))
    mod = mod[:mod.size // 2]  # symetricke

    average = np.average(mod[:500]) * 2
    peaks = scipsig.find_peaks(mod, prominence=5, height=12*average)
    biggest_peak = scipsig.find_peaks(mod, height=max(mod))
    peaks = peaks[0]
    index = np.where(peaks == biggest_peak[0][0])
    peaks = peaks[:index[0][0] + 1]
    if i >= 41:
        frec = peaks[-1]
    elif i >= 38:
        frec = peaks[-1]
    else:
        frec = peaks[-1] / 2

    if i == 24:
        plt.subplot(611)
        plt.title(str(i) + ":    f = " + str(frec))
        plt.plot(mod[:700])
        plt.xlabel('$f [Hz]$')
        plt.plot(peaks[-1], mod[peaks[-1]], "*", color="black")
    if i == 59:
        plt.subplot(613)
        plt.title(str(i) + ":    f = " + str(frec))
        plt.plot(mod[:2000])
        plt.xlabel('$f [Hz]$')
        plt.plot(peaks[-1], mod[peaks[-1]], "*", color="black")
    if i == 89:
        plt.subplot(615)
        plt.title(str(i) + ":    f = " + str(frec))
        plt.plot(mod[:2000])
        plt.xlabel('$f [Hz]$')
        plt.plot(peaks[-1], mod[peaks[-1]], "*", color="black")
    # obecny vypocet pro vsechny tony
    if i < 41:
        print(str(i) + "\t" + str(peaks[0] / 2))
    else:
        print(str(i) + "\t" + str(peaks[0]))

plt.show()
