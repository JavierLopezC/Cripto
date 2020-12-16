#ifndef RSA_H
#define RSA_H

#include <gmp.h>

void lr_bin_exp(mpz_t r, mpz_t g, mpz_t e, mpz_t m);
void lr_kary_exp(mpz_t r, mpz_t g, mpz_t e, mpz_t m, int k);

void init_rstate();
void gmp_randb(mpz_t r, int bits);
void gmp_randn(mpz_t r, mpz_t n);
void clear_rstate();

int calc_reps(int b, double p);

void gen_prime_mr(mpz_t n, int b, double p);

int mr_test(mpz_t n, int t);

void break_rsa_miller(mpz_t n, mpz_t d, mpz_t e, mpz_t p, mpz_t q);


#endif
