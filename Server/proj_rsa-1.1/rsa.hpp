#include <gmp.h>
#include <gmpxx.h>

typedef mpz_class pair_t[2];

class rsa {
public:
  static inline bool isPrime(mpz_srcptr n) {
    return mpz_probab_prime_p(n, 10) != 0;
  }  
  
  /* fait comme la fonction python crypto.getPrime */
  static inline mpz_class getPrime(int bits) {
    mpz_class n;
    char str[bits + 1];
    str[bits - 1] = '1'; // on commence par un impair
    str[bits]     = 0; // fin de chaîne
    do {
      for (int i = 0; i < bits - 1; i++) {
	int c =  (int)(2.0 * rand()  / (RAND_MAX + 1.0));
	str[i] = '0' + c;
      }
      mpz_init_set_str(n.get_mpz_t(), str, 2);
    } while (!isPrime(n.get_mpz_t()));
    return n;
  }

  /* pas à pas, la transcription de la fonction python gen_rsa_keypair */
  static inline pair_t * genKeyPairs(int bits) {
    mpz_class p = getPrime(bits / 2);
    mpz_class q = getPrime(bits / 2);
    mpz_class n = p * q;
    mpz_class phi_n = (p - 1) * (q - 1), e, d;
    unsigned long int ul_e;
    do {
      ul_e = 65537 + (rand() & 0xFFFF);
    } while (mpz_gcd_ui(NULL, phi_n.get_mpz_t(), ul_e) != 1);
    mpz_set_ui(e.get_mpz_t(), ul_e); // e
    mpz_invert(d.get_mpz_t(), e.get_mpz_t(), phi_n.get_mpz_t()); // d
    // ici j'ai calculé tout : (e, n) et (d, n)
    return new pair_t[2]{{e, n}, {d, n}};
  }

  /* comme la fonction python code */
  static inline mpz_class code(mpz_class a, pair_t pair) {
    mpz_class res;
    mpz_powm(res.get_mpz_t(), a.get_mpz_t(), pair[0].get_mpz_t(),
             pair[1].get_mpz_t());
    return res;
  }
};
