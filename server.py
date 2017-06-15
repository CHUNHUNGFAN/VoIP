import socket
import pyaudio
import wave
import time

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
WAVE_OUTPUT_FILENAME = "server_output.wav"
#WIDTH = 2

p = pyaudio.PyAudio()
stream = p.open(
                format=FORMAT,
				channels=CHANNELS,
                rate=RATE,
                output=True,
                frames_per_buffer=CHUNK)

HOST = "10.99.1.30"                 # Symbolic name meaning all available interfaces
PORT = 2017              # Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(1)

while True:
	print('Server is ready!')
	conn, addr = s.accept()
	print('Connected by', addr)
	data = conn.recv(2048)
	frames = []
	
	while data:
		stream.write(data)
		data = conn.recv(2048)
		frames.append(data)

	print('Audio is over!')
	stream.stop_stream()
	stream.close()
	p.terminate()
	conn.close()
	wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
	wf.setnchannels(CHANNELS)
	wf.setsampwidth(p.get_sample_size(FORMAT))
	wf.setframerate(RATE)
	wf.writeframes(b''.join(frames))
	wf.close()
