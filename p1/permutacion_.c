/*
    Este fichero contiene el código necesario para el criptosistema de permuta-
    cion por bloques.

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permutacion.h"

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

#define MAX(X,Y)        (((X) > (Y))? (X) : (Y))

int args[6] = {OB,OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
int k1[M];
int k2[N];
int block[M][N];
int block_[M][N];

FILE *in, *out;

/* permutacion {-C|-D} {-k1 clave} {-k2 clave} [-i file_in] [-o file_out] */

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

/*
    PARTE 1: Funciones para el cifrado
*/

/*
    Devuelve un número aleatorio entre 0 y 25
*/
int rand_()
{
    return (rand() % ALPH_SIZE);
}

/*
    Rellena el bloque con números aleatorios a partir de la fila i y columna j-
    Generalmente usado para rellenar el último bloque a cifrar.
*/
void rand_fill(int i, int j)
{
    while (i*N + j < N*M)
    {
        block[i][j] = rand_() + 'A';
        if ((j = (++j % N)) == 0)
            i++;
    }
}

/*
    Permite mirar el siguiente char de un fichero
*/
int fpeek(FILE *fp)
{
  int c = getc(fp);
  return c == EOF ? EOF : ungetc(c, fp);
}

/*
    Lee y almacena un bloque de caracteres del fichero de entrada en la matriz
    block
*/
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
    if (fpeek(in) == EOF || fpeek(in) == '\n')
        return EOF;
    else
        return !EOF;
}

/*
    Escribe en el fichero de salida el bloque block
*/
void put_block()
{
    int i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            fputc(block[i][j], out);
}

/*
    Intercambia el contenido de los bloques block <-> block_
*/
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

/*
    Permuta las filas de block en función de la permutación de filas y almacena
    el resultado en block_
*/
void perm_rows()
{
    int i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            block_[ i ][ j ] = block[ k1[i] ][ j ];
}

/*
    Permuta las columnas de block_ en función de la permutación de columnas y
    almacena el resultado en block
*/
void perm_cols()
{
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
            block[ j ][ i ] = block_[ j ][ k2[i] ];
}

/*
    Invierte las permutaciones de filas y columnas
*/
void invert_perms()
{
    int i, foo[MAX(M,N)];
    for (i = 0; i < M; i++)
        foo[ k1[i] ] = i;
    for (i = 0; i < M; i++)
        k1[i] = foo[i];
    for (i = 0; i < N; i++)
        foo[ k2[i] ] = i;
    for (i = 0; i < N; i++)
        k2[i] = foo[i];
}

/*
    Imprime el bloque block
*/
void print_block()
{
    int i, j;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
            printf("%d ", block[i][j]);
        printf("\n");
    }
}

/*
    Codifica block, almacenando el bloque codificado en el propio block
*/
int encode_block()
{
    perm_rows();
    perm_cols();
}

/*
    Lee bloques, los cifra y los escribe
*/
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
    fputc('\n', out);
}

/*
    Decodifica block, almacenando el bloque en el propio block
*/
int decode_block()
{
    invert_perms();
    swap_blocks();
    perm_cols();
    perm_rows();
    swap_blocks();
    invert_perms();
}



/*
    Lee bloques, decodifica y escribe
*/
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
    fputc('\n', out);
}

/*
    Comprueba que las permutaicones son válidas
*/
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
    for (i = 0; i < M; i++)
    {
        if (k1_[i] == 0)
            return ERR;
    }
    for (i = 0; i < N; i++)
    {
        if (k2_[i] == 0)
            return ERR;
    }
    return OK;
}

/*
    PARTE EXTRA: Funciones para hacer el opcional
*/

/*
    Establece los valores de k1 y k2
*/
int set_keys(int k1_[M], int k2_[N])
{
    int i;
    for (i = 0; i < M; i++)
        k1[i] = k1_[i];
    for (i = 0; i < N; i++)
        k2[i] = k2_[i];
    if (are_valid_perms() == ERR)
        return ERR;
    else
        return OK;
}

/*
    Codifica in_ y la almacena en out_
*/
void encode(int **in_, int **out_)
{
    int i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            block[i][j] = in_[i][j];
    encode_block();
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            out_[i][j] = block[i][j];
}

/*
    Decodifica in_ y la almacena en out_. No llega a usarse nunca, pero ahí está
*/
void decode(int **in_, int **out_)
{
    int i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            block[i][j] = in_[i][j];
    decode_block();
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            out_[i][j] = block[i][j];
}

/*
    El programa cifra/descifra el contenido de los ficheros que toquen bloque a
    bloque hasta encontrar un caracter fuera del alfabeto o EOF (que tmbn está
    fuera del alfabeto)
*/

// int main (int argc, char *argv[])
// {
//     /* Parsea, carga e imprime los argumentos */
//     if (parse_args(argc, argv) == ERR)
//         return ERR;
//     load_args(argv);
//     print_args(argv);
//
//     /* Comprueba que las permutaciones son válidas */
//     if (are_valid_perms() == ERR)
//     {
//         printf("Error: permutaciones inválidas\n");
//         return ERR;
//     }
//
//     /* Codifica o decodifica dependiendo del modo */
//     if (mode == ENC)
//         permutacion_encode();
//     else /* if (mode == DEC) */
//         permutacion_decode();
//
//     /* Limpia la memoria utilizada */
//     clean();
//
//     return OK;
// }
