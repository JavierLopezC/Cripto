#ifndef EUCLID_H
#define EUCLID_H
#include <gmp.h>

struct ext_ret
{
    mpz_t bez_a;
    mpz_t bez_b;
    mpz_t gcd;
};

void euclid(mpz_t gcd, mpz_t a, mpz_t b);
void ext_euclid(struct ext_ret *ret, mpz_t a, mpz_t b);

#endif
