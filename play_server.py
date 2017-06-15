#!/usr/bin/env python3
import argparse
import logging
import socket
import sounddevice as sd
import soundfile as sf
import numpy as np
import wave
import pyaudio
from playsound import playsound
import pickle

sk = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
host = "localhost"
port = 2017
sk.bind((host,port))
sk.listen(5)

FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 44100
WAVE_OUTPUT_FILENAME = "server.wav"
chunk = 1024
p = pyaudio.PyAudio()

print("Server start....")
while True:
	connection,addr = sk.accept()
	print("Got connection from",addr)
	msg = connection.recv(2048)
	da=pickle.loads(msg)
	#my_bytes = bytearray(msg)
	#ndata = np.fromstring(msg, dtype=np.float32)
	#sd.play(ndata,44100,blocking=True)
	#status = sd.get_status()
	#if status:
#		logging.warning(str(status))
	#connection.close()
	
	wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
	wf.setnchannels(CHANNELS)
	wf.setsampwidth(p.get_sample_size(FORMAT))
	wf.setframerate(RATE)
	wf.writeframes(da)
	wf.close()
	
	f = wave.open("output.wav", 'rb')
	stream = p.open(format = p.get_format_from_width(f.getsampwidth()),  
                channels = f.getnchannels(),  
                rate = f.getframerate(),  
                output = True)  
	#read data  
	data = f.readframes(chunk)  

	#play stream  
	while data:  
		stream.write(data)  
		data = f.readframes(chunk)  

	#stop stream  
	stream.stop_stream()  
	stream.close()  

	#close PyAudio  
	p.terminate() 
	connection.close()
#parser = argparse.ArgumentParser(description=__doc__)
#parser.add_argument("filename", help="audio file to be played back")
#parser.add_argument("-d", "--device", type=int, help="device ID")
#args = parser.parse_args()
#try:
#    data, fs = sf.read(args.filename, dtype='float32')
#    sd.play(data, fs, device=args.device, blocking=True)
#    status = sd.get_status()
#    if status:
#        logging.warning(str(status))
#except BaseException as e:
#    # This avoids printing the traceback, especially if Ctrl-C is used.
#    raise SystemExit(str(e))
