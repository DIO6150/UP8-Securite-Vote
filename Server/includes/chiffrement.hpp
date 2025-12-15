#ifndef RSA_SERVER_HPP
#define RSA_SERVER_HPP

#include <gmpxx.h>
#include <string>
#include <openssl/sha.h>

namespace rsa {

struct PublicKey {
    mpz_class n;
    mpz_class e;
    PublicKey(const mpz_class& n_, const mpz_class& e_) : n(n_), e(e_) {}
};

// Hash du message qui retourne un mpz_class
inline mpz_class hash_sha256(const std::string& message) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(message.data()), message.size(), hash);
    mpz_class h;
    mpz_import(h.get_mpz_t(), SHA256_DIGEST_LENGTH, 1, 1, 0, 0, hash);
    return h;
}

// Vérifie la signature: sig^e mod n == hash(message)
inline bool verify(const std::string& message, const mpz_class& signature, const PublicKey& pk) {
    mpz_class m_check;
    mpz_powm(m_check.get_mpz_t(), signature.get_mpz_t(), pk.e.get_mpz_t(), pk.n.get_mpz_t());
    mpz_class h = hash_sha256(message);
    return m_check == h;
}

}

#endif