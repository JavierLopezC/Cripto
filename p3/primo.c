/*
    Devuelve en formato decimal un número pseudoprimo con la probabilidad
    indicada de error

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
    primo -b numero_bits -t sec [-o file_out]

    el parámetro sec, seguridad, es un número que sirve para calcular la
    probabilidad de error con la siguiente fórmula:

    10^{-sec}
*/
int main(int argc, char const *argv[]) {
    int n_bits, expon;
    double sec;
    FILE *out = NULL;

    if (argc != 5 && argc != 7)
    {
        printf("Error: número de argumentos incorrectos\n");
        return 2;
    }

    sscanf(argv[2], "%d", &n_bits);
    sscanf(argv[4], "%d", &expon);
    sec = pow(10, -expon);

    if (argc == 7)
        out = fopen(argv[6], "w");
    else
        out = stdout;

    mpz_t n;
    mpz_init(n);
    init_rstate();
    clock_t start = clock();
    gen_prime_mr(n, n_bits, sec);
    clock_t end = clock();
    double t = (double)(end - start)/CLOCKS_PER_SEC;


    int reps = calc_reps(n_bits, sec);
    int res = mpz_probab_prime_p(n, reps);

    char *pp = mpz_get_str(NULL, 10, n);
    fprintf(out, "---- PSEUDOPRIMO ----\n%s\n---- FIN PSEUDOPRIMO ----\n", pp);
    fprintf(out, "-------- 0=Compuesto;1=PrimoProbable;2=Primo\n");
    fprintf(out, "Nuestro resultado:\t\t1\n");
    fprintf(out, "Resultado de gmp:\t\t%d\n", res);
    fprintf(out, "Número de repeticiones:\t\t%d\n", reps);
    fprintf(out, "Probabilidad error:\t\t< 10^-%d\n", expon);
    fprintf(out, "Tiempo (s) en generar:\t\t%lf\n", t);
    fprintf(out, "--------\n");
    free(pp);

    clear_rstate();
    mpz_clear(n);

    return 0;
}
