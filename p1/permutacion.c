#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <assert.h>

#define MODE_   0
#define K1_     1
#define K2_     2
#define IN_     3
#define OUT_    4

#define NARGS   5

#define ENC     1
#define DEC     2

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

#define M       4       /* M = número de filas */
#define N       7       /* N = número de columnas */

int args[6] = {OB,OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
int k1[M];
int k2[N];
FILE *in, *out;

/* permutacion {-C|-D} {-k1 clave} {-k2 clave} [-i file_in] [-o file_out] */

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
        if (strcmp(argv[i], "-C") == 0)
        {
            args[MODE_] = i;
        }
        else if (strcmp(argv[i], "-D") == 0)
        {
            args[MODE_] = i;
        }
        else if (strcmp(argv[i], "-k1") == 0)
        {
            args[K1_] = ++i;
            i += M - 1;
        }
        else if (strcmp(argv[i], "-k2") == 0)
        {
            args[K2_] = ++i;
            i += N - 1;
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

        if (argc <= i)
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
    int i;
    /* Argumentos obligatorios */
    if (strcmp(argv[args[MODE_]], "-C") == 0)
        mode = ENC;
    else // if (strcmp(argv[i], "-D") == 0)
        mode = DEC;
    for (i = 0; i < M; i++)
        sscanf(argv[args[K1_] + i], "%ld", &k1[i]);
    for (i = 0; i < N; i++)
        sscanf(argv[args[K2_] + i], "%ld", &k2[i]);
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

void clean()
{
    fclose(in);
    fclose(out);
}

int print_args(char *argv[]) {
    printf("Modo: %d\n", mode);
    printf("Clave: %s\n", key);
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

/*
    El programa cifra/descifra el contenido de los ficheros que toquen char a
    char hasta encontrar un caracter fuera del alfabeto o EOF (que tmbn está
    fuera del alfabeto but ok)

    todo: apañar un poco todo, cerrar ficheros liberar memoria...
*/

int main (int argc, char *argv[])
{
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    if (is_valid_str(key, key_len) == ERR)
    {
        printf("Error: la clave contiene chars fuera del alfabeto\n");
        return ERR;
    }

    if (mode == ENC)
        vigenere_encode();
    else /* if (mode == DEC) */
        vigenere_decode();

    clean();

    return OK;
}
