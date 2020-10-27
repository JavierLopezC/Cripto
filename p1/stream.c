#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <assert.h>

#define MODE_   0
#define K_      1
#define IN_     2
#define OUT_    3

#define NARGS   4

#define ENC     1
#define DEC     2

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

int args[6] = {OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
long key;
FILE *in, *out;

/* stream {-C|-D} {-k clave} [-i file_in] [-o file_out] */

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
        else if (strcmp(argv[i], "-k") == 0)
        {
            args[K_] = ++i;
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
    if (strcmp(argv[args[MODE_]], "-C") == 0)
        mode = ENC;
    else // if (strcmp(argv[i], "-D") == 0)
        mode = DEC;
    sscanf(argv[args[K_]], "%ld", &key);
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
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

int print_args(char *argv[]) {
    printf("Modo: %d\n", mode);
    printf("Clave: %ld\n", key);
    if (args[IN_] == OP)
        printf("stdin\n");
    else
        printf("%s\n", argv[args[IN_]]);
    if (args[OUT_] == OP)
        printf("stdout\n");
    else
        printf("%s\n", argv[args[OUT_]]);
}

int rand_()
{
    return (rand() % ALPH_SIZE);
}

int encode_char(int c, int r)
{
    return (c + r - 'A') % ALPH_SIZE + 'A';
}

int stream_encode()
{
    int c, i = 0;
    srand(key);
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
        fputc(encode_char(c, rand_()), out);
}

int decode_char(int c, int r)
{
    return (c + ALPH_SIZE - r - 'A') % ALPH_SIZE + 'A';
}

int stream_decode()
{
    int c, i = 0;
    srand(key);
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
        fputc(decode_char(c, rand_()), out);
}

int is_valid_str(char *str, int str_len)
{
    for (int i = 0; i < str_len; i++)
    {
        if (!IS_IN_ALPH(str[i]))
            return ERR;
    }
    return OK;
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
        return ERR;
    load_args(argv);
    print_args(argv);

    if (mode == ENC)
        stream_encode();
    else /* if (mode == DEC) */
        stream_decode();

    clean();

    return OK;
}
