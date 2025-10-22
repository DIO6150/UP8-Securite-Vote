# **Bienvenue sur notre projet de système de vote chiffré et sécurisé**

__*Groupe H - Système de vote - Leo Gagey; Mael Éouzan; Neil Belhadj; Nikolas Podevin; Noé Choplin*__



# Protocole

## Init
Serveur -> Lit un fichier avec les informations, vote, date, candidat  
Serveur -> ouvert  

## Vote

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
Interface web par Nikolas Podevin

# Commandes

## Client

### Login


// TODO : peut etre le faire en auto parce que c'est une communication inutile demander
#### 1st Pass
```
clt: ASK_KEY

srv: RETURN O0 <server-public-key>
```

#### 2nd Pass
```
clt: LOGIN <id> <encrypted-pwd> <client-public-key> <pwd-sha256>

srv: RETURN O0				// OK, client connecté

srv: RETURN E0				// erreur, 
```

// TODO: Demander si on doit renvoyer la clé lors du vote
### Envoi d'un vote
```
clt: VOTE <encrypted-candidate_1> <encrypted-candidate_2> <encrypted-candidate_3> ... <encrypted-candidate_n>

srv: RETURN O0				// OK, vote enregistré
srv: RETURN O1				// OK, vote modifié
srv: RETURN E0				// erreur, pas assez de crypté pour le nombre de candidats
srv: RETURN E1				// erreur, trop de crypté pour le nombre de candidats
srv: RETURN E2				// erreur, cryptés invalides (somehow)
srv: RETURN E3				// erreur, client non connecté
```

// TODO: est ce que le serveur renvoi les candidats en cryptés

## Server
