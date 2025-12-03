import socket
import chiffrement
import queue
import threading

def send(msg):
	if(type(msg)==int):
		msg_bytes = msg.to_bytes(4, 'big')
	else:
		msg_bytes = msg.encode('ASCII')
	size = len(msg_bytes).to_bytes(4, 'big')
	msg_chiffre = chiffrement.rsa(server_cle_public,msg_bytes)
	client.sendall(size)
	client.sendall(msg_chiffre)

def listen():
	size_bytes = client.recv(4)
	msg_size = int.from_bytes(size_bytes, 'big')
	msg_chiffre = client.recv(msg_size)
	msg_dechiffre = chiffrement.rsa(cle_private, msg_chiffre).decode('ASCII')
	responses.put(msg_dechiffre) # queue est thread safe


client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

cle_public=0
cle_private=1
candidats = 0

client.connect(('localhost', 12345))
server_cle_public = int.from_bytes(client.recv(112)) #taille fix
print("Réponse du serveur :", 	server_cle_public)

cle_public_chiffre = chiffrement.rsa(server_cle_public, cle_public.to_bytes(4,'big'))
client.sendall(cle_public_chiffre)
search = ""
responses = queue.Queue()
listen_thread = threading.Thread(target=listen, daemon=True)
listen_thread.start()

def read():
	rt = "E"
	while(not queue.Empty):
		message = responses.get()
		responses.task_done()
		mots = message.upper().split()
		if mots[0] == "RETURN":
			if mots[2] == search:
				if mots[1] == "CODE":
					rt = mots[3]
				elif mots[1] == "INT8":
					rt = int(mots[3])
				elif mots[1] == "CHAR":
					if mots[2] == "GET_CANDIDATS":
						candidats = mots[3].split('/')
						rt = candidats
					rt = mots[3]
	return rt


def login(nom, mdp):
	msg = "LOGIN " + nom + " " + mdp
	send(msg)
	search = "LOGIN"

def disconnect():
	print("client fermer")
	client.close()

def get_candidats():
	send("GET_CANDIDATS")
	search = "GET_CANDIDATS"

def vote(vote):
	msg = "VOTE"
	for loop in range(len(candidats)):
		if candidats[loop] == vote:
			msg+='1'
		else:
			msg += '0'
	send(msg)
	search="VOTE"

