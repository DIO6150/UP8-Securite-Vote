#include "rsa.hpp"
#include <iostream>

static void test(pair_t pub, pair_t priv) {
  // données du test
  mpz_class msg("1312", 10);
  mpz_class emsg, dmsg;
  // encoder msg dans emsg
  emsg = rsa::code(msg, pub);
  // décoder emsg dans dmsg
  dmsg = rsa::code(emsg, priv);
  
  if (msg == dmsg)
    fprintf(stderr, "Youpi !\n");
  else
    fprintf(stderr, "Merde !\n");
}  

int main(void) {
  srand(time(NULL));
  pair_t *pairs = rsa::genKeyPairs(2048);
  test(pairs[0], pairs[1]);
  return 0;  
}  
