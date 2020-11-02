/*
    Código para el ejercicio de Seguridad Perfecta

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define M_      0
#define IN_     1
#define OUT_    2

#define NARGS   3

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

#define EP      0
#define NEP     1

int args[6] = {OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
FILE *in, *out;

int freqs[ALPH_SIZE] = {0};
int freqs_enc[ALPH_SIZE] = {0};
int total = 0;

/* seg-perf {-P | -I} [-i file_in] [-o file_out] */

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
        else if (strcmp(argv[i], "-P") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-I") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-i") == 0)
            args[IN_] = ++i;
        else if (strcmp(argv[i], "-o") == 0)
            args[OUT_] = ++i;
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
            i++;
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
    if (strcmp(argv[args[M_]], "-P") == 0)
        mode = EP;
    else /* if (strcmp(argv[args[M_]], "-I") == 0) */
        mode = NEP;
    /* Argumentos opcionales */
    if (args[IN_] == OP)
        in = stdin;
    else
        in = fopen(argv[args[IN_]], "r");
    if (args[OUT_] == OP)
        out = stdout;
    else
        out = fopen(argv[args[OUT_]], "w");
    return OK;
}

int print_args(char *argv[]) {
    printf("Modo EP/NEP: %d\n", mode);
    printf("Entrada: ");
    if (args[IN_] == OP)
        printf("stdin\n");
    else
        printf("%s\n", argv[args[IN_]]);
    printf("Salida: ");
    if (args[OUT_] == OP)
        printf("stdout\n");
    else
        printf("%s\n", argv[args[OUT_]]);
}

void clean()
{
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

/*
    PARTE 1: Funciones para hacer el ejercicio
*/

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

    clean();

    return OK;
}
