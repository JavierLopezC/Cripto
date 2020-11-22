#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "AES_tables.c"

#define DIRECT 0
#define INVERSE 1

#define OK 0
#define ERR -1

long search_table_AES(long input, int tabla){
	long fila = input / 16;
	long columna = input % 16;
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


int check_table(long input1, long input2, long multiplier, int tabla){
	long op1, op2, res_sum, res_prod, sum, prod;

	printf("###################################################################################################\n");

	if(tabla == DIRECT){
		printf("Comprobando no-linealidad tabla directa del AES con sumandos %lx y %lx, y multiplicador %ld:\n", input1, input2, multiplier);
	}else if(tabla == INVERSE){
		printf("Comprobando no-linealidad tabla inversa del AES con sumandos %lx y %lx, y multiplicador %ld:\n", input1, input2, multiplier);
	}

	printf("###################################################################################################\n");

	op1 = search_table_AES(input1, tabla);
	if(op1 == ERR) return ERR;
	op2 = search_table_AES(input2, tabla);
	if(op2 == ERR) return ERR;
	printf("f(%lx) = %lx\n", input1, op1);
	printf("f(%lx) = %lx\n", input2, op2);
	printf("\nf(%lx) + f(%lx) = %lx\n", input1, input2, (op1 + op2) % 256);

	sum = (input1 + input2) % 256;
	res_sum = search_table_AES(sum, tabla);
	if(res_sum == ERR) return ERR;
	printf("f(%lx + %lx) = f(%lx) = %lx\n", input1, input2, sum, res_sum);
	if(res_sum == op1 + op2){
		printf("\nCumple linealidad en la suma. f(x) + f(y) = f(x + y)\n\n");
	}else{
		printf("\nNo cumple lnealidad en la suma. f(x) + f(y) != f(x + y)\n\n");
	}

	printf("%ld * f(%lx) = %ld * %lx = %lx\n", multiplier, input1, multiplier, op1, (multiplier * op1) % 256);
	prod = (multiplier * input1) % 256;
	res_prod = search_table_AES(prod, tabla);
	if(res_prod == ERR) return ERR;
	printf("f(%ld * %lx) = f(%lx) = %lx\n", multiplier, input1, prod, res_prod);
	if(res_prod == multiplier * op1){
		printf("\nCumple linealidad en el producto. f(a * x) = a * f(x)\n\n");
	}else{
		printf("\nNo cumple lnealidad en el producto. f(a * x) != a * f(x)\n\n");
	}
	
	return OK;
}

int main(int argc, char *argv[]){
	long input1, input2, multiplier;
	char *ptr;
	int tabla, i = 1;

	if(argc != 7){
		printf("Número incorrecto de arumentos.");
		return ERR;
	}
    while (i < argc){
        if (argv[i][0] != '-'){
            printf("Error: sintaxis\n");
            return ERR;
        }else if (strcmp(argv[i], "-D") == 0){
            tabla = DIRECT;
        }else if (strcmp(argv[i], "-I") == 0){
            tabla = INVERSE;
        }else if (strcmp(argv[i], "-sums") == 0){
            input1 = strtol(argv[++i], &ptr, 16);
        	input2 = strtol(argv[++i], &ptr, 16);
        }else if (strcmp(argv[i], "-m") == 0){
            multiplier = strtol(argv[++i], &ptr, 16);
        }else{
            printf("Error: argumento no identificado\n");
            return ERR;
        }
        if (argc == i){
            printf("Error: sintaxis\n");
            return ERR;
        }else
            i++;
    }

    if(input1 > 255 || input2 > 255){
    	printf("Sumandos no válidos, por favor, introduce solo 2 dígitos hexadecimales.");
    	return ERR;
    }

	if(check_table(input1, input2, multiplier, tabla) == ERR) return ERR;

	return OK;
}