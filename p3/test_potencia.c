/*
    Compara la eficiencia de las funciones de exponenciación implementadas por
    nosotros y la que ofrece gmp.

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
    test_potencia [numero_ejecuciones]

    Si no se introduce numero de ejecuciones se usa 1000
*/

int main(int argc, char const *argv[]) {
    long n = 1000;
    clock_t start, end;
    if (argc == 2)
        sscanf(argv[1], "%ld", &n);

    double s_gmp = 0;
    double s_lr_b = 0;
    double s_lr_k[5] = {0};

    mpz_t r[7], g, e, m;


    init_rstate();
    mpz_inits(g, e, m, NULL);
    for (int i = 0; i < 7; i++)
        mpz_init(r[i]);

    for (long i = 0; i < n; i++)
    {
        gmp_randb(g, 4096);
        gmp_randb(e, 4096);
        gmp_randb(m, 4096);
        start = clock();
        mpz_powm(r[0], g, e, m);
        end = clock();
        s_gmp += (double)(end - start)/CLOCKS_PER_SEC;
        start = clock();
        lr_bin_exp(r[1], g, e, m);
        end = clock();
        s_lr_b += (double)(end - start)/CLOCKS_PER_SEC;
        for (int j = 0; j < 5; j++)
        {
            start = clock();
            lr_kary_exp(r[2+j], g, e, m, j+1);
            end = clock();
            s_lr_k[j] += (double)(end - start)/CLOCKS_PER_SEC;
        }
        for (int j = 0; j < 6; j++)
        {
            if (mpz_cmp(r[j], r[j+1]) != 0)
            {
                printf("Error: las funciones no dan el mismo resultado\n");
                gmp_printf("r[%d] = %Zd\n", j, r[j]);
                gmp_printf("r[%d] = %Zd\n", j+1, r[j+1]);
                return 2;
            }
        }
    }

    clear_rstate();
    mpz_clears(g, e, m, NULL);
    for (int i = 0; i < 7; i++)
        mpz_init(r[i]);

    printf("-------- Tiempo en realizar %ld potencias\n", n);
    printf("Tiempo función de gmp:\t\t\t%lf\n", s_gmp);
    printf("Tiempo LR binaria:\t\t\t%lf\n", s_lr_b);
    for (int i = 0; i < 5; i++)
        printf("Tiempo LR %d-aria:\t\t\t%lf\n", i+1, s_lr_k[i]);
    printf("--------\n");
    printf("Los resultados de las operaciones coinciden para todas funciones\n");

    return 0;
}
