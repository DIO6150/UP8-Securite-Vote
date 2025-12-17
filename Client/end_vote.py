import client
from time import sleep

sleep (0.5)
client.send ("SEND_KEY " + str(client.cle_public))

sleep (0.5)
client.send ("SEND_KEY_PROOF 1")

sleep (0.5)
client.send ("LOGIN M M")

sleep (0.5)
client.send ("VOTE_END")
