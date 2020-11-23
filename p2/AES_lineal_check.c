#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "AES_tables.c"

#define DIRECT 0
#define INVERSE 1

#define OK 0
#define ERR -1

long search_table_AES(int input, int tabla){
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
	int op1, op2, res_sum, res_prod, sum, prod;

	printf("######################################################################\n");

	if(tabla == DIRECT){
		printf("Comprobando no-linealidad tabla directa del AES con sumandos %02X y %02X:\n", input1, input2);
	}else if(tabla == INVERSE){
		printf("Comprobando no-linealidad tabla inversa del AES con sumandos %02X y %02X:\n", input1, input2);
	}

	printf("######################################################################\n");

	op1 = search_table_AES(input1, tabla);
	if(op1 == ERR) return ERR;
	op2 = search_table_AES(input2, tabla);
	if(op2 == ERR) return ERR;
	printf("f(%02X) = %02X\n", input1, op1);
	printf("f(%02X) = %02X\n", input2, op2);
	printf("\nf(%02X) XOR f(%02X) = %02X\n", input1, input2, op1^op2);

	sum = input1^input2;
	res_sum = search_table_AES(sum, tabla);
	if(res_sum == ERR) return ERR;
	printf("f(%02X XOR %02X) = f(%02X) = %02X\n", input1, input2, sum, res_sum);
	if(res_sum == (op1 ^ op2)){
		printf("\nCumple linealidad en XOR. f(x) XOR f(y) = f(x XOR y)\n\n");
		return 100;
	}else{
		printf("\nNo cumple lnealidad en XOR. f(x) XOR f(y) != f(x XOR y)\n\n");
	}
	return OK;
}

int main(){
	int res, i, j, k, l, lineal_dir[300][2], lineal_inv[300][2];

	printf("\n\nPROBANDO TABLA DIRECTA:\n\n");
	k = 0;
	for(i=0; i < 256; i++){
		for(j = 0; j < 256; j++){
			res = check_table(i, j, DIRECT);
			if(res == ERR) return ERR;
			else if(res == 100){
				lineal_dir[k][0] = i;
				lineal_dir[k][1] = j;
				k++;
			}
		}
	}

	printf("\n\nPROBANDO TABLA INVERSA:\n\n");
	l = 0;
	for(i=0; i < 256; i++){
		for(j = 0; j < 256; j++){
			res = check_table(i, j, INVERSE);
			if(res == ERR) return ERR;
			else if(res == 100){
				lineal_inv[l][0] = i;
				lineal_inv[l][1] = j;
				l++;
			}
		}
	}

	printf("\n##############################################################################");
	printf("\n\nLa tabla directa cumple linealidad en XOR con los siguientes pares de datos:\n\n");
	for(i=1; i <= k; i++){
		printf("\t(%02X, %02X)", lineal_dir[i - 1][0], lineal_dir[i - 1][1]);
		if(i % 6 == 0) printf("\n");
	}
	printf("\n##############################################################################");
	printf("\n\nLa tabla inversa cumple linealidad en XOR con los siguientes pares de datos:\n\n");
	for(i=1; i <= l; i++){
		printf("\t(%02X, %02X)", lineal_inv[i - 1][0], lineal_inv[i - 1][1]);
		if(i % 6 == 0) printf("\n");
	}

	printf("\n\nEsto supone que:\n\n");
	printf("\tLa tabla directa es lineal en XOR para %d pares de 65536 posibles, un %lf%%\n", k, ((double)k/65536) * 100);
	printf("\tLa tabla inversa es lineal en XOR para %d pares de 65536 posibles, un %lf%%\n", l, ((double)l/65536) * 100);

	return OK;
}
