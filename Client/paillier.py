import random
import hashlib


def pgcd(a, b):
    while b:
        a, b = b, a % b
    return a


def powmod(base, exp, mod):
    res = 1
    base = base % mod
    while exp > 0:
        if exp % 2 == 1:
            res = (res * base) % mod
        exp = exp >> 1
        base = (base * base) % mod
    return res


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


def ppcm(a, b):
    return abs(a * b) // pgcd(a, b)


def is_prime(n, k=10):
    """Test de primalité de Miller-Rabin

    Args:
        n: Nombre à tester
        k: Nombre d'itérations

    Returns:
        True si n est premier
    """
    if n < 2:
        return False
    if n == 2 or n == 3:
        return True
    if n % 2 == 0:
        return False

    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2

    for _ in range(k):
        a = random.randrange(2, n - 1)
        x = powmod(a, d, n)

        if x == 1 or x == n - 1:
            continue

        for _ in range(r - 1):
            x = powmod(x, 2, n)
            if x == n - 1:
                break
        else:
            return False

    return True


def generate_prime(bits):
    """Génère un nombre premier de taille donnée

    Args:
        bits: Nombre de bits du nombre premier

    Returns:
        Un nombre premier de "bits" bits
    """
    while True:
        n = random.getrandbits(bits)
        n |= (1 << bits - 1) | 1  # impair et de taille bits
        if is_prime(n):
            return n


def generate_keypair(bits=2048):
    """Génère une paire de clés Paillier

    Args:
        bits: Taille en bits de chaque nombre premier

    Returns:
        (public_key, private_key) avec:
            public_key = (n, g)
            private_key = (lambda, mu)
    """
    # Générer deux nombres premiers distincts
    p = generate_prime(bits)
    q = generate_prime(bits)
    while p == q:
        q = generate_prime(bits)

    n = p * q
    g = n + 1
    lambda_val = ppcm(p - 1, q - 1)

    def l(x):
        return (x - 1) // n

    n_squared = n * n
    g_lambda = powmod(g, lambda_val, n_squared)
    mu = modinv(l(g_lambda), n)

    public_key = (n, g)
    private_key = (lambda_val, mu)

    return public_key, private_key


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


def paillier_decrypt(c, public_key, private_key):
    """Déchiffre un chiffré Paillier

    Args:
        c: Chiffré
        public_key: Clé publique (n, g)
        private_key: Clé privée (lambda, mu)

    Returns:
        Message déchiffré
    """
    n, g = public_key
    lambda_val, mu = private_key
    n_squared = n * n

    def l(x):
        return (x - 1) // n

    # m = L(c^lambda mod n^2) * mu mod n
    c_lambda = powmod(c, lambda_val, n_squared)
    m = (l(c_lambda) * mu) % n

    return m


def paillier_add(c1, c2, public_key):
    """Addition homomorphe de deux chiffrés

    Args:
        c1, c2: Chiffrés Paillier
        public_key: Clé publique (n, g)

    Returns:
        c = Enc(m1 + m2) avec c1 = Enc(m1) et c2 = Enc(m2)
    """
    n, g = public_key
    n_squared = n * n
    return (c1 * c2) % n_squared


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


def zkp_verify(c, proof, public_key, t=128):
    """Vérifie une preuve ZKP

    Args:
        c: Chiffré
        proof: Preuve générée par zkp_prove
        public_key: Clé publique (n, g)
        t: Taille du challenge en bits

    Returns:
        True si la preuve est valide, False sinon
    """
    n, g = public_key
    n_squared = n * n

    a_values = proof['a_values']
    z_values = proof['z_values']
    e_values = proof['e_values']
    e_total = proof['e']
    valid_values = proof['valid_values']

    ell = len(a_values)

    # Recalcul du challenge
    hash_input = ''.join(str(a) for a in a_values).encode()
    hash_output = hashlib.sha256(hash_input).hexdigest()
    e_computed = int(hash_output, 16) % (2**t)

    if e_computed != e_total:
        return False

    # e = e_1 + ... + e_ℓ mod 2^t
    sum_e = sum(e_values) % (2**t)
    if sum_e != e_total:
        return False

    # Pour chaque i, vérifier z_i^n = a_i * (c * g^(-m_i))^e_i mod n^2
    for i in range(ell):
        m_i = valid_values[i]

        # z_i^n mod n^2
        z_n = powmod(z_values[i], n, n_squared)

        # g^m_i mod n^2
        g_m = powmod(g, m_i, n_squared)

        # g^(-m_i) = (g^m_i)^(-1) mod n^2
        g_minus_m = modinv(g_m, n_squared)

        # (c * g^(-m_i)) mod n^2
        term = (c * g_minus_m) % n_squared

        # (c * g^(-m_i))^e_i mod n^2
        term_e = powmod(term, e_values[i], n_squared)

        # a_i * (c * g^(-m_i))^e_i mod n^2
        right_side = (a_values[i] * term_e) % n_squared

        # Vérification
        if z_n != right_side:
            return False

    return True
