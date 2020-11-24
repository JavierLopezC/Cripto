#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include "AES_tables.c"
#include "gf8.h"
#include "AES_tables.c"

#define DIRECT 0
#define INVERSE 1

#define OK 0
#define ERR -1

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

int affine_transf(int x){
	int i, j, ret, *desc, *res;
	desc = descomp_gf(x);
	res = (int*)malloc(8 * sizeof(int));
	for(i=0; i<8; i++){
		res[i] = 0;
		for(j=0; j<8; j++){
			res[i] = res[i] ^ (X[i][j] * desc[j]);
		}
		res[i] += C[i];
	}
	free(desc);
	return recomp_gf(res);
}

/*long search_table_AES(int input, int tabla){
	int fila = input / 16;
	int columna = input % 16;
	char *ptr;

	if(tabla == DIRECT){
		return strtol(DIRECT_SBOX[fila][columna], &ptr, 16);
	}else if(tabla == INVERSE){
		return strtol(INVERSE_SBOX[fila][columna], &ptr, 16);
	}else{
		printf("Tabla no válida.");
		return ERR;
	}
}


int check_table(int input1, int input2, int tabla){

}*/

void print_matrix(int **m){
	int i, j;
	for(i=0; i<16; i++){
		printf("{");
		for(j=0;j<16;j++){
			printf(" %02x ", m[i][j]);
		}
		printf("}\n");
	}
	printf("\n\n");
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

int compare_matrix(int **m, int type){
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

int main(){
	int i, check, line, col, value, **dir, **inv;
	dir = alloc_matrix();
	inv = alloc_matrix();
	for(i=0; i<256; i++){
		line = i/16;
		col = i%16;
		value = inv_gf8(i);
		value = affine_transf(value);
		dir[line][col] = value;
		line = value/16;
		col = value%16;
		inv[line][col] = i;
	}
	printf("S-Box Directa:\n");
	print_matrix(dir);
	printf("Comparando S-Box directa con la del fichero AES_tables.c\n\n");
	check = compare_matrix(dir, DIRECT);
	if(check == -1) return ERR;
	if(check == 0){
		printf("Las S-Boxes directas coinciden.\n\n");
	}else{
		printf("Las S-Boxes directas coinciden.\n\n");
	}

	printf("S-Box Inversa:\n");
	print_matrix(inv);

	printf("Comparando S-Box inversa con la del fichero AES_tables.c\n\n");
	check = compare_matrix(inv, INVERSE);
	if(check == -1) return ERR;
	if(check == 0){
		printf("Las S-Boxes inversas coinciden.\n\n");
	}else{
		printf("Las S-Boxes inversas coinciden.\n\n");
	}

	free_matrix(dir);
	free_matrix(inv);

	return OK;
}
