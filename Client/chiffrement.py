import random
import hashlib

def pgcd(a, b):
    while b:
        a, b = b, a % b
    return a


def powmod(base, exp, mod):
    return pow(base, exp, mod)


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


def generate_rsa_keypair(bits=2048):
    p = generate_prime(bits)
    q = generate_prime(bits)
    while p == q:
        q = generate_prime(bits)
    n = p * q
    phi = (p - 1) * (q - 1)

    e = 65537
    d = modinv(e, phi)
    return (n, e), (n, d)


def rsa_sign(m, private_key):
    n, d = private_key
    h = hashlib.sha256(m).digest()
    h_int = int.from_bytes(h, 'big')
    sig = pow(h_int, d, n)
    return sig


def rsa_verify(m, sig, public_key):
    n, e = public_key
    h = hashlib.sha256(m).digest()
    h_int = int.from_bytes(h, 'big')
    check = pow(sig, e, n)
    return check == h_int
