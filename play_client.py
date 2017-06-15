import pyaudio
import wave
import socket
import numpy
import sounddevice as sd

sk = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
host = "localhost"
port = 50007

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 44100
RECORD_SECONDS = 30
WAVE_OUTPUT_FILENAME = "output.wav"

p = pyaudio.PyAudio()

stream = p.open(format=FORMAT,
                channels=CHANNELS,
				rate=RATE,
			    input=True,
				frames_per_buffer=CHUNK)
print("connecting to server....")
sk.connect((host, port))

print("* recording")

frames = []
for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
    data = stream.read(CHUNK)
    frames.append(numpy.fromstring(data, dtype=numpy.float32))
print("* done recording")
#numpydata = numpy.hstack(frames)
#d=pickle.dumps(numpydata)
sk.send(b''.join(frames))
#sk.send(numpydata)
stream.stop_stream()
stream.close()
p.terminate()

wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
wf.setnchannels(CHANNELS)
wf.setsampwidth(p.get_sample_size(FORMAT))
wf.setframerate(RATE)
#wf.writeframes(b''.join(frames))
wf.writeframes(numpydata)
wf.close()
sk.close()
