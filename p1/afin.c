#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "euclid.h"

#define MODE_   0
#define M_      1
#define A_      2
#define B_      3
#define IN_     4
#define OUT_    5

#define NARGS   6

#define ENC     1
#define DEC     2

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

int args[6] = {OB,OB,OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
mpz_t m, a, b;
FILE *in, *out;

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
        else if (strcmp(argv[i], "-m") == 0)
        {
            args[M_] = ++i;
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            args[A_] = ++i;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            args[B_] = ++i;
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
    {
        mode = ENC;
    }
    else // if (strcmp(argv[i], "-D") == 0)
    {
        mode = DEC;
    }
    mpz_init(m);
    mpz_init(a);
    mpz_init(b);
    mpz_set_str(m, argv[args[M_]], 10);
    mpz_set_str(a, argv[args[A_]], 10);
    mpz_set_str(b, argv[args[B_]], 10);
    /* Argumentos opcionales */
    if (args[IN_] == OP)
    {
        in = stdin;
    }
    else
    {
        in = fopen(argv[args[IN_]], "r");
    }
    if (args[OUT_] == OP)
    {
        out = stdout;
    }
    else
    {
        out = fopen(argv[args[OUT_]], "w");
    }
    return OK;
}

int print_args(char *argv[])
{
    printf("Modo: %d\n", mode);
    gmp_printf("m = %Zd\n", m);
    gmp_printf("a = %Zd\n", a);
    gmp_printf("b = %Zd\n", b);
    if (args[IN_] == OP)
    {
        printf("stdin\n");
    }
    else
    {
        printf("%s\n", argv[args[IN_]]);
    }
    if (args[OUT_] == OP)
    {
        printf("stdout\n");
    }
    else
    {
        printf("%s\n", argv[args[OUT_]]);
    }
}

int check_args(){
    mpz_t gcd;
    /*comprobar 0 <= b < m*/
    if(mpz_sgn(b) == -1 || mpz_cmp(b, m) > 0){
        printf("Error: argumento -b no válido para afín.\n");
        mpz_clear(a);
        mpz_clear(b);
        mpz_clear(m);
        if(in){
            free(in);
        }
        if(out){
            free(out);
        }
        return ERR;
    }
    /*comprobar 0 < a*/
    if(mpz_sgn(a) <= 0){
        printf("Error: argumento -a no válido para afín.\n");
        mpz_clear(a);
        mpz_clear(b);
        mpz_clear(m);
        if(in){
            free(in);
        }
        if(out){
            free(out);
        }
        return ERR;
    }

    mpz_init(gcd);
    euclid(gcd, a, b);
    /*comprobar a y b coprimos*/
    if(mpz_cmp_ui(gcd, 1) != 0){
        printf("Error: argumentos -a y -b no son coprimos.\n");
        mpz_clear(gcd);
        mpz_clear(a);
        mpz_clear(b);
        mpz_clear(m);
        if(in){
            free(in);
        }
        if(out){
            free(out);
        }
        return ERR;
    }
    mpz_clear(gcd);
    return OK;
}

int encode_char(int c){
    mpz_t res1, res2;
    c = c - 'A';
    /*comprobar c en alfabeto*/
    if(c < 0 | mpz_cmp_ui(m, c) <= 0){
        printf("Caracter en texto plano no pertenece al alfabeto.\n");
        mpz_clear(a);
        mpz_clear(b);
        mpz_clear(m);
        return ERR;
    }
    mpz_init(res1);
    mpz_init(res2);
    /*c = a*c + b mod m*/
    mpz_mul_ui(res1, a, c);
    mpz_add(res2, res1, b);
    mpz_mod(res1, res2, m);
    c = 'A' + mpz_get_ui(res1);
    mpz_clear(res1);
    mpz_clear(res2);
    return c;
}

int decode_char(int c){
    mpz_t res1, res2, res3;
    c = c - 'A';
    /*comprobar c en alfabeto*/
    if(c < 0 | mpz_cmp_ui(m, c) <= 0){
        printf("Caracter en texto plano no pertenece al alfabeto.\n");
        mpz_clear(a);
        mpz_clear(b);
        mpz_clear(m);
        return ERR;
    }
    mpz_init(res1);
    mpz_init(res2);
    mpz_init(res3);
    /*c = a⁻¹ * (c - b) mod m*/
    mpz_invert(res1, a, m);
    mpz_ui_sub(res2, c, b);
    mpz_mul(res3, res1, res2);
    mpz_mod(res1, res3, m);
    c = 'A' + mpz_get_ui(res1);
    mpz_clear(res1);
    mpz_clear(res2);
    mpz_clear(res3);
    return c;
}

int affine_encode(){
    int c;
    /*comprobar argumentos*/
    if(check_args() == ERR){
        return ERR;
    }
    /*recorrer fichero*/
    while ((c = fgetc(in)) != EOF){
        if(c == '\n'){
            break;
        }
        /*cifrar c*/
        c = encode_char(c);
        if(c == ERR){
            return ERR;
        }
        /*imprimir en salida*/
        fputc(c, out);
    }
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(m);
    return OK;
}

int affine_decode(){
    int c;
    /*comprobar argumentos*/
    if(check_args() == ERR){
        return ERR;
    }
    /*recorrer fichero*/
    while ((c = fgetc(in)) != EOF){
        if(c == '\n'){
            break;
        }
        /*descifrar c*/
        c = decode_char(c);
        if(c == ERR){
            return ERR;
        }
        /*imprimir en salida*/
        fputc(c, out);
    }
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(m);
    return OK;
}



int main (int argc, char *argv[]){
    int ret;
    /*parsear y guardar argumentos*/
    if (parse_args(argc, argv) == ERR){
        return ERR;
    }
    if(load_args(argv) == ERR){
        return ERR;
    }
    print_args(argv);
    /*cifrar*/
    if(mode == ENC){
        ret = affine_encode();
        if(in != stdin){
            fclose(in);
        }
        if(out != stdout){
            fclose(out);
        }
        return ret;
    /*descifrar*/
    }else if(mode == DEC){
        ret = affine_decode();
        if(in != stdin){
            fclose(in);
        }
        if(out != stdout){
            fclose(out);
        }
        return ret;
    }
    printf("Error en el modo seleccionado.");
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(m);
    if(in != stdin){
        fclose(in);
    }
    if(out != stdout){
        fclose(out);
    }
    return ERR;
}
