import random
import hashlib


def pgcd(a, b):
    while b:
        a, b = b, a % b
    return a


def powmod(base, exp, mod):
    return pow(base, exp, mod)
    """res = 1
    base = base % mod
    while exp > 0:
        if exp % 2 == 1:
            res = (res * base) % mod
        exp = exp >> 1
        base = (base * base) % mod
    return res"""


def modinv(a, m):
    if pgcd(a, m) != 1:
        return None

    u1, u2, u3 = 1, 0, a
    v1, v2, v3 = 0, 1, m
    while v3 != 0:
        q = u3 // v3
        v1, v2, v3, u1, u2, u3 = (
            u1 - q * v1, u2 - q * v2, u3 - q * v3, v1, v2, v3
        )

    return u1 % m


def paillier_encrypt(m, public_key):
    """Chiffre un message avec Paillier

    Args:
        m: Message à chiffrer (int)
        public_key: Clé publique (n, g)

    Returns:
        (c, r): Chiffré c et aléa r
    """
    n, g = public_key
    n_squared = n * n

    # Choisir un aléa r inversible modulo n
    while True:
        r = random.randrange(1, n)
        if pgcd(r, n) == 1:
            break

    # c = g^m * r^n mod n^2
    c = (powmod(g, m, n_squared) * powmod(r, n, n_squared)) % n_squared

    return c, r


def zkp_prove(c, m, r, public_key, valid_values=[0, 1], t=128):
    """Génère une preuve ZKP non-interactive

    Prouve que c = Enc(m) où m ∈ valid_values, sans révéler m.

    Args:
        c: Chiffré à prouver
        m: Message chiffré
        r: Aléa utilisé pour le chiffrement
        public_key: Clé publique (n, g)
        valid_values: Liste des valeurs valides
        t: Taille du challenge en bits

    Returns:
        proof: Dict contenant la preuve ZKP avec:
            - a_values: Engagements
            - z_values: Réponses
            - e_values: Défis partiels
            - e: Challenge total
            - valid_values: Valeurs valides
    """
    n, g = public_key
    n_squared = n * n
    ell = len(valid_values)

    # Vérifier que m est valide
    try:
        i = valid_values.index(m)
    except ValueError as exc:
        raise ValueError(
            f"La valeur {m} n'est pas dans {valid_values}") from exc

    # Initialisation
    z_values = [None] * ell
    e_values = [None] * ell
    a_values = [None] * ell

    # Phase 1
    # Pour j != i (faux messages), simuler la preuve
    for j in range(ell):
        if j != i:
            # Choisir z_j inversible modulo n
            while True:
                z_values[j] = random.randrange(1, n)
                if pgcd(z_values[j], n) == 1:
                    break

            # Choisir e_j aléatoire dans [0, 2^t - 1]
            e_values[j] = random.randrange(0, 2**t)

            # Calculer a_j = z_j^n * (g^m_j * c^(-1))^e_j mod n²
            m_j = valid_values[j]
            c_inv = modinv(c, n_squared)
            g_m_j = powmod(g, m_j, n_squared)
            term = (g_m_j * c_inv) % n_squared

            a_values[j] = (powmod(z_values[j], n, n_squared) *
                           powmod(term, e_values[j], n_squared)) % n_squared

    # Pour vrai message, engagement honnête
    while True:
        s = random.randrange(1, n)
        if pgcd(s, n) == 1:
            break

    a_values[i] = powmod(s, n, n_squared)

    # Phase 2
    # e = Hash(a_1, ..., a_ℓ) mod 2^t
    hash_input = ''.join(str(a) for a in a_values).encode()
    hash_output = hashlib.sha256(hash_input).hexdigest()
    e = int(hash_output, 16) % (2**t)

    # Phase 3
    # e_i = e - sum(e_j pour j != i) mod 2^t
    sum_e = sum(e_values[j] for j in range(ell) if j != i)
    e_values[i] = (e - sum_e) % (2**t)

    # z_i = s * r^e_i mod n
    z_values[i] = (s * powmod(r, e_values[i], n)) % n

    return {
        'a_values': a_values,
        'z_values': z_values,
        'e_values': e_values,
        'e': e,
        'valid_values': valid_values
    }
