#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "gf8.h"
#include "AES_tables.c"

#define DIRECT 0
#define INVERSE 1

#define OK 0
#define ERR -1

#define OB      -1
#define OP      0

#define M_      0
#define OUT_	1

#define NARGS   2

static const int X[8][8] = {
	{1, 0, 0, 0, 1, 1, 1, 1},
	{1, 1, 0, 0, 0, 1, 1, 1},
	{1, 1, 1, 0, 0, 0, 1, 1},
	{1, 1, 1, 1, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 1, 1, 1, 1, 1, 0},
	{0, 0, 0, 1, 1, 1, 1, 1}
};

static const int Y[8][8] = {
	{0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0},
	{0, 1, 0, 0, 1, 0, 0, 1},
	{1, 0, 1, 0, 0, 1, 0, 0},
	{0, 1, 0, 1, 0, 0, 1, 0},
	{0, 0, 1, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 0, 1, 0, 0},
	{0, 1, 0, 0, 1, 0, 1, 0}
};

static const int C[8] = {1, 1, 0, 0, 0, 1, 1, 0};

static const int D[8] = {1, 0, 1, 0, 0, 0, 0, 0};

int args[6] = {OB,OP};     /* OB es para args. obligatorios */
int type = 0;
FILE *out;


/* AES_tables_calc {-C | -D} [-o file_out] */

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
        else if (strcmp(argv[i], "-C") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-D") == 0)
            args[M_] = i;
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
    if (strcmp(argv[args[M_]], "-C") == 0)
        type = DIRECT;
    else /* if (strcmp(argv[args[M_]], "-D") == 0) */
        type = INVERSE;
    /* Argumentos opcionales */
    if (args[OUT_] == OP)
        out = stdout;
    else
        out = fopen(argv[args[OUT_]], "w");
    return OK;
}

int print_args(char *argv[]) {
	if(type == 0){
		printf("Tipo de tabla C/D: C Directa\n");
	}else{
		printf("Tipo de tabla C/D: D Inversa\n");
	}

    printf("Salida: ");
    if (args[OUT_] == OP)
        printf("stdout\n");
    else
        printf("%s\n", argv[args[OUT_]]);
}

int affine_transf(int x){
	int i, j, ret, *desc, *res;
	desc = descomp_gf(x);
	res = (int*)malloc(8 * sizeof(int));
	for(i=0; i<8; i++){
		res[i] = 0;
		for(j=0; j<8; j++){
			if(type == DIRECT){
				res[i] = res[i] ^ (X[i][j] * desc[j]);
			}else{
				res[i] = res[i] ^ (Y[i][j] * desc[j]);
			}

		}
		if(type == DIRECT){
			res[i] = res[i] ^ C[i];
		}else{
			res[i] = res[i] ^ D[i];
		}
	}
	free(desc);
	return recomp_gf(res);
}

void print_matrix(int **m){
	int i, j;
	for(i=0; i<16; i++){
		fprintf(out, "{");
		for(j=0;j<16;j++){
			fprintf(out, " %02x ", m[i][j]);
		}
		fprintf(out, "}\n");
	}
	fprintf(out, "\n\n");
}

int** alloc_matrix(){
	int i, **M;
	M = (int**)malloc(16 * sizeof(int*));
	for(i=0;i<16;i++){
		M[i] = (int*)malloc(16 * sizeof(int));
	}
	return M;
}

void free_matrix(int **M){
	int i;
	for(i=0;i<16;i++){
		free(M[i]);
	}
	free(M);
}

int compare_matrix(int **m){
	int i, j;
	char *ptr;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			if(type == DIRECT){
				if(m[i][j] == (int)strtol(DIRECT_SBOX[i][j], &ptr, 16)){
					continue;
				}else{
					return 1;
				}
			}else if(type == INVERSE){
				if(m[i][j] == (int)strtol(INVERSE_SBOX[i][j], &ptr, 16)){
					continue;
				}else{
					return 1;
				}
			}else{
				printf("type no válido.\n");
				return -1;
			}
		}
	}
	return 0;
}

void clean(){
    fflush(out);
    fclose(out);
}

int main(int argc, char *argv[]){
	int i, check, line, col, value, **m;
	if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);
	m = alloc_matrix();
	for(i=0; i<256; i++){
		line = i/16;
		col = i%16;
		if(type == DIRECT){
			value = inv_gf8(i);
			value = affine_transf(value);
		}else{
			value = affine_transf(i);
			value = inv_gf8(value);
		}
		m[line][col] = value;
	}

	print_matrix(m);
	printf("Comparando S-Box con la del fichero AES_tables.c\n\n");

	check = compare_matrix(m);
	if(check == -1){
		free_matrix(m);
		clean();
		return ERR;
	}
	if(check == 0){
		printf("Las S-Boxes coinciden.\n\n");
	}else{
		printf("Las S-Boxes no coinciden.\n\n");
	}

	free_matrix(m);
	clean();

	return OK;
}
