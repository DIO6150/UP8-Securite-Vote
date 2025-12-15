import socket
import select
import sys

client = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
client.connect (('localhost', 12345))

def send (usr_msg):
        header = int.to_bytes (len (usr_msg), length=4)
        print (header)
        message = header + bytes (usr_msg, encoding='ASCII')

        client.send (message)

run = True

cle_public=1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

while (run):

    read_sockets, _, _ = select.select([client, sys.stdin], [], [])

    for source in read_sockets:
        # 1. Incoming message from server
        if source is client:
            header_size_bytes = client.recv(4)
            header_size = int.from_bytes (header_size_bytes)

            data = client.recv (header_size)
            if not data:
                print("Server closed connection.")
                run = False
                break
                
            print(f"[server] {data.decode().rstrip()}")

        # 2. Input from user
        else:
            msg = sys.stdin.readline()
            if not msg:
                continue

            msg.strip ('\n')

            if msg == "exit\n":
                run = False
                break

            if msg == "send_key\n":
                send ("SEND_KEY " + str(cle_public))
                continue
            send (msg)

client.close ()
