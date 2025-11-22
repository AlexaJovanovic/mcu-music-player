import sounddevice as sd
import numpy as np
import matplotlib.pyplot as plt

# ===== Device setup =====
sd.default.device = (1, 3)  # (input, output)
fs = 22100
duration = 9.0

print("Recording from Microphone Array...")
recording = sd.rec(int(duration * fs), samplerate=fs, channels=1, dtype='float64')
sd.wait()
print("Recording complete.")

print("Playing back through Speakers...")
sd.play(recording, samplerate=fs)
sd.wait()
print("Playback finished.")

# ===== FFT analysis =====
signal = recording.flatten()
N = len(signal)
freqs = np.fft.rfftfreq(N, d=1/fs)
magnitude = np.abs(np.fft.rfft(signal)) / N

dominant_freq = freqs[np.argmax(magnitude)]
print(f"Dominant frequency: {dominant_freq:.1f} Hz")

plt.figure(figsize=(12, 6))
plt.subplot(2, 1, 1)
plt.plot(np.linspace(0, duration, N), signal)
plt.title("Time Domain Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")

plt.subplot(2, 1, 2)
plt.plot(freqs, magnitude)
plt.title("Frequency Spectrum")
plt.xlabel("Frequency [Hz]")
plt.ylabel("Magnitude")
plt.xlim(0, 8000)
plt.tight_layout()
plt.show()
