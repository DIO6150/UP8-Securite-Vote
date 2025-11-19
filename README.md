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

# Protocole

## Format de messages

Chaque échange entre le serveur et clients se fait par le biais de ```message```s qui constitue:
- un ```header``` de 32 bits qui donne la taille du ```body``` (envoyé en Big Endian)
- un ```body``` qui contient les données que le serveur et le client veulent s'échanger.

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
- ```E7``` : 

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