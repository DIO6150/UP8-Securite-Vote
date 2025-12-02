# implémentation d'un RSA (générer les clés, chiffrer et déchiffrer)
# en python
# Auteur : Neil Bel hadj

from Crypto.Util import number as crypto
import random


def gen_rsa_keypair(bits):
    p = crypto.getPrime(bits//2)
    q = crypto.getPrime(bits//2)
    n = p * q
    phi_n = (p-1) * (q-1)
    e = 1
    while crypto.GCD(phi_n, e) != 1 or e < 65537:
        e = random.randint(65537, 1 << 17) % n
    d = crypto.inverse(e, phi_n)
    return ((e, n), (d, n))


def code(a, key) -> int:
    # return (a ** key[0]) % key[1]
    return pow(a, key[0], key[1])


def encode(clr_msg, pub_key, bits=2048):
    enc_msg = []
    fill = 0
    number = 0
    for c in clr_msg:
        # enfiler les octets
        number = number * 256 + ord(c)
        fill += 1
        # si le grand nombre de 2048 bits est plein
        if fill == (bits / 8):
            # le chiffrer et ajouter
            enc_msg.append(code(number, pub_key))
            fill = 0
            number = 0
    # si le grand nombre de 2048 bits reste partiellement plein
    if fill != 0:
        # le chiffrer et l'ajouter
        enc_msg.append(code(number, pub_key))
    return enc_msg


def decode(enc_msg, priv_key):
    clr_msg = ""
    lem = len(enc_msg)
    for i in range(lem):
        number = code(enc_msg[lem - 1 - i], priv_key)
        while number > 0:
            c = number % 256
            number = number // 256
            clr_msg = chr(c) + clr_msg
    return clr_msg


def initif():
    test_msg = "Le fait que l’ouvrier se présente face à l’argent et \
    offre sa puissance de travail comme une marchandise à vendre \
    sous-entend : 1) que les conditions de travail, les conditions \
    objectives du travail sont face à lui comme des puissances \
    étrangères (fremde Mächte), des conditions qui lui sont rendues \
    étrangères (entfremdet). Propriété d’autrui (fremdes Eigentum)."
    print("message d'origine :", test_msg)
    pub_priv_keys = gen_rsa_keypair(2048)
    # print(pub_priv_keys)
    encm = encode(test_msg, pub_priv_keys[0])
    print("message chiffré :", encm)
    decm = decode(encm, pub_priv_keys[1])
    print("message déchiffré :", decm)


initif()


# pub_priv_keys = gen_rsa_keypair(2048)
# c = code(ord('a'), pub_priv_keys[0])
# print(c)
# print(code(c, pub_priv_keys[1]))
