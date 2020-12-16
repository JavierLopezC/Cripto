/*
    Consigue factorizar el módulo de RSA sabiendo el exponente de descifrado.
    Para ello se utiliza el algoritmo de las vegas visto en clase, que en otras
    fuentes llaman algoritmo de Miller.

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

#define BUF_SIZE 8096

/*
    Carga la clave almacenada en fkey con nuestro formato
*/
void get_key(FILE *fkey, mpz_t n, mpz_t e, mpz_t d)
{
    char buf[BUF_SIZE];
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    mpz_set_str(n, buf, 10);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    mpz_set_str(e, buf, 10);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    mpz_set_str(d, buf, 10);
    fgets(buf, BUF_SIZE, fkey);
}

/*
    break_rsa -n modulo -e exp_cifrado -d exp_descifrado

    o bien

    break_rsa -f fichero_claves

    donde el fichero de claves es uno generado por nuestro programa RSA
*/

int main(int argc, char const *argv[]) {
    mpz_t n, d, e, p, q;
    mpz_inits(n, d, e, p, q, NULL);

    if (argc != 7 && argc != 3)
    {
        printf("Error: número de argumentos incorrectos\n");
        return 2;
    }

    if (argc == 7)
    {
        mpz_set_str(n, argv[2], 10);
        mpz_set_str(e, argv[4], 10);
        mpz_set_str(d, argv[6], 10);
    }
    else /* if (argc == 3) */
    {
        FILE *fkey = fopen(argv[2], "r");
        get_key(fkey, n, e, d);
        fclose(fkey);
    }

    init_rstate();
    break_rsa_miller(n, d, e, p, q);
    clear_rstate();
    printf("---- PRIMO 1 ----\n");
    gmp_printf("%Zd\n", p);
    printf("---- FIN PRIMO 1 ----\n");
    printf("---- PRIMO 2 ----\n");
    gmp_printf("%Zd\n", q);
    printf("---- FIN PRIMO 2 ----\n");


    return 0;
}
