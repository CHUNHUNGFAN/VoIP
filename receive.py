import socket
import pyaudio
import wave
import sys                   

port = 9999                   # Reserve a port for your service.
s = socket.socket()             # Create a socket object
host = socket.gethostname()     # Get local machine name
s.bind((host, port))            # Bind to the port
s.listen(5)                     # Now wait for client connection.
f = open('new_file.wav', 'wb')
print('Server listening....')


c, addr = s.accept()
print('Got connection from ', addr)
print('Receiving...')
l = c.recv(1024)
while (l):
    f.write(l)
    l = c.recv(1024)
f.close()
print('Done Receiving')
c.close()
s.close()
CHUNK = 1024


wf = wave.open('new_file.wav', 'rb')

p = pyaudio.PyAudio()

stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                channels=wf.getnchannels(),
                rate=wf.getframerate(),
                output=True)

data = wf.readframes(CHUNK)

while data:
    stream.write(data)
    data = wf.readframes(CHUNK)

stream.stop_stream()
stream.close()

p.terminate()
