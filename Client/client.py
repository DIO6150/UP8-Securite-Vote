import socket
import chiffrement
import queue
import threading

def send(msg):
	if(type(msg)==int):
		msg_bytes = msg.to_bytes(4, 'big')
	else:
		msg_bytes = msg.encode('ASCII')
	len = len(msg_bytes)
	msg_chiffre = rsa(server_cle_public,msg_bytes)
	client.sendall(len)
	client.sendall(msg_chiffre)

def listen():
	size_bytes = client.recv(4)
	msg_size = int.from_bytes(size_bytes, 'big')
	msg_chiffre = client.recv(msg_size)
	msg_dechiffre = rsa_decrypt(cle_private, msg_chiffre).decode('ASCII')
	responses.put(msg_dechiffre) # queue est thread safe



client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

cle_public=0
cle_private=1
responses = queue.Queue()
listen_thread = threading.Thread(target=listen, daemon=True)
listen_thread.start()

client.connect(('localhost', 12345))
server_cle_public = int.from_bytes(client.recv(msg_size)) #taille fix
print("Réponse du serveur :", 	server_key_public)

cle_public_chiffre = rsa(server_cle_public, cle_public.to_bytes(4,'big'))
client.sendall(cle_public_chiffre)

def login(nom, mdp):
	msg = "LOGIN " + nom + " " + mdp
	send(msg)



client.close()
