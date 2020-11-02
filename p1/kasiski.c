/*
    Contiene el código para aplicar el test de Kasiski

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

#define MAX_DISTS       100

int args[6] = {OB,OP,OP};     /* OB es para args. obligatorios */

int n;
FILE *in, *out;

char *whole_file;
char *n_gram;


/* kasiski {-l longitud n-grama a probar} [-i file_in] [-o file_out] */

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
    n = atoi(argv[args[L_]]);
    /* Argumentos opcionales */
    if (args[IN_] == OP)
    {
        in = stdin;
    }
    else
    {
        in = fopen(argv[args[IN_]], "rb");
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
    printf("Tamaño de n-grama: %d\n", n);
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
    Devuelve un número aleatorio entre 0 y 26
*/
int rand_()
{
    return (rand() % ALPH_SIZE);
}

/*
    Devuelve un n-grama aleatorio
*/
void random_n_gram()
{
    int i;

    n_gram = (char *)malloc(sizeof(char) * n);
    for (i = 0; i < n; i++)
        n_gram[i] = rand_() + 'A';
}

void destroy_ngram()
{
    free(n_gram);
}

/*
    Lee el fichero de entrada entero y lo guarda en un string
*/
char *read_whole_file()
{
    fseek(in, 0, SEEK_END);
    long fsize = ftell(in);
    fseek(in, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, in);

    string[fsize] = 0;

    return string;
}

char *first(char *ngram)
{
    char *ret;
    ret = strstr(whole_file, ngram);
}

int main (int argc, char *argv[])
{
    int i;
    int distances[MAX_DISTS] = {0};
    char *ret;
    /* Parsea, carga e imprime los argumentos */
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    whole_file = read_whole_file();
    // printf("%s\n", whole_file);

    i = 0;
    while (i < MAX_DISTS)
    {
        random_n_gram();
    }

    /* imprime las distancias */
    for (i = 0; i < MAX_DISTS; i++)
        fprintf(out, "%d\n", distances[i]);




    return OK;
}
