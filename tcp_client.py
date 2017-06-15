import socket
sk = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
host = "localhost"
port = 2017

print("connecting to server....")
sk.connect((host, port))
str = input("What do you want to do?")
sk.send(str.encode(encoding = 'utf-8'))
buffer = sk.recv(2048)
print(buffer.decode(encoding = 'utf-8'))

sk.close

