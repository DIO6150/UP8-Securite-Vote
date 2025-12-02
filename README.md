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
