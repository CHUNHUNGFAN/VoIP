import socket
sk = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
host = "localhost"
port = 2017
sk.bind((host,port))
sk.listen(5)
print("Server start....")
while True:
      connection,addr = sk.accept()
      print("Got connection from",addr)
      msg = connection.recv(2048)
      print(msg.decode(encoding = "utf-8"))
      connection.send("ok!".encode(encoding = 'utf-8'))
      connection.close()

