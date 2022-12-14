import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt
import scipy.io.wavfile as scp

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
plt.figure()

tone_samples = WHOLETONE_SEC * Fs
# #vykresleni tonu
for tone, section in zip(my_tones, tones_plot):
	plt.subplot(section)
	plt.title(tone)
	print(tone)
	start = int(tone_samples * (tone - MIDIFROM) + SKIP_SEC * Fs)
	end = int(start + HOWMUCH_SEC * Fs)
	plt.plot(s[start : end])
# #	generovani .wav filu
# 	start_gen = int(tone_samples * (tone - MIDIFROM))
# 	end_gen = int(start_gen + WHOLETONE_SEC * Fs)
# 	scp.write(str(tone) + ".wav", Fs, s[start_gen : end_gen])


# # vykresleni 3 period tonu
# for tone, section, f in zip(my_tones, tones_plot, tones_frecs):
# 	plt.subplot(section)
# 	plt.title("3 periods of:" + str(tone))
# 	print(tone)
# 	start = int(tone_samples * (tone - MIDIFROM) + SKIP_SEC * Fs)
# 	end = int(start +  3 * (f**-1)* Fs)
# 	plt.plot(s[start : end])
plt.show()


