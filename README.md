# Bienvenue sur notre projet de système de vote chiffré et sécurisé

Groupe H - Système de vote - Leo Gagey; Mael Éouzan; Neil Belhadj; Nikolas Podevin; Noé Choplin


# Protocole

## Init
Serveur -> Lit un fichier avec les informations, vote, date, candidat  
Serveur -> ouvert  

## Système de vote

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