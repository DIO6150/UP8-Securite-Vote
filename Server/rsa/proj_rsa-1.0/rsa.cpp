#include "gmp.h"
#include <ctime>
#include <gmpxx.h>

bool isPrime(mpz_srcptr n) {
  return mpz_probab_prime_p(n, 10) != 0;
}  

/*
  fait comme la fonction python crypto.getPrime
*/
mpz_class getPrime(int bits) {
  mpz_class n;
  //mpz_srcptr  n = new mpz_t;
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

/*

pas à pas, la transcription de la fonction gen_rsa_keypair

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
*/
void genKeyPairs(int bits) {
  mpz_class p = getPrime(bits / 2);
  mpz_class q = getPrime(bits / 2);
  mpz_class n = p * q;
  mpz_class phi_n = (p - 1) * (q - 1), e, d;
  unsigned long int ul_e;
  do {
    ul_e = 65537 + (rand() & 0xFFFF);
  } while (mpz_gcd_ui(NULL, phi_n.get_mpz_t(), ul_e) != 1);
  mpz_set_ui(e.get_mpz_t(), ul_e);
  mpz_invert(d.get_mpz_t(), e.get_mpz_t(), phi_n.get_mpz_t());
  // ici j'ai calculé tout : (e, n) et (d, n)

  // test
  mpz_class msg("1312", 10);
  mpz_class emsg, dmsg;
  //encoder msg dans emsg
  mpz_powm(emsg.get_mpz_t(), msg.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
  //décoder emsg dans dmsg
  mpz_powm(dmsg.get_mpz_t(), emsg.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());

  printf("msg = ");
  mpz_out_str(stdout, 10, msg.get_mpz_t());
  printf("\nemsg = ");
  mpz_out_str(stdout, 10, emsg.get_mpz_t());
  printf("\ndmsg = ");
  mpz_out_str(stdout, 10, dmsg.get_mpz_t());
  printf("\n");

  if (msg == dmsg) {
    printf("Youpi !\n");
  } else {
    printf("Merde !\n");
  }
}  

int main(void) {
  srand(time(NULL));
  genKeyPairs(2048);
  return 0;  
}  
