#include "euclid.h"

#define OLD_R_  mpzs[0]
#define OLD_S_  mpzs[1]
#define OLD_T_  mpzs[2]
#define R_      mpzs[3]
#define S_      mpzs[4]
#define T_      mpzs[5]
#define Q_      mpzs[6]
#define FOO_    mpzs[7]
#define BAR_    mpzs[8]

void ext_euclid(struct ext_ret *ret, mpz_t a, mpz_t b)
{
    mpz_t mpzs[9];
    for (int i = 0; i < 9; i++)
        mpz_init(mpzs[i]);
    mpz_init    (ret->bez_a);
    mpz_init    (ret->bez_b);
    mpz_init    (ret->gcd);
    mpz_set     (OLD_R_, a);
    mpz_set     (R_, b);
    mpz_set_ui  (OLD_S_, 1);
    mpz_set_ui  (S_, 0);
    mpz_set_ui  (OLD_T_, 0);
    mpz_set_ui  (T_, 1);
    while (mpz_cmp_ui(R_, 0) != 0)
    {
        mpz_fdiv_q  (Q_, OLD_R_, R_);

        mpz_mul     (FOO_, Q_, R_);
        mpz_sub     (BAR_, OLD_R_, FOO_);
        mpz_set     (OLD_R_, R_);
        mpz_set     (R_, BAR_);

        mpz_mul     (FOO_, Q_, S_);
        mpz_sub     (BAR_, OLD_S_, FOO_);
        mpz_set     (OLD_S_, S_);
        mpz_set     (S_, BAR_);

        mpz_mul     (FOO_, Q_, T_);
        mpz_sub     (BAR_, OLD_T_, FOO_);
        mpz_set     (OLD_T_, T_);
        mpz_set     (T_, BAR_);
    }
    mpz_set     (ret->bez_a, OLD_S_);
    mpz_set     (ret->bez_a, OLD_T_);
    mpz_set     (ret->gcd, OLD_R_);
    for (int i = 0; i < 9; i++)
        mpz_clear(mpzs[i]);
}

void euclid(mpz_t gcd, mpz_t a, mpz_t b)
{
    struct ext_ret ret;
    ext_euclid  (&ret, a, b);
    mpz_set     (gcd, ret.gcd);
    mpz_clear   (ret.bez_a);
    mpz_clear   (ret.bez_b);
    mpz_clear   (ret.gcd);
}
