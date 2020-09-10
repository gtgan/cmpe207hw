import socket, string, sys

PORT = 23
SERVER = "94.142.241.111"
ADDR = (SERVER, PORT)
FORMAT = 'utf-8'

print(f"The socket I am connecting to is : {ADDR}")
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
		client.connect(ADDR)
except:
		print('Unable to connect')
		sys.exit()


print(f"[CONNECTED] to {ADDR}")
read_sockets = []
read_sockets.append(client)
while 1:
		
		for sock in read_sockets:
			#incoming message from remote server
			if sock == client:
				data = sock.recv(4096)
				if not data :
					print('Connection closed')
					sys.exit()
				else :
					#print data
					dataString = data.decode(FORMAT)
					sys.stdout.write(dataString)
			
			#user entered a message
			else :
				msg = sys.stdin.readline()
				s.send(msg)