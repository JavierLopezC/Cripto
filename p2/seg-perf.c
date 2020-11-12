/*
    Código para el ejercicio de Seguridad Perfecta

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>

#define M_      0
#define IN_     1
#define OUT_    2

#define NARGS   3

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

#define EP      0
#define NEP     1

int args[6] = {OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
FILE *in, *out;

double probs[ALPH_SIZE] = {0};
int freqs_enc[ALPH_SIZE] = {0};
double probs_cond[ALPH_SIZE][ALPH_SIZE], media[ALPH_SIZE], desv[ALPH_SIZE];
int total = 0;

/* seg-perf {-P | -I} [-i file_in] [-o file_out] */

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
        else if (strcmp(argv[i], "-P") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-I") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-i") == 0)
            args[IN_] = ++i;
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
    if (strcmp(argv[args[M_]], "-P") == 0)
        mode = EP;
    else /* if (strcmp(argv[args[M_]], "-I") == 0) */
        mode = NEP;
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

int print_args(char *argv[]) {
    printf("Modo EP/NEP: %d\n", mode);
    printf("Entrada: ");
    if (args[IN_] == OP)
        printf("stdin\n");
    else
        printf("%s\n", argv[args[IN_]]);
    printf("Salida: ");
    if (args[OUT_] == OP)
        printf("stdout\n");
    else
        printf("%s\n", argv[args[OUT_]]);
}

void clean()
{
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

void reset_files(char *argv[]){
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
    if (args[IN_] == OP)
        in = stdin;
    else
        in = fopen(argv[args[IN_]], "r");
    if (args[OUT_] == OP)
        out = stdout;
    else
        out = fopen(argv[args[OUT_]], "r");
}

int key_gen(gmp_randstate_t state){
    mpz_t key, max;
    int final_key, module;
    if(mode == NEP) module = 37;
    else module = 26;
    mpz_init(key);
    mpz_init(max);
    mpz_set_ui(max, module);
    mpz_urandomm(key, state, max);
    final_key = mpz_get_ui(key);
    mpz_clear(max);
    mpz_clear(key);
    return final_key;
}

/*
    Devuelve el caracter c codificado por desplazamiento vía r
*/
int encode_char(int c, int r)
{
    return (c + r - 'A') % ALPH_SIZE + 'A';
}

/*
    Establece la clave mediante srand. Después, char a char, lee, codifica y
    escribe, hasta terminar el fichero  o encontrar un char fuera del alfabeto
*/
int encode_P()
{
    gmp_randstate_t state;
    int c;
    gmp_randinit_default(state);
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
        fputc(encode_char(c, key_gen(state)), out);
    fputc('\n', out);
}

void calc_probs(){
    int i, j, c, c_enc;
    for(i = 0; i<ALPH_SIZE; i++){
        for(j = 0; j<ALPH_SIZE; j++)
        probs_cond[i][j] = 0;
    }
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c)){
        probs[c - 'A']++;
        total++;
        if ((c_enc = fgetc(out)) != EOF && IS_IN_ALPH(c_enc)){
            probs_cond[c_enc - 'A'][c - 'A']++;
            freqs_enc[c_enc - 'A']++;
        }else{
            printf("Texto cifrado incorrecto.\n");
        }
    }
    for(i = 0; i<ALPH_SIZE; i++){
        probs[i] /= total;
    }
    for(i = 0; i<ALPH_SIZE; i++){
        for(j = 0; j<ALPH_SIZE; j++)
        probs_cond[i][j] /= freqs_enc[i];
    }
    for(i = 0; i < ALPH_SIZE; i++){
        for(j = 0, media[i] = 0; j < ALPH_SIZE; j++){
            media[i] += fabs(probs_cond[j][i] - probs[i]);
        }
        media[i] /= ALPH_SIZE;
    }

    for(i = 0; i < ALPH_SIZE; i++){
        for(j = 0, desv[i] = 0; j < ALPH_SIZE; j++){
            desv[i] += pow((fabs(probs_cond[j][i] - probs[i]) - media[i]), 2);
        }
        desv[i] /= ALPH_SIZE;
    }

}

void print_probs(){
    int i, j;
    double med = 0;
    for(i = 0; i < ALPH_SIZE; i++){
        printf("------------------------------------------");
        printf("\nP(%c) = %lf\n", i + 'A', probs[i]);
        for(j = 0; j < ALPH_SIZE; j++){
            if(j % 2 == 0) printf("\n");
            printf("P(%c|%c) = %lf\t", i + 'A', j + 'A', probs_cond[j][i]);
        }
        printf("\n\nError medio: %lf\t", media[i]);
        printf("Desviación del error: %lf\n", desv[i]);
        med += media[i];
    }
    printf("------------------------------------------");
    printf("\n\nError medio total: %lf\n", med/ALPH_SIZE);
}

/*
    PARTE 1: Funciones para hacer el ejercicio
*/

int main (int argc, char *argv[])
{
    int i;
    /* Parsea, carga e imprime los argumentos */
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    encode_P();
    reset_files(argv);
    calc_probs();

    print_probs();

    clean();

    return OK;
}
