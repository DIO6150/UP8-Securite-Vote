import socket
import chiffrement
import queue
import threading
server_cle_public = 2

def send(msg):
	if(type(msg)==int):
		msg_bytes = msg.to_bytes(4, 'big')
	else:
		msg_bytes = msg.encode('ASCII')
	msg_chiffre = chiffrement.rsa(server_cle_public,msg_bytes)
	size = len(msg_chiffre).to_bytes(4, 'big')
	client.sendall(size + msg_chiffre)

def listen():
	while(True):
		size_bytes = client.recv(4)
		if(len(size_bytes) != 0):
			msg_size = int.from_bytes(size_bytes, 'big')
			msg_chiffre = client.recv(msg_size)
			msg_dechiffre = chiffrement.rsa(cle_private, msg_chiffre).decode('ASCII')
			responses.put(msg_dechiffre) # queue est thread safe


client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

cle_public=1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
cle_private=1
candidats = 0

client.connect(('localhost', 12345))
search = ""
responses = queue.Queue()
connected = False

def connect():
	global client, connected, listen_thread
	try:
		client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		client.connect(('10.74.228.109', 12345))
		connected = True
		listen_thread = threading.Thread(target=listen, daemon=True)
		listen_thread.start()
		return True
	except Exception as e:
		print(f"Connection failed: {e}")
		connected = False
		return False

# Initial connection attempt is now manual
# client.connect(('10.74.228.109', 12345))
# listen_thread = threading.Thread(target=listen, daemon=True)
# listen_thread.start()


def read():
	global search
	global recu
	rt = "E"
	while(not responses.empty()):
		message = responses.get_nowait()
		mots = message.upper().split()
		print("debug " + str(mots) + " " + search)
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
		elif mots[0] == "SEND_KEY":
			cle_serv = mots[1]
			send("SEND_KEY "+str(cle_public))
			search = mots[0]
		elif mots[0] == "SEND_KEY_PROOF":
			send("SEND_KEY_PROOF " + str(int(mots[1])+1))
			search = mots[0]
	return rt

def login(nom, mdp):
	global search
	msg = "LOGIN " + nom + " " + mdp
	send(msg)
	search = "LOGIN"

def disconnect():
	print("client fermer")
	client.close()

def get_candidats():
	global search
	send("GET_CANDIDATS")
	search = "GET_CANDIDATS"

def vote(vote):
	global search
	msg = "VOTE"
	for loop in range(len(candidats)):
		if candidats[loop] == vote:
			msg+='1'
		else:
			msg += '0'
	send(msg)
	search="VOTE"

