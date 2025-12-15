# Projet de Système de Vote Chiffré et Sécurisé

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

### Scénario de Vote

1.  **Connexion :** Le client se connecte au serveur.
2.  **Authentification :** Le client prouve qu'il est un votant légitime (inscrit sur les listes).
3.  **Réception des données :** Le serveur accepte la connexion et envoie la liste des candidats (chiffrée ou publique selon config).
4.  **Action de Vote :**
    * Le client envoie ses votes : 1 chiffré randomisé par candidat (dans l'ordre reçu).
    * *Challenge technique :* Vérification de l'unicité du vote (ZKP - Zero Knowledge Proof).
    * **Zero-Knowledge Proof (ZKP) :**
        - Le client envoie un seul vote chiffré $c$, accompagné de plusieurs triplets de preuve $(\text{a}_j, \text{z}_j, \text{e}_j)$ pour tous les messages possibles.
        - Chaque triplet contient :
            * $\text{a}_j$ : l'engagement, une valeur qui s'engage à un message sans le révéler.
            * $\text{e}_j$ : le défi partiel, un morceau du défi global. La somme de tous les $\text{e}_j$ doit donner le défi global, qui est calculé comme le hash de tous les engagements $\text{a}_j$ et du chiffré $c$.
            * $\text{z}_j$ : le morceau de preuve, un nombre qui satisfait l'équation :
            $$
            \text{z}_j^n \equiv \text{a}_j \cdot (c \cdot g^{-\text{m}_j})^{\text{e}_j} \mod n^2
            $$
            Pour le vrai vote, $\text{z}_j$ est calculé à partir du secret réel (l’aléa $\text{r}_i$), tandis que pour les autres votes simulés, $\text{z}_j$ est construit pour que l'équation soit correcte sans révéler le vote réel.
        - Le serveur reçoit $c$ et tous les triplets, recalcule le défi global, vérifie que la somme des $\text{e}_j$ correspond au défi global, puis teste l’équation pour chaque triplet.
        - Si toutes les vérifications sont correctes, le vote est validé, sans que le serveur connaisse quel message était réel.
5.  **Fermeture :**
    * Le serveur attend la fin du temps imparti (X temps).
    * Il clôt les votes et notifie les clients.
6.  **Résultats :**
    * Le client ne peut plus modifier son vote et attend.
    * Le serveur additionne les votes et déchiffre le résultat.
    * Le serveur signe les gagnants et les envoie.
    * Le client reçoit et vérifie la signature.

---

## 3. Protocole de Communication

### Format des Messages
Chaque échange entre le serveur et les clients se fait par le biais de `message`s constitués de :
1.  **Header (32 bits, Big Endian) :** Taille du `body`.
2.  **Body :** Contient les données.
    * *Note :* Une fois authentifié, le `body` contient un `hash` + le `message`.

### Status d'Authentification du Client

1.  **NoAuth :** La socket est connectée mais n'a rien envoyé.
2.  **PubKeyKnown :** Le client a envoyé sa clé publique. Le serveur demande une confirmation.
3.  **PubKeyConfirmed :** Le client a répondu correctement à la demande de confirmation.
4.  **FullAuth :** Le socket client a envoyé son login/mot de passe et ceux-ci sont valides.

### Connection & Authentification

- Échange de clef
    - Au moment ou le client se connecte, le serveur envoie sa clef publique en brut. (La clef est convertie en string)
    - Le client envoie alors sa propre clef publique chiffrée avec la clef publique du serveur.

- Validation de clef
    - La clef est de la bonne taille aka 2048 bits
    - Le serveur envoie un entier $a \in [0, +\infty[$ chiffré. Le client doit renvoyer $b = a + 1$.

- Validation d'authentification
    - Le mot de passe envoyé est hashé puis comparer au hash du mot de passe enregistrer pour l'utilisateur.
    - Si le nom d'utilisateur n'est pas enregistré on renvoie un code d'erreur
    - Si le mot de passe est invalide on renvoie un code d'erreur

### Gestion des Reconnexions (TODO)
Le serveur doit gérer les reconnexions via une "clé de reconnexion" (cookie) :
* Si un client se déconnecte, le serveur garde en mémoire son ID et sa clé pour un temps donné.
* Si le client revient avec la clé, il récupère ses permissions et un nouvel ID interne.
* **Sécurité :** Taille de la file d'attente fixe et timeout pour éviter les DDOS.

---

## 4. API et Codes de Retour

Réponse typique du serveur : `RETURN <DATA_TYPE> <COMMAND> <DATA>`

- Où ```DATA_TYPE``` indique si le type de donnée que le serveur va envoyer
- Où ```COMMAND``` est le nom de la commande du client
- Où ```DATA``` est la réponse du serveur

### Types de données
`CODE`, `INT8`, `PUB_KEY`, `CHAR`.

### Table des Codes

| Code | Type | Description |
| :--- | :--- | :--- |
| **O0** | Succès | Utilisateur identifié avec succès |
| **O1** | Succès | Vote pris en compte |
| **O2** | Succès | Clef validée par le serveur |
| **O3** | Succès | Clef reçue par le serveur |
| **E0** | Erreur | Commande non reconnue |
| **E1** | Erreur | Utilisateur non identifié |
| **E2** | Erreur | Nom d'utilisateur invalide |
| **E3** | Erreur | Mot de passe invalide |
| **E4** | Erreur | Nombre d'arguments invalides |
| **E5** | Erreur | Chiffré invalide (Échec ZKP) |
| **E6** | Erreur | Client n'est pas administrateur |
| **E7** | Erreur | La clé envoyée par le client n'est pas valide |
| **E8** | Erreur | La preuve de clé n'est pas valide |
| **E9** | Erreur | Client déjà identifié |
| **E10** | Erreur | Un vote est déjà en cours |
| **E11** | Erreur | La clef n'a pas été validée au stade initial d'authentification du client |
| **E12** | Erreur | Le client n'en est plus à ce stade d'authentification |
| **E13** | Erreur | Il n'y a pas de votes en cours |


## Protocole

#### Serveur envoie sa clef

* **Action :**
    * `srv: SEND_KEY <key>`
        * `key` (clef publique du serveur)

* **Réponse :**
    * `clt: SEND_KEY <key>`
        * `key` (clef publique du client, chiffrée avec la clef publique du serveur)

#### Serveur demande une preuve de clef

* **Action :**
    * `srv: SEND_KEY_PROOF <key_proof>`
        * `key_proof` (entier $a \in [0, +\infty[$)

* **Réponse :**
    * `clt: SEND_KEY_PROOF <key_proof>`
        * `key_proof` (entier $b = a + 1$)

#### Client envoie sa clef

* **Action :**
    * `clt: SEND_KEY <key>`
        * `key` (clef publique du client chiffrée)

* **Réponse :**
    * `srv: RETURN CODE SEND_KEY O3` (la clef a été reçue par le serveur)
    * `srv: RETURN CODE SEND_KEY E11` (la clef n'a pas été validée par le serveur)

#### Client envoie sa preuve de clef

* **Action :**
    * `clt: SEND_KEY_PROOF <key_proof>`
        * `key_proof` (entier)

* **Réponse :**
    * `srv: RETURN CODE SEND_KEY O2` (la clef a été validée par le serveur)
    * `srv: RETURN CODE SEND_KEY E8` (la clef n'a pas été validée par le serveur)

#### Client envoie sa clef de paillier

* **Action :**
    * `clt: SEND_KEY_PAILLIER <n> <g>`
        * `n` partie n de la clé
        * `g` partie g de la clé

#### Se connecter

* **Action :**
    * `clt: LOGIN <username> <pwd>`
        * `username` (nom d'utilisateur)
        * `pwd` (mot de passe)

* **Réponse:**
    * `srv: RETURN CODE LOGIN O0` (L'utilisateur s'est identifié avec succès et est désormais en capacité de voter ou de changer son vote.)
    * `srv: RETURN CODE LOGIN E2` (Le nom d'utilisateur est invalide)
    * `srv: RETURN CODE LOGIN E3` (Le mot de passe est invalide)

#### Récupérer la liste des candidats

* **Action :**
    * `clt: GET_CANDIDATS`

* **Réponse :**
    * `srv: RETURN CHAR GET_CANDIDATS <candidate_0> <candidate_1> ... <candidate_n>`

#### Voter

* **Action :**
    * `clt: VOTE <candidat_0> <candidat_1> ... <candidat_n>`
    * *Note :* Chaque candidat est un chiffré homomorphe de pallier 0 ou 1.
* **Réponse :**
    * `srv: RETURN CODE VOTE O1` (Pris en compte)
    * `srv: RETURN CODE VOTE E4` (Nombre de candidats invalides)
    * `srv: RETURN CODE VOTE E5` (Erreur ZKP)

#### Serveur envoie le résultat des votes

* **Action :**
    * `srv: SEND_VOTE_RESULT <result_1> ... <result_n>`
        * `result_n` (le résultat pour candidat n)


## Commandes Administrateurs

* **Action :**
    * `a-clt: STOP` (Arrête abruptement le serveur.)
    * `a-clt: VOTE_BEGIN <candidate_0> <candidate_1> ... <candidate_n>` (Commence la procédure de vote)
    * `a-clt: VOTE_END` (Arrête le vote)

* **Réponse :**
    * `srv: RETURN CODE VOTE_END E6` (Si le client n'est pas administrateur)
___

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

* [ ] **Interface Socket :** Améliorer l'envoi de message pour permettre `server.Send(client_socket, "#1# #2#", args...);`  au lieu de construire les chaînes manuellement.
* [ ] **Pretty Print :** Refactoriser la bibliothèque de `str pretty` (actuellement désordonnée).
* [ ] **ZKP :** Recherche et implémentation de la procédure Zero-Knowledge Proof pour valider les chiffrés du vote (0 ou 1).


___
___
___
___

## RSA

Actuellement le portage du programme depuis python vers GMP et C++ est à plus de 50%
Il reste à développer deux méthodes (méthodes de classe), une encode et une decode
Il faut mettre en place un mécanisme de remplissage d’un nombre n bits (2048 bits dans les tests) octet (8bits) par octet depuis une chaine de caractères (string car c++), une fois plein, encoder ou décoder ce gros nombre puis transformer le résultat à nouveau en chaine de caractères. Continuer cette opération tant qu’il reste des octets dans la string (stream) en entrée. L’idée est d’avoir du string clair TO string chiffré et inversement … en y réfléchissant bien une seule même fonction ferait aussi l’affaire car la seule chose qui changerait serait le paire (e, n) ou (d, n)

## Organisation actuelle du code :

Le coeur des fonctions (méthodes de classe - car static (inline en plus)) se trouve dans rsa.h. Dans rsa.c il n’y a qu’un test
Principalement 2 structures (dans le rsa.h) :
pair_t qui est juste un couplage de deux mpz_class, pour faire une paire pour chaque clé (c’est simplement un typedef)
Un mpz_class est une surcouche c++ du type C mpz_t (gros entier dans Z). La classe sert à permettre l’utilisation plus facile à comprendre des opérateurs +, -, *, … le mpz_t sert pour utiliser certaines fonctions qui ne sont disponibles qu’en C, comme par exemple mpz_powm (puissance modulaire) ; on utilise alors la méthode get_mpz_t() pour passer les paramètres à la fonction
Les méthodes ont été créées de classe (static) pour ne pas avoir besoin d’instancier quoi que ce soit (la programmation en paradigme objet n’est pas très utile pour un problème comme ça). Ainsi, à l’extérieur de  la classe, l’appel de ces méthodes se par rsa::nom_de_la_methode(…). Certaines de ces méthodes pourraient être private car au final, depuis l’extérieur de la classe, on aurait juste besoin de la méthode genKeyPair(2048) qui génère et renvoie les deux paires de clés, et d’une méthode qui code/décode un message en renvoyant un message ; ça pourrait être :
string msgCode(string msg, pair_t key);
A faire : 
tout re-parcourir le code et éventuellement ajouter des commentaires pour bien comprendre la correspondance entre les fonctionnalités python et celles proposées par GMP ;
Commencer à implémenter la méthode string msgCode(string msg, pair_t key); en s’inspirant de la fonction encode du code python. Attention, cette dernière renvoie un tableau de gros entiers (c’était un mauvais choix), il faut plutôt refabriquer une chaîne d’octets (string ??? Car potentiel problème est qu’un octet à zéro soit généré lors de l’encodage, dans ce cas il signifierait la fin de chaîne alors que ça n’est pas le cas … ou peut-être que string possède une connaissance de sa longueur auquel cas pas de problème et pas besoin du \0 pour la fin de chaîne)

---
