/*
    Implementa un programa que dados una base, un exponente y un módulo realiza
    la exponenciación modular con nuestra función.

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

/*
    potencia base exponente modulo [-v]

    El programa realiza la operación potencia^base mod modulo. El argumento
    opcional -v activa el modo verboso. Se comprueba que el exponente sea un
    número no negativo y que el módulo sea positivo.
*/

int main(int argc, char const *argv[]) {
    mpz_t r, g, e, m;
    int v = 0;

    if (argc != 4 && argc != 5)
    {
        printf("Error: número de argumentos incorrectos\n");
        return 2;
    }

    mpz_inits(r, g, e, m, NULL);

    mpz_set_str(g, argv[1], 10);
    mpz_set_str(e, argv[2], 10);
    mpz_set_str(m, argv[3], 10);

    if (mpz_cmp_ui(e, 0) < 0 || mpz_cmp_ui(m, 0) <= 0)
    {
        printf("Error: exponente o módulo inválidos\n");
        mpz_clears(r, g, e, m, NULL);
        return 2;
    }

    if (argc == 5 && strcmp(argv[4], "-v") == 0)
        v = 1;

    lr_kary_exp(r, g, e, m, 5);

    if (v == 1)
    {
        printf("--------\n");
        gmp_printf("g := %Zd\n", g);
        gmp_printf("e := %Zd\n", e);
        gmp_printf("m := %Zd\n", m);
        gmp_printf("g^e mod m = %Zd\n", r);
        printf("--------\n");
    }
    else
        gmp_printf("%Zd\n", r);

    mpz_clears(r, g, e, m, NULL);

    return 0;
}
