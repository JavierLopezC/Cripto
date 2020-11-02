/*
    Contiene el código para calcular el IC de un texto cifrado con vigenere
    y una hipótesis de longitud de clave

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define L_      0
#define IN_     1
#define OUT_    2

#define NARGS   3

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

#define IC_SPA      0.078
#define IC_ENG      0.065
#define IC_RAN      0.038
#define THOLD       0.005

int args[6] = {OB,OP,OP};     /* OB es para args. obligatorios */

int key_len;
FILE *in, *out;

int **freqs = NULL;
double *totals = NULL;
double *ics = NULL;

/* ic {-l longitud n-grama a probar} [-i file_in] [-o file_out] */

/*
    PARTE 0: Parseo de argumentos. Dado que no forma estrictamente parte de la
    práctica no se comenta sistemáticamente
*/

int parse_args(int argc, char *argv[])
{
    int i;
    i = 1;
    while (i < argc)
    {
        if (argv[i][0] != '-')
        {
            printf("Error: sintaxis\n");
            return ERR;
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            args[L_] = ++i;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            args[IN_] = ++i;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            args[OUT_] = ++i;
        }
        else
        {
            printf("Error: argumento no identificado\n");
            return ERR;
        }

        if (argc == i)
        {
            printf("Error: sintaxis\n");
            return ERR;
        }
        else
        {
            i++;
        }
    }
    for (i = 0; i < NARGS; i++)
    {
        if (args[i] == OB)
        {
            printf("Error: falta argumento obligatorio\n");
            return ERR;
        }
    }
    return OK;
}

int load_args(char *argv[])
{
    /* Argumentos obligatorios */
    key_len = atoi(argv[args[L_]]);
    /* Argumentos opcionales */
    if (args[IN_] == OP)
    {
        in = stdin;
    }
    else
    {
        in = fopen(argv[args[IN_]], "r");
    }
    if (args[OUT_] == OP)
    {
        out = stdout;
    }
    else
    {
        out = fopen(argv[args[OUT_]], "w");
    }
    return OK;
}

int print_args(char *argv[]) {
    printf("Tamaño clave a probar: %d\n", key_len);
    if (args[IN_] == OP)
    {
        printf("stdin\n");
    }
    else
    {
        printf("%s\n", argv[args[IN_]]);
    }
    if (args[OUT_] == OP)
    {
        printf("stdout\n");
    }
    else
    {
        printf("%s\n", argv[args[OUT_]]);
    }
}

void clean()
{
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

/*
    PARTE 1: Funciones para el criptoanálisis
*/

/*
    Lee el fichero de entrada y almacena las frecuencias de cada caracter en
    cada subtexto
*/
void calc_freqs()
{
    int i, c;
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
    {
        freqs[ i % key_len ][ c - 'A' ]++;
        totals[ i % key_len ]++;
        i++;
    }
}

/*
    Con los datos de las frecuencias calcula el IC para cada subtexto
*/
void calc_ics()
{
    int i, j;
    for (i = 0; i < key_len; i++)
    {
        ics[i] = 0;
        for (j = 0; j < ALPH_SIZE; j++)
        {
            ics[i] += pow(freqs[i][j], 2);
        }
        ics[i] /= pow(totals[i], 2);
    }
}

/*
    Obtiene el IC medio de todos los subtextos
*/
double get_avg_ic()
{
    int i;
    double tmp = 0;
    for (i = 0; i < key_len; i++)
        tmp += ics[i];
    return tmp / key_len;
}

/*
    Imprime los resultados del criptoanálisis
*/
void print_ics()
{
    int i;
    double avg;
    fprintf(out, "-------- Resultados\n");
    for (i = 0; i < key_len; i++)
    {
        fprintf(out, "Subtexto %d\t\t IC = %lf\n", i, ics[i]);
    }
    avg = get_avg_ic();
    fprintf(out, "-------- IC medio = %lf\n", avg);
    fprintf(out, "Diferencia con IC inglés:\t%lf\n", fabs(avg - IC_ENG));
    fprintf(out, "Diferencia con IC español:\t%lf\n", fabs(avg - IC_SPA));
    fprintf(out, "Diferencia con IC aleatorio:\t%lf\n", fabs(avg - IC_ENG));
    fprintf(out, "-------- Conclusiones\n");
    if (fabs(avg - IC_ENG) < THOLD) fprintf(out, "¡Ojo! Dif. con IC inglés < %lf\n", THOLD);
    if (fabs(avg - IC_SPA) < THOLD) fprintf(out, "¡Ojo! Dif. con IC español < %lf\n", THOLD);
}

/*
    Reserva toda la memoria necesaria
*/
void alloc_memory()
{
    int i;
    freqs = (int **)malloc(sizeof(int *) * key_len);
    totals = (double *)calloc(key_len, sizeof(double));
    ics = (double *)malloc(sizeof(double) * key_len);
    for (i = 0; i < key_len; i++)
        freqs[i] = (int *)calloc(ALPH_SIZE, sizeof(int));
}

/*
    Libera toda la memoria necesaria
*/
void dealloc_memory()
{
    int i;
    for (i = 0; i < key_len; i++)
        free(freqs[i]);
    free(ics);
    free(totals);
    free(freqs);
}

int main (int argc, char *argv[])
{
    int i;
    /* Parsea, carga e imprime los argumentos */
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    /*
        Reserva memoria, calcula las frecuencias, calcula los ICs, los imprime
        en el formato deseado y libera la memoria
    */
    alloc_memory();
    calc_freqs();
    calc_ics();
    print_ics();
    dealloc_memory();

    clean();

    return OK;
}
