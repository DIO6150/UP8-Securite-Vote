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
candidats = 0

client.connect(('localhost', 12345))
server_cle_public = int.from_bytes(client.recv(msg_size)) #taille fix
print("Réponse du serveur :", 	server_cle_public)

cle_public_chiffre = rsa(server_cle_public, cle_public.to_bytes(4,'big'))
client.sendall(cle_public_chiffre)
search = ""
essais = 0

def read():
	if(queue.Empty):
		essais +=1
		return "E"
	message = responses.get()
	responses.task_done()
	mots = message.upper().split()
	if mots[0] == "RETURN":
		if mots[2] == search:
			if mots[1] == "CODE":
				return mots[3]
			elif mots[1] == "INT8":
				return int(mots[3])
			elif mots[1] == "CHAR":
				if mots[2] == "CANDIDATS":
					candidats = mots[3].split('/')
					return candidats
				return mots[3]
	return "E"


def login(nom, mdp):
	msg = "LOGIN " + nom + " " + mdp
	send(msg)
	search = "LOGIN"

def disconnect():
	client.close()

def get_candidats():
	send("CANDIDATS")
	search = "CANDIDATS"

def vote(vote):
	msg = "VOTE"
	for loop in range(len(candidats)):
		if candidats[loop] == vote:
			msg+='1'
		else:
			msg += '0'
	send(msg)
	search="VOTE"

client.close()
