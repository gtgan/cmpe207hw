import socket
import threading


#this will be the format for decoding the data
FORMAT = 'utf-8'
# this will set the protocol that will be the same number everytime. it will say the # of bytes of the next Msg. 
HEADER = 64
#this is the port that the server will use for communication
PORT = 5050
# this line will get the current IP address of the machine. the IP address assigned to the miachine is dynacimt because of NAT so this code will dynamically get the current IP address.
SERVER = socket.gethostbyname(socket.gethostname())
# the address is basically a combination of IP address and PORT, 
ADDR = (SERVER, PORT)



# server is created using the family and type. family is INET for IPv4
# the type is stream

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#the server is assigned the ADDR, these three things make the socket; type of comm, port, and IP address. 
server.bind(ADDR)

#this will handle a client
def handle_client (conn, addr):
    print(f"[NEW CONNECION] {addr} connected.")
    image_file = open("ascii_image.txt")
    lines = image_file.readlines()
    for line in lines:
        conn.send(line.encode(FORMAT))        
        
    conn.close()

    

def start():
    # will listen on the socket for incoming data continously. 
    server.listen()
    print(f"[LISTENING] Server is listeing on {server}")
    while True:
        # will pick up a connection and assign client info to variables. 
        conn, addr = server.accept()
        
        #will now create a new thread to handle the indivdual new connection
        thread = threading.Thread(target=handle_client, args=(conn, ADDR))
        thread.start()
        print(f"[ACTIVE CONNECTIONS] {threading.activeCount() -1}")



print ("[STARTING] server is starting")
start()


