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
# for tone, section in zip(my_tones, tones_plot):
# 	plt.subplot(section)
# 	plt.title(tone)
# 	print(tone)
# 	start = int(tone_samples * (tone - MIDIFROM) + SKIP_SEC * Fs)
# 	end = int(start + HOWMUCH_SEC * Fs)
# 	plt.xlabel('$t [s]$')
# 	plt.plot(np.arange(SKIP_SEC, SKIP_SEC + HOWMUCH_SEC, Fs**-1), s[start : end])
# #	generovani .wav filu
# 	start_gen = int(tone_samples * (tone - MIDIFROM))
# 	end_gen = int(start_gen + WHOLETONE_SEC * Fs)
# 	scp.write(str(tone) + ".wav", Fs, s[start_gen : end_gen])


# vykresleni 3 period tonu
for tone, section, f in zip(my_tones, tones_plot, tones_frecs):
	plt.subplot(section)
	plt.title("3 periody:" + str(tone))
	print(tone)
	start = int(tone_samples * (tone - MIDIFROM) + SKIP_SEC * Fs)
	end = int(start +  3 * (f**-1)* Fs)
	plt.xlabel('$t [s]$')
	plt.ylabel('Amplituda')
	plt.plot(np.arange(0, (end - start)*Fs**-1,Fs**-1),s[start : end])

# # vykresleni spektra
# for tone, section in zip(my_tones, tones_plot):
# 	start = int(tone_samples * (tone - MIDIFROM) + SKIP_SEC*Fs)
# 	end = int(start + Fs*HOWMUCH_SEC)
# 	mod = np.fft.fft(s[start : end])
# 	mod = mod[:mod.size // 2] # symetricke
# 	mod = np.log2(mod)
# 	plt.subplot(section)
# 	plt.title(" spektrum tonu:" + str(tone))
# 	plt.xlabel('$f [Hz]$')
# 	plt.ylabel('Amplituda')
# 	plt.plot(mod)

plt.tight_layout()
plt.show()


