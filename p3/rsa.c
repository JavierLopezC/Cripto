/*
    Contiene todas las funciones importantes para realizar todos los apartados
    del segundo ejercicio de la práctica 3: todo lo relacionado con RSA.

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "rsa.h"

gmp_randstate_t rstate;

static const int map[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/*
    Multiplica y toma módulo
*/
static inline void mpz_mul_mod(mpz_t r, mpz_t a, mpz_t b, mpz_t m)
{
    mpz_mul(r, a, b);
    mpz_mod(r, r, m);
}

/*
    Recibe un digito o una letra minuscula y devuelve el valor numérico
*/
int char_to_int(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'z')
        return c - 'a' + 10;
    else
        return -1;
}

/*
    Inicializa rstate con Mersenne twister y lee de /dev/urandom para hallar
    la seed
*/
void init_rstate()
{
    gmp_randinit_mt(rstate);
    FILE *f = fopen("/dev/urandom", "r");
    unsigned long int rseed;
    fread(&rseed, sizeof(rseed), 1, f);
    gmp_randseed_ui(rstate, rseed);
    fclose(f);
}

/*
    Genera un número aleatorio con un determinado número de bits
*/
void gmp_randb(mpz_t r, int bits)
{
    mpz_urandomb(r, rstate, bits);
}

/*
    Genera un número aleatorio entre 0 y n, ambos incluidos
*/
void gmp_randn(mpz_t r, mpz_t n)
{
    mpz_urandomm(r, rstate, n);
}

/*
    Libera el rstate creado antes
*/
void clear_rstate()
{
    gmp_randclear(rstate);
}

/*
    Implementa exponenciación binaria de izquierda a derecha
*/
void lr_bin_exp(mpz_t r, mpz_t g, mpz_t e, mpz_t m)
{
    char *repr = mpz_get_str(NULL, 2, e);
    mpz_set_ui(r, 1);
    for (int i = 0; i < strlen(repr); i++)
    {
        mpz_mul_mod(r, r, r, m);
        if (repr[i] == '1')
            mpz_mul_mod(r, r, g, m);
    }
    free(repr);
}

/*
    Implementa exponenciación k-aria de izquierda a derecha.

    La función de gmp mpz_get_str solo acepta valores para la base entre 2 y 62,
    por tanto k = 2, 3, 4, 5, pues 2^6 > 62. Es posible que esto pueda hacer
*/
void lr_kary_exp(mpz_t r, mpz_t g, mpz_t e, mpz_t m, int k)
{
    int base = (int)pow(2, k);
    char *repr = mpz_get_str(NULL, base, e);
    /* **** precomputatión **** */
    mpz_t *gs = (mpz_t *)malloc(sizeof(mpz_t) * base);
    for (int i = 0; i < base; i++)
        mpz_init(gs[i]);
    mpz_set_ui  ( gs[0], 1 );
    for (int i = 1; i < base; i++)
        mpz_mul_mod( gs[i], gs[i-1], g, m );
    /* **** fin precomputación **** */
    mpz_set_ui(r, 1);
    for (int i = 0; i < strlen(repr); i++)
    {
        for (int j = 0; j < k; j++)
            mpz_mul_mod(r, r, r, m);
        mpz_mul_mod(r, r, gs[map[repr[i]]], m);
    }
    free(repr);
    for (int i = 0; i < base; i++)
        mpz_clear(gs[i]);
    free(gs);
}

/*
    Dado un entero n, devuelve s y r tales que n = 2^s * r
*/
void even_odd(mpz_t n, int *s, mpz_t r)
{
    *s = 0;
    mpz_set(r, n);
    while (mpz_divisible_ui_p(r, 2))
    {
        mpz_divexact_ui(r, r, 2);
        (*s)++;
    }
}

/*
    Aplica Miller-rabin a un n y a concretos. Devuelve 1 si pasa el test; 0 en
    caso contrario. 1 := pseudoprimo, 0 := compuesto.
*/
int mr_test_a(mpz_t n, mpz_t a)
{
    mpz_t r, nsub, rmax, y;
    int s;
    mpz_inits(r, nsub, rmax, y, NULL);
    mpz_sub_ui(nsub, n, 1);
    even_odd(nsub, &s, r);

    mpz_powm(y, a, r, n);
    if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, nsub) != 0)
    {
        int j = 1;
        while (j <= s - 1 && mpz_cmp(y, nsub) != 0)
        {
            mpz_powm_ui(y, y, 2, n);
            if (mpz_cmp_ui(y, 1) == 0)
                return 0;
            j++;
        }
        if (mpz_cmp(y, nsub) != 0)
            return 0;
    }

    mpz_clears(r, nsub, rmax, y, NULL);
    return 1;
}

/*
    Aplica el test de Miller-Rabin a n con parámetro de seguridad t.
*/
int mr_test(mpz_t n, int t)
{
    mpz_t r, nsub, a, rmax, y;
    int s;
    mpz_inits(r, nsub, a, rmax, y, NULL);
    mpz_sub_ui(nsub, n, 1);
    even_odd(nsub, &s, r);
    mpz_sub_ui(rmax, n, 4);
    for (int i = 0; i < t; i++)
    {
        mpz_urandomm(a, rstate, rmax);
        mpz_add_ui(a, a, 2);
        mpz_powm(y, a, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, nsub) != 0)
        {
            int j = 1;
            while (j <= s - 1 && mpz_cmp(y, nsub) != 0)
            {
                mpz_mul_mod(y, y, y, n);
                if (mpz_cmp_ui(y, 1) == 0)
                {
                    mpz_clears(r, nsub, a, rmax, y, NULL);
                    return 0;
                }
                j++;
            }
            if (mpz_cmp(y, nsub) != 0)
            {
                mpz_clears(r, nsub, a, rmax, y, NULL);
                return 0;
            }
        }
    }
    mpz_clears(r, nsub, a, rmax, y, NULL);
    return 1;
}

int calc_reps(int b, double p)
{
    return (int)ceil( log((1-p)*b*log(2)/p) / log(4) );
}

/*
    Genera un pseudoprimo aleatorio con el número de bits indicado y la probabi-
    lidad de error indicada.
*/
void gen_prime_mr(mpz_t n, int b, double p)
{
    int t;
    t = calc_reps(b, p);
    do {
        mpz_urandomb(n, rstate, b);
    } while (mpz_cmp_ui(n, 3) < 0 || mpz_even_p(n) != 0 || mr_test(n, t) == 0);
}

/*
    Rompe RSA conociendo exponente de descifrado usando el A. las vegas.
*/
void break_rsa_miller(mpz_t n, mpz_t d, mpz_t e, mpz_t p, mpz_t q)
{
    mpz_t w, v, v0, rmax, foo, r;
    int s;
    mpz_inits(w, v, v0, rmax, foo, r, NULL);
    newtrial:
    /* d*e - 1 = 2^s * r */
    mpz_mul(foo, d, e);
    mpz_sub_ui(foo, foo, 1);
    even_odd(foo, &s, r);
    /* 1 <= w aleatorio <= n-1 */
    mpz_sub_ui(rmax, n, 2);
    mpz_urandomm(w, rstate, rmax);
    mpz_add_ui(w, w, 1);
    mpz_gcd(foo, w, n);
    /* si gcd(w,n) != 1 ya hemos factorizado w */
    if (mpz_cmp_ui(foo, 1) != 0)
    {
        mpz_set(p, foo);
        mpz_divexact(q, n, p);
        mpz_clears(w, v, v0, rmax, foo, r, NULL);
        return;
    }
    /* v = w^r mod n */
    mpz_powm(v, w, r, n);
    /* si v = 1 nuevo intento */
    if (mpz_cmp_ui(v, 1) == 0)
        goto newtrial;
    do
    {
        mpz_set(v0, v);
        mpz_powm_ui(v, v, 2, n);
    }
    while(mpz_cmp_ui(v, 1) != 0);
    /* mientras v != 1 ... */
    mpz_sub_ui(foo, n, 1);
    /* si v0 = n-1, nuevo intento */
    if (mpz_cmp(v0, foo) == 0)
        goto newtrial;
    else
    {
        /* p <--- gcd(v0+1, n) */
        mpz_add_ui(foo, v0, 1);
        mpz_gcd(p, foo, n);
        mpz_divexact(q, n, p);
    }
    mpz_clears(w, v, v0, rmax, foo, r, NULL);
}
