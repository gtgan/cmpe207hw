import socket, string, sys


#this will be the format for decoding the data.
FORMAT = 'utf-8'
#this is the port that the server will use for communication, the telnet port. 
PORT = 5050
# this is the IP Address of the server we are trying to connect to. 
SERVER = socket.gethostbyname(socket.gethostname())
# the address is basically a combination of IP address and PORT, 
ADDR = (SERVER, PORT)
print(f"The socket I am connecting to is : {ADDR}")

# client socket is created using the family and type. family is INET for IPv4
# the type is stream
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
		client.connect(ADDR)
except:
		print('Unable to connect')
		sys.exit()

print(f"[CONNECTED] to {ADDR}")

while True:
    #incoming message from remote server
    #will recieve the data using telnet protocols 4096 bytes of data. 
    data = client.recv(2048)
    if not data :
        print('Connection closed')
        sys.exit()
    else :
        # will have to decode the bytes into utf-8 to be printed. 
        dataString = data.decode(FORMAT)
        sys.stdout.write(dataString)