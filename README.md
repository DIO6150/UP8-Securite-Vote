# Bienvenue sur notre projet de système de vote chiffré et sécurisé

**Groupe H - Système de vote**
*Membres : Leo Gagey, Mael Éouzan, Neil Belhadj, Nikolas Podevin, Noé Choplin*

---

## 1. Vue d'ensemble

Ce projet vise à implémenter une architecture client-serveur sécurisée permettant des votes anonymes et vérifiables.

### Informations Générales
| Composant | Technologie | Détails |
| :--- | :--- | :--- |
| **Serveur** | C++ | Gestion des connexions, chiffrement RSA (GMP), logique de vote |
| **Client** | Python | Interface utilisateur, communication socket |
| **Interface** | Tkinter | Développée par Nikolas Podevin |

---

## 2. Architecture et Fonctionnement

### Initialisation
* **Serveur :** Lit un fichier de configuration (informations, vote, date, candidats) et ouvre le service d'écoute.

### Scénario de Vote (Workflow)

1.  **Connexion :** Le client se connecte au serveur.
2.  **Authentification :** Le client prouve qu'il est un votant légitime (inscrit sur les listes).
3.  **Réception des données :** Le serveur accepte la connexion et envoie la liste des candidats (chiffrée ou publique selon config).
4.  **Action de Vote :**
    * Le client envoie ses votes : 1 chiffré randomisé par candidat (dans l'ordre reçu).
    * *Challenge technique :* Vérification de l'unicité du vote (ZKP - Zero Knowledge Proof).
5.  **Fermeture :**
    * Le serveur attend la fin du temps imparti (X temps).
    * Il clôt les votes et notifie les clients.
6.  **Résultats :**
    * Le client ne peut plus modifier son vote et attend.
    * Le serveur additionne les votes et déchiffre le résultat (distinction entre addition et vote unique via chiffrement homomorphe).
    * Le serveur signe les gagnants et les envoie.
    * Le client reçoit et vérifie la signature.

---

## 3. Protocole de Communication

### Format des Messages
Chaque échange entre le serveur et les clients se fait par le biais de `message`s constitués de :
1.  **Header (32 bits, Big Endian) :** Taille du `body`.
2.  **Body :** Contient les données.
    * *Note :* Une fois authentifié, le `body` contient un `hash` (authenticité) + le `message`.

### Étapes d'Authentification (Machine à états)

1.  **NoAuth :** La socket est connectée mais n'a rien envoyé.
2.  **PubKeyKnown :** Le client a envoyé sa clé publique. Le serveur demande une confirmation.
3.  **PubKeyConfirmed :** Le client a répondu correctement à la demande de confirmation.
    * *Challenge :* Le serveur envoie un entier $a \in [0, n]$ chiffré. Le client doit renvoyer $b = a + 1$.
    * Désormais, les messages sont chiffrés avec RSA.
4.  **FullAuth :** Le socket client a envoyé son login/mot de passe et ceux-ci sont valides.

### Gestion des Reconnexions (TODO)
Le serveur doit gérer les reconnexions via une "clé de reconnexion" (cookie) :
* Si un client se déconnecte, le serveur garde en mémoire son ID et sa clé pour un temps donné.
* Si le client revient avec la clé, il récupère ses permissions et un nouvel ID interne.
* **Sécurité :** Taille de la file d'attente fixe et timeout pour éviter les DDOS.

---

## 4. API et Codes de Retour

Réponse typique du serveur : `RETURN <DATA_TYPE> <COMMAND> <DATA>`

### Types de données
`CODE` (erreur), `INT8`, `INT16`, `INT32`, `INT64`, etc.

### Table des Codes

| Code | Type | Description |
| :--- | :--- | :--- |
| **O0** | Succès | Utilisateur identifié avec succès |
| **O1** | Succès | Vote pris en compte |
| **E0** | Erreur | Commande non reconnue |
| **E1** | Erreur | Utilisateur non identifié |
| **E2** | Erreur | Nom d'utilisateur invalide |
| **E3** | Erreur | Mot de passe invalide |
| **E4** | Erreur | Nombre de chiffrés invalide |
| **E5** | Erreur | Chiffré invalide (Échec ZKP) |
| **E6** | Erreur | Client n'est pas administrateur |
| **E7** | Erreur | La clé envoyée par le client n'est pas valide |
| **E8** | Erreur | La preuve de clé n'est pas valide |

### Commandes Principales

#### Connection & Auth
* **Échange de clés :**
    * Srv -> Clé Publique Serveur
    * Clt -> Clé Publique Client (chiffrée avec clé serveur)
* **Login :**
    * `clt: LOGIN <username> <pwd>`
    * `srv: RETURN CODE LOGIN O0` (Succès) ou `E2`/`E3` (Erreur)

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
- ```CHAR``` : chaine de caractères
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
Les connections se font en ASCII

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

### candidat

#### Client

```
clt: CANDIDATS
```

#### Server

```
srv: RETURN CHAR CANDIDATS <nom_candidat_0>/<nom_candidat_1>/<nom_candidat_2>/ . . . <nom_candidat_n>
```


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
<<<<<<< HEAD
=======
Langage choisi :  Python (Client) | C++ (Serveur)  
Chiffrement par Léo Gagey (python c++)  
Server (C++ par Mael Eouzan, Neil Belhadj  
Client partie réseau (python) par Noé Choplin  
Tkinter par Nikolas Podevin  
=======

___

** INFO **

serveur à besoin de la clé publique
client ont besoint de s'échanger des clefs privées sans que le serveur connaisse




** RSA **

RSA

Actuellement le portage du programme depuis python vers GMP et C++ est à plus de 50%
Il reste à développer deux méthodes (méthodes de classe), une encode et une decode
Il faut mettre en place un mécanisme de remplissage d’un nombre n bits (2048 bits dans les tests) octet (8bits) par octet depuis une chaine de caractères (string car c++), une fois plein, encoder ou décoder ce gros nombre puis transformer le résultat à nouveau en chaine de caractères. Continuer cette opération tant qu’il reste des octets dans la string (stream) en entrée. L’idée est d’avoir du string clair TO string chiffré et inversement … en y réfléchissant bien une seule même fonction ferait aussi l’affaire car la seule chose qui changerait serait le paire (e, n) ou (d, n)

##Organisation actuelle du code : ## 
Le coeur des fonctions (méthodes de classe — car static (inline en plus)) se trouve dans rsa.h. Dans rsa.c il n’y a qu’un test
Principalement 2 structures (dans le rsa.h) :
pair_t qui est juste un couplage de deux mpz_class, pour faire une paire pour chaque clé (c’est simplement un typedef)
Un mpz_class est une surcouche c++ du type C mpz_t (gros entier dans Z). La classe sert à permettre l’utilisation plus facile à comprendre des opérateurs +, -, *, … le mpz_t sert pour utiliser certaines fonctions qui ne sont disponibles qu’en C, comme par exemple mpz_powm (puissance modulaire) ; on utilise alors la méthode get_mpz_t() pour passer les paramètres à la fonction
Les méthodes ont été créées de classe (static) pour ne pas avoir besoin d’instancier quoi que ce soit (la programmation en paradigme objet n’est pas très utile pour un problème comme ça). Ainsi, à l’extérieur de  la classe, l’appel de ces méthodes se par rsa::nom_de_la_methode(…). Certaines de ces méthodes pourraient être private car au final, depuis l’extérieur de la classe, on aurait juste besoin de la méthode genKeyPair(2048) qui génère et renvoie les deux paires de clés, et d’une méthode qui code/décode un message en renvoyant un message ; ça pourrait être :
string msgCode(string msg, pair_t key);
A faire : 
tout re-parcourir le code et éventuellement ajouter des commentaires pour bien comprendre la correspondance entre les fonctionnalités python et celles proposées par GMP ;
Commencer à implémenter la méthode string msgCode(string msg, pair_t key); en s’inspirant de la fonction encode du code python. Attention, cette dernière renvoie un tableau de gros entiers (c’était un mauvais choix), il faut plutôt refabriquer une chaîne d’octets (string ??? Car potentiel problème est qu’un octet à zéro soit généré lors de l’encodage, dans ce cas il signifierait la fin de chaîne alors que ça n’est pas le cas … ou peut-être que string possède une connaissance de sa longueur auquel cas pas de problème et pas besoin du \0 pour la fin de chaîne)
=======
#### Vote
* **Action :**
    * `clt: VOTE <candidat_0> <candidat_1> ... <candidat_n>`
    * *Note :* Chaque candidat est un chiffré homomorphe de pallier 0 ou 1.
* **Réponse :**
    * `srv: RETURN CODE VOTE O1` (Pris en compte)
    * `srv: RETURN CODE VOTE E5` (Erreur ZKP)

#### Administration
* `a-clt: STOP` : Arrêt abrupt du serveur.
* `a-clt: VEND` : Arrêt du vote.

---

## 5. Détails Techniques : RSA et C++

Le portage du code Python vers GMP/C++ est en cours (>50%).

### Structure du code
Le cœur des fonctions se trouve dans `rsa.h` (méthodes statiques).
* **`pair_t` :** Typedef couplant deux `mpz_class` pour les clés.
* **`mpz_class` :** Surcouche C++ du type `mpz_t` de GMP (gestion des grands entiers).

### Méthodes clés
* `rsa::genKeyPair(2048)` : Génère et renvoie les deux paires de clés.
* `string msgCode(string msg, pair_t key)` : Encode/Décode un message.

### Implémentation (En cours)
Il reste à finaliser la méthode `msgCode` :
1.  Mécanisme de remplissage d'un nombre $n$ bits octet par octet depuis une `string`.
2.  Une fois plein, encoder/décoder le grand nombre.
3.  Transformer le résultat en `string`.
4.  Boucler tant qu'il reste des octets dans le stream d'entrée.

---

## 6. Notes et TODOs

* [ ] **Interface Socket :** Améliorer l'envoi de message pour permettre `server.Send(client_socket, "#1# #2#", args...);` (style variadique) au lieu de construire les chaînes manuellement.
* [ ] **Pretty Print :** Refactoriser la bibliothèque de `str pretty` (actuellement désordonnée).
* [ ] **ZKP :** Recherche et implémentation de la procédure Zero-Knowledge Proof pour valider les chiffrés du vote (0 ou 1).
