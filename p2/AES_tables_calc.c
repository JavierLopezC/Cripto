#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "AES_tables.c"
#include "euclid_gf.h"

#define DIRECT 0
#define INVERSE 1

#define OK 0
#define ERR -1

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

void main(){
	int inv;
	char *ptr;
	struct ext_ret_gf ret;
	ext_euclid_gf(&ret, (int)strtol("95", &ptr, 16));
	inv = ret.inv;
	printf("Inverso de 95: %02X\n", inv);
}
