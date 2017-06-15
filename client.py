import socket
import pyaudio
import wave

#record
CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
RECORD_SECONDS = 30

HOST = '10.105.24.196'    # The remote host
PORT = 2017              # The same port as used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

p = pyaudio.PyAudio()

stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

print("Start recording")


for i in range(0, int(RATE/CHUNK*RECORD_SECONDS)):
	data  = stream.read(CHUNK)
	s.send(data)

print("Finish recording")

stream.stop_stream()
stream.close()
p.terminate()
s.close()
