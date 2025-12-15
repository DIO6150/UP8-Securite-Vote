#ifndef PAILLIER_SERVER_HPP
#define PAILLIER_SERVER_HPP

#include <gmp.h>
#include <gmpxx.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/sha.h>

namespace paillier {

struct PublicKey {
    mpz_class n;
    mpz_class g;
    mpz_class n_squared;
    
    PublicKey() {}
    PublicKey(const mpz_class& n_, const mpz_class& g_) 
        : n(n_), g(g_) {
        n_squared = n * n;
    }
};

struct PrivateKey {
    mpz_class lambda;
    mpz_class mu;
    
    PrivateKey() {}
    PrivateKey(const mpz_class& lambda_, const mpz_class& mu_) 
        : lambda(lambda_), mu(mu_) {}
};

struct ZKProof {
    std::vector<mpz_class> a_values;
    std::vector<mpz_class> z_values;
    std::vector<mpz_class> e_values;
    mpz_class e;
    std::vector<int> valid_values;
};

std::pair<PublicKey, PrivateKey> generate_keypair(const mpz_class& p, const mpz_class& q);
mpz_class add(const mpz_class& c1, const mpz_class& c2, const PublicKey& pk);
mpz_class decrypt(const mpz_class& c, const PublicKey& pk, const PrivateKey& sk);
bool zkp_verify(const mpz_class& c, const ZKProof& proof, const PublicKey& pk, int t = 128);

namespace internal {

inline mpz_class L(const mpz_class& x, const mpz_class& n) {
    return (x - 1) / n;
}

inline mpz_class hash_to_mpz(const std::string& input, int t) {
    unsigned char hash_output[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input.c_str(), input.length(), hash_output);
    
    mpz_class result;
    mpz_import(result.get_mpz_t(), SHA256_DIGEST_LENGTH, 1, 1, 0, 0, hash_output);
    
    mpz_class modulus = mpz_class(1) << t;
    return result % modulus;
}

}

inline std::pair<PublicKey, PrivateKey> generate_keypair(
    const mpz_class& p, 
    const mpz_class& q
) {
    mpz_class n = p * q;
    mpz_class g = n + 1;
    
    mpz_class p_minus_1 = p - 1;
    mpz_class q_minus_1 = q - 1;
    mpz_class gcd_val;
    mpz_gcd(gcd_val.get_mpz_t(), p_minus_1.get_mpz_t(), q_minus_1.get_mpz_t());
    mpz_class lambda = (p_minus_1 * q_minus_1) / gcd_val;
    
    mpz_class n_squared = n * n;
    mpz_class g_lambda;
    mpz_powm(g_lambda.get_mpz_t(), g.get_mpz_t(), lambda.get_mpz_t(), n_squared.get_mpz_t());
    
    mpz_class l_result = internal::L(g_lambda, n);
    mpz_class mu;
    mpz_invert(mu.get_mpz_t(), l_result.get_mpz_t(), n.get_mpz_t());
    
    return {PublicKey(n, g), PrivateKey(lambda, mu)};
}

inline mpz_class add(const mpz_class& c1, const mpz_class& c2, const PublicKey& pk) {
    return (c1 * c2) % pk.n_squared;
}

inline mpz_class decrypt(const mpz_class& c, const PublicKey& pk, const PrivateKey& sk) {
    mpz_class c_lambda;
    mpz_powm(c_lambda.get_mpz_t(), c.get_mpz_t(), sk.lambda.get_mpz_t(), pk.n_squared.get_mpz_t());
    
    mpz_class m = (internal::L(c_lambda, pk.n) * sk.mu) % pk.n;
    return m;
}

inline bool zkp_verify(const mpz_class& c, const ZKProof& proof, const PublicKey& pk, int t) {
    int ell = proof.a_values.size();
    mpz_class modulus = mpz_class(1) << t;
    
    // Phase 1
    std::string hash_input;
    for (const auto& a : proof.a_values) {
        hash_input += a.get_str();
    }
    mpz_class e_computed = internal::hash_to_mpz(hash_input, t);
    
    if (e_computed != proof.e) {
        return false;
    }
    
    // Phase 2
    mpz_class sum_e = 0;
    for (const auto& e_i : proof.e_values) {
        sum_e = (sum_e + e_i) % modulus;
    }
    
    if (sum_e != proof.e) {
        return false;
    }
    
    // Phase 3
    for (int i = 0; i < ell; i++) {
        int m_i = proof.valid_values[i];
        
        // z_i^n mod n^2
        mpz_class z_n;
        mpz_powm(z_n.get_mpz_t(), proof.z_values[i].get_mpz_t(), 
                 pk.n.get_mpz_t(), pk.n_squared.get_mpz_t());
        
        // g^m_i mod n^2
        mpz_class g_m;
        mpz_powm(g_m.get_mpz_t(), pk.g.get_mpz_t(), 
                 mpz_class(m_i).get_mpz_t(), pk.n_squared.get_mpz_t());
        
        // g^(-m_i) mod n^2
        mpz_class g_minus_m;
        mpz_invert(g_minus_m.get_mpz_t(), g_m.get_mpz_t(), pk.n_squared.get_mpz_t());
        
        // (c * g^(-m_i)) mod n^2
        mpz_class term = (c * g_minus_m) % pk.n_squared;
        
        // (c * g^(-m_i))^e_i mod n^2
        mpz_class term_e;
        mpz_powm(term_e.get_mpz_t(), term.get_mpz_t(), 
                 proof.e_values[i].get_mpz_t(), pk.n_squared.get_mpz_t());
        
        // a_i * (c * g^(-m_i))^e_i mod n^2
        mpz_class right_side = (proof.a_values[i] * term_e) % pk.n_squared;
        
        if (z_n != right_side) {
            return false;
        }
    }
    
    return true;
}

}

#endif