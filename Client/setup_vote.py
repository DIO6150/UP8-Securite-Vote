import client
from time import sleep

sleep (0.5)
client.send ("SEND_KEY " + str(client.cle_public))

sleep (0.5)
client.send ("SEND_KEY_PROOF 1")

sleep (0.5)
client.send ("LOGIN M M")

sleep (0.5)
client.send ("VOTE_BEGIN BADASS_MCBADASSON FRANK_LEBOEUF BERNARD_MINET MAITRE_GIMS_SANS_LUNETTES KAENU_REEVES")

sleep (0.5)
client.send ("SEND_KEY_PAILLIER " + str(client.cle_paillier[0][0]) + " " + str(client.cle_paillier[0][1]))

