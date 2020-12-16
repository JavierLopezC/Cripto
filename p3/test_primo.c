/*
    Calculado los datos para hacer una gráfica

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
    Comprueba si los pseudoprimos generados por M-R siguiente la conjetura de
    Gauss de distribución de números primos
*/
int main(int argc, char const *argv[]) {
    int n_max = 10000;
    int ts[5] = {1, 5, 10, 20, 40};
    int p[5][n_max];
    mpz_t n;
    mpz_init(n);
    init_rstate();
    for (int i = 0; i < 5; i++)
    {
        mpz_set_ui(n, 5);
        for (int j = 0; j < n_max; j++)
        {
            p[i][j] = mr_test(n, ts[i]);
            mpz_add_ui(n, n, 1);
        }
    }
    mpz_clear(n);
    clear_rstate();
    int n_primos[5][100];
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            n_primos[i][j] = 0;
            for (int k = 0; k < 100; k++)
            {
                if (p[i][j*100 + k] == 1)
                    n_primos[i][j] += 1;
            }
        }
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 1; j < 100; j++)
            n_primos[i][j] += n_primos[i][j-1];
    }

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%d\t", n_primos[j][i]);
        }
        printf("\n");
    }
    return 0;
}
