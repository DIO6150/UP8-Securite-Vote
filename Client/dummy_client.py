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

while (run):

    read_sockets, _, _ = select.select([client, sys.stdin], [], [])

    for source in read_sockets:
        # 1. Incoming message from server
        if source is client:
            data = client.recv(4096)
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
            send (msg)

client.close ()