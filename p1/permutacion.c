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

#define MAX(X,Y)        (((X) > (Y))? (X) : (Y))

int args[6] = {OB,OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
int k1[M];
int k2[N];
int block[M][N];
int block_[M][N];

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
        sscanf(argv[args[K1_] + i], "%d", &k1[i]);
    for (i = 0; i < N; i++)
        sscanf(argv[args[K2_] + i], "%d", &k2[i]);
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
    int i;
    printf("Modo: %d\n", mode);
    for (i = 0; i < M; i++)
        printf("%d ", k1[i]);
    printf("\n");
    for (i = 0; i < N; i++)
        printf("%d ", k2[i]);
    printf("\n");
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

void rand_fill(int i, int j)
{
    while (i*N + j < N*M)
    {
        block[i][j] = rand_() + 'A';
        if ((j = (++j % N)) == 0)
            i++;
    }
}

int fpeek(FILE *fp)
{
  int c = getc(fp);
  return c == EOF ? EOF : ungetc(c, fp);
}

int get_block()
{
    int i, j, c;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
        {
            if ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
                block[i][j] = c;
            else
            {
                rand_fill(i, j);
                return EOF;
            }
        }
    if (fpeek(in) == EOF)
        return EOF;
    return !EOF;
}

void put_block()
{
    int i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            fputc(block[i][j], out);
}

void swap_blocks()
{
    int i, j, a;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
        {
            a = block[i][j];
            block[i][j] = block_[i][j];
            block_[i][j] = a;
        }
}

void perm_rows()
{
    int i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            block_[ k1[i] ][ j ] = block[i][j];
}

void perm_cols()
{
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
            block[ j ][ k2[i] ] = block_[j][i];
}

void invert_perms()
{
    int i, foo[MAX(M,N)];
    for (i = 0; i < M; i++)
        foo[ k1[i] ] = i;
    for (i = 0; i < N; i++)
        foo[ k2[i] ] = i;
}


void print_block()
{
    int i, j;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
            printf("%c ", block[i][j]);
        printf("\n");
    }
}

int encode_block()
{
    perm_rows();
    perm_cols();
}

int permutacion_encode()
{
    int ret;
    int i = 0;
    do
    {
        ret = get_block();
        encode_block();
        put_block();
    }
    while (ret != EOF);
}

int decode_block()
{
    invert_perms();
    swap_blocks();
    perm_cols();
    perm_rows();
    swap_blocks();
}

int permutacion_decode()
{
    int ret;
    do
    {
        ret = get_block();
        decode_block();
        put_block();
    }
    while (ret != EOF);
}

int are_valid_perms()
{
    int k1_[M] = {0};
    int k2_[N] = {0};
    int i;
    for (i = 0; i < M; i++)
    {
        if (k1[i] >= 0 && k1[i] < M && k1_[ k1[i] ] == 0)
            k1_[ k1[i] ]++;
        else
            return ERR;
    }
    for (i = 0; i < N; i++)
    {
        if (k2[i] >= 0 && k2[i] < N && k2_[ k2[i] ] == 0)
            k2_[ k2[i] ]++;
        else
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

    if (are_valid_perms() == ERR)
    {
        printf("Error: permutaciones inválidas\n");
        return ERR;
    }

    if (mode == ENC)
        permutacion_encode();
    else /* if (mode == DEC) */
        permutacion_decode();

    clean();

    return OK;
}
