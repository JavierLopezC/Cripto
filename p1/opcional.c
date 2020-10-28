/*
    Este fichero contiene el código necesario para criptoanalizar el sistema
    de permutación por bloques.

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "permutacion.h"
#include "opcional.h"

#define ERR     -1
#define OK      0


/*
    Las longitudes de M y N en base el tamaño del alfabeto
*/
int len1 = (int)ceil(log(M)/log(ALPH_SIZE));
int len2 = (int)ceil(log(N)/log(ALPH_SIZE));

/*
    Devuelve en repr la representación de n en base base
*/
void int_to_repr(int base, int n, int *repr)
{
    int i, foo;
    i = 0;
    foo = n;
    do
    {
        repr[i] = foo % base;
        foo = (foo - repr[i])/base;
        i++;
    }
    while (foo != 0);
}

/*
    Devuelve el número de longitud len representado en repr en base base
*/
int repr_to_int(int base, int *repr, int len)
{
    int i, n;
    n = 0;
    for (i = len-1; i >= 0; i--)
        n = base*n + repr[i];
    return n;
}

/*
    Establece toda la columna col de la matriz mat al valor val
*/
void set_col(int **mat, int col, int val)
{
    int i;
    for (i = 0; i < M; i++)
        mat[i][col] = val;
}

/*
    Establece toda la fila row de la matriz mat al valor val
*/
void set_row(int **mat, int row, int val)
{
    int i;
    for (i = 0; i < N; i++)
        mat[row][i] = val;
}

/*
    Devuelve el valor de la columna col de la matriz mat
*/
int get_col(int **mat, int col)
{
    return mat[0][col];
}

/*
    Devuelve el valor de la fila row de la matriz mat
*/
int get_row(int **mat, int row)
{
    return mat[row][0];
}

/*
    Reserva memoria para una matriz y devuelve el puntero
*/
int **alloc_matrix()
{
    int **mat;
    int i;
    mat = (int **)malloc(sizeof(int *) * M);
    for (i = 0; i < M; i++)
        mat[i] = (int *)malloc(sizeof(int) * N);
    return mat;
}

/*
    Libera la memoria de una matriz
*/
void dealloc_matrix(int **mat)
{
    int i;
    for (i = 0; i < M; i++)
        free(mat[i]);
    free(mat);
}

/*
    Imprime la matriz
*/
void print_matrix(int **mat)
{
    int i, j;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
    printf("\n");
}

/*
    Imprime una lista mats de len matrices
*/
void print_matrices(int ***mats, int len)
{
    int i;
    for (i = 0; i < len; i++)
        print_matrix(mats[i]);
}

int main (int argc, char *argv[])
{
    int ***mats = NULL;
    int ***enc_mats = NULL;
    int *repr = NULL;
    int derived_k1[M];
    int derived_k2[N];
    int i, j;

    /*
        Establece las claves del cifrador a k1 y k2
    */
    if (set_keys(k1, k2) == ERR)
    {
        printf("Error: permutaciones incorrectas\n");
        return ERR;
    }

    /*
        PARTE 1: CRIPTOANALISIS PARA SACAR LA PERMUTACION DE FILAS
    */

    /*
        Reservamos memoria para las listas de matrices y repr
    */
    mats = (int ***)malloc(sizeof(int **) * len1);
    enc_mats = (int ***)malloc(sizeof(int **) * len1);
    for (i = 0; i < len1; i++)
    {
        mats[i] = alloc_matrix();
        enc_mats[i] = alloc_matrix();
    }
    repr = (int *)malloc(sizeof(int) * len1);

    /*
        Para cada fila, la representa en base m y establece el valor de las
        filas de las matrices correspondientes a los valores adecuados
    */
    for (i = 0; i < M; i++)
    {
        int_to_repr(ALPH_SIZE, i, repr);
        for (j = 0; j < len1; j++)
            set_row( mats[j], i, repr[j] );
    }

    printf("-------- Matrices fila\n");
    print_matrices(mats, len1);

    /*
        Cifra las matrices usando el cifrador y las almacena en enc_mats
    */
    for (i = 0; i < len1; i++)
        encode( mats[i], enc_mats[i] );

    printf("-------- Matrices fila cifradas\n");
    print_matrices(enc_mats, len1);

    /*
        Para cada fila, obtiene los valores de las matrices en esa fila,
        consiguiendo una representacion y la transformándola a entero para
        saber la fila que era antes del cifrado
    */
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < len1; j++)
            repr[j] = get_row( enc_mats[j], i );
        derived_k1[i] = repr_to_int(ALPH_SIZE, repr, len1);
    }

    /*
        Libera las listas de matrices y repr
    */
    free(repr);
    for (i = 0; i < len1; i++)
    {
        dealloc_matrix(mats[i]);
        dealloc_matrix(enc_mats[i]);
    }
    free(mats);
    free(enc_mats);

    /*
        PARTE 2: CRIPTOANALISIS PARA SACAR LA PERMUTACION DE COLUMNAS

        Los comentarios de esta parte son los mismos que la anterior cambiando
        lo que hay que cambiar: filas -> columnas
    */

    mats = (int ***)malloc(sizeof(int **) * len2);
    enc_mats = (int ***)malloc(sizeof(int **) * len2);
    for (i = 0; i < len2; i++)
    {
        mats[i] = alloc_matrix();
        enc_mats[i] = alloc_matrix();
    }

    repr = (int *)malloc(sizeof(int) * len2);
    for (i = 0; i < N; i++)
    {
        int_to_repr(ALPH_SIZE, i, repr);
        for (j = 0; j < len2; j++)
            set_col( mats[j], i, repr[j] );
    }

    printf("-------- Matrices columna\n\n");
    print_matrices(mats, len2);

    for (i = 0; i < len2; i++)
        encode( mats[i], enc_mats[i] );

    printf("-------- Matrices columna cifradas\n\n");
    print_matrices(enc_mats, len2);

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < len2; j++)
            repr[j] = get_col( enc_mats[j], i );
        derived_k2[i] = repr_to_int(ALPH_SIZE, repr, len2);
    }

    free(repr);
    for (i = 0; i < len2; i++)
    {
        dealloc_matrix(mats[i]);
        dealloc_matrix(enc_mats[i]);
    }
    free(mats);
    free(enc_mats);

    /*
        RESULTADOS DEL CRIPTOANALISIS

        Se imprimen las claves derivadas del criptoanálisis
    */
    printf("-------- Resultados\n");
    printf("La permutación de filas obtenida es:\n\n");
    for (i = 0; i < M; i++)
        printf("%d ", derived_k1[i]);
    printf("\n\n");
    printf("La permutación de columnas obtenida es:\n\n");
    for (i = 0; i < N; i++)
        printf("%d ", derived_k2[i]);
    printf("\n\n");
    printf("-------- Fin Resultados\n");

    return OK;
}
