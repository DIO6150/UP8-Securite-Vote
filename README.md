# **Bienvenue sur notre projet de système de vote chiffré et sécurisé**

__*Groupe H - Système de vote - Leo Gagey; Mael Éouzan; Neil Belhadj; Nikolas Podevin; Noé Choplin*__

# Protocole

## Init
Serveur -> Lit un fichier avec les informations, vote, date, candidat  
Serveur -> ouvert  

## Idées

Client -> ce connect au serveur, preuve que c'est un votant et non un inconnu  
Serveur -> Accepte ou non si inscript sur les listes de vote et envoie si votant les candidats en public ou chiffré ?  

Client -> envoie ou non ses votes : 1 chiffrés randomisés par candidats dans l'ordre reçu, comment vérifier qu'il vote bien qu'une fois et non pour plusieurs candidats ?  
Serveur -> jusqu'à X temps, reste a recevoir les votes puis clos les votes et envoie au client la fermeture des votes  

Client -> reçois la fermeture des votes et donc ne peux plus modifier et attends le résultat  
Serveur -> addition des votes, déchiffrement, comment déchiffrer que l'addition et non un vote ?  

Serveur -> envoie les gagnants en signer  
Client -> reçois et vérifier la signature  

# Informations générales

Langage choisi :  Python (Client) | C++ (Serveur)
Interface en TKinter par Nikolas Podevin

# Notes

améliorer l'interface d'envoie de message au client pour pouvoir faire:
server.Send (client_socket, "#1# #2# ... #n#", args...);
plutot que server.Send (client_socket, StrArgs ("#1#", args...));
fonctionnellement identique mais moins verbose.

améliorer la bilbiothèque de str pretty parce que c'est le foutoir

# Protocole

## Notes


### Reconnections

Le serveur ne gère pas encore les reconnections
Cependant, j'immagine que c'est facile à implémenter:
le serveur envoie une clef de reconnection au client
quand un client se déconnecte, il le garde en mémoire, lui et sa clef de reconnection un certain nombre de temps.
si un client essaie de se connecter et envoie sa clef de reconnection,
le client lui accorde un nouvel ID internement (parce que l'ancien ID a pu être attribué par un autre client) et lui redonne ses perms sans rien demander de plus.
Essentiellement, la clef agit comme un cookie.
Attention cependant:
- la taille de la queue de client déconnecté en mémoire doit être fixe, pour ne pas faire exploser la mémoire du serveur
- Les clients qui sont connectés moins de X secondes ne doivent pas être enregistrés (DDOS & co)
- La clef de reconnection ne peut être attribuée qu'au moment d'une authentification complète

### Les étapes d'authentification:

- NoAuth : la socket client est connectée au serveur mais n'a rien envoyée.
- PubKeyKnown : la socket client à envoyée sa clef publique. Le serveur envoie une demande de confirmation de clef au client.
- PubKeyConfirmed : la socket client à répondu correctement à la demande de confirmation. Tous les messages suivants sont chiffrés avec RSA entre le serveur et le client.
- FullAuth : la socket client à envoyée son login et son mot de passe et ceux ci sont valides.

La validation de la clef publique client se fait de la manière suivante:
- la clef est de la bonne taille
- le serveur envoie un entier a € [0, n] chiffré. Le client doit renvoyer un entier b = a + 1. (On s'assure ainsi que le client ne s'est pas trompé en envoyant sa propre clef publique) On redemande la clef publique au client si cette étape n'est pas validée.

La validation du login se fait de la manière suivante:
- le mot de passe envoyé est hashé puis comparer au hash du mot de passe enregistrer pour l'utilisateur.
-- Si le nom d'utilisateur n'est pas enregistré on renvoie un code d'erreur et on ne déconnecte pas le client.
-- Si le mot de passe est invalide on renvoie un code d'erreur et on ne déconnecte pas le client




## Format de messages

Chaque échange entre le serveur et clients se fait par le biais de ```message```s qui constitue:
- un ```header``` de 32 bits qui donne la taille du ```body``` (envoyé en Big Endian).
- un ```body``` qui contient les données que le serveur et le client veulent s'échanger.

- Par la suite, une ```body``` sera constitué d'un ```hash``` de X bits et d'un ```message```. (A partir du moment ou un client est totalement identifié)

## Réponse du serveur

Réponse typique du serveur:
```
RETURN <DATA_TYPE> <command> <data>
```
- Où ```DATA_TYPE``` indique si le type de donnée que le serveur va envoyer
- Où ```command``` est le nom de la commande du client
- Où ```data``` est la réponse du serveur

### Data

#### Type de Données

- ```CODE```: code d'erreur
- ```INT8```: int sur 8-bits
- ```INT16```: int sur 16-bits
- ```INT32```: int sur 32-bits
- ```INT64```: int sur 64-bits
- ... (à déterminer au fur et à mesure)

##### Erreurs

- ```E0``` : Commande non reconnue
- ```E1``` : Utilisateur non identifié
- ```E2``` : Nom d'utilisateur invalide
- ```E3``` : Mot de passe invalide
- ```E4``` : Nombre de chiffrés invalides
- ```E5``` : Chiffré invalide
- ```E6``` : Client n'est pas administrateur
- ```E7``` : La clef envoyée par le client n'est pas valide
- ```E8``` : La preuve de clef n'est pas valide.

##### Oks

- ```O0``` : Utilisateur identifié avec succès
- ```O1``` : Vote prit en compte
- ```O2``` : 
- ```O3``` : 
- ```O4``` : 
- ```O5``` : 
- ```O6``` : 
- ```O7``` : 

## Connection

### Échange de clefs

Au moment où le client établi une connexion avec le serveur :
- Le serveur envoie sa clef publique.
- Le client chiffre sa clef publique avec la clef publique du serveur et l'envoie

À partir de maintenant, toutes les communications sont chiffrées avec RSA¹

### Identification

#### Client
```
clt: LOGIN <username> <pwd>
```

___
#### Serveur

```
srv: RETURN CODE LOGIN O0
```
L'utilisateur s'est identifié avec succès et est désormais en capacité de voter ou de changer son vote.
___
```
srv: RETURN CODE LOGIN E2
```
Le nom d'utilisateur est invalide
___
```
srv: RETURN CODE LOGIN E3
```
Le mot de passe est invalide

### Vote

#### Client

```
clt: VOTE <candidate_0> <candidate_1> <candidate_2> ... <candidate_n>
```

Chaque ```candidate_n``` est un chiffré homomorphe de pallier de 0 ou 1.

**La procédure n'est pas encore indiquée pour manque de recherche mais le serveur doit demander une ZKP au client**

#### Server

```
srv: RETURN CODE VOTE O1
```

Le vote a été pris en compte. Que ce soit une modification ou un premier vote.

___
```
srv: RETURN CODE VOTE E4
```

Le nombre de  chiffrés est invalide
___
```
srv: RETURN CODE VOTE E5
```

**Procédure ZKP**

### Commandes Administrateurs

```
a-clt: STOP
```
Arrête abruptement le serveur.

```
a-clt: VEND
```
Arrête le vote.

```
srv: RETURN CODE VEND E6
```

[¹] D'abord le message du client et ensuite le hash du message pour s'assurer de son authenticité.
Si l'authenticité du message ne peut être prouvée, la connection se ferme automatiquement.

___

** INFO **

serveur à besoin de la clé publique
client ont besoint de s'échanger des clefs privées sans que le serveur connaisse