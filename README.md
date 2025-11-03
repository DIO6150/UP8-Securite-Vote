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


**ATTENTION LES COMMANDES QUI SUIVENT NE SONT PAS VALIDES**
# Commandes

__Note: Chaque commande sera chiffrée par RSA__

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

ON SIGNE AVEC RSA PAS SHA 256 RAAAAAAHHHHH
on peut renvoyer somme par ligne et par colonne des bulletins pour les vérifications


Client dit en clair au serveur: "je veux voter"
Le serveur répond sa clef publique
Le client chiffre sa clef publique avec la clef publique du serveur
le serveur déchiffre avec sa clef privée
paf échange de clefs

# Protocole

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

- ```E0``` : 
- ```E1``` : 
- ```E2``` : 
- ```E3``` : 
- ```E4``` : 
- ```E5``` : 
- ```E6``` : 
- ```E7``` : 

##### Oks

- ```O0``` : 
- ```O1``` : 
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
clt: LOGIN <username> <hashed_pwd>
```

___
#### Serveur

```
RETURN CODE LOGIN O0
```
L'utilisateur s'est identifié avec succès et est désormais en capacité de voter ou de changer son vote.
___
```
RETURN CODE LOGIN E1
```
Le nom d'utilisateur est invalide
___
```
RETURN CODE LOGIN E2
```
Le mot de passe est invalide

### Vote

#### Client
```
```
___
[¹] D'abord le message du client et ensuite le hash du message pour s'assurer de son authenticité.
Si l'authenticité du message ne peut être prouvée, la connection se ferme automatiquement.
