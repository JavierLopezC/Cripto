#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "euclid.h"

#define MODE_   0
#define M_      1
#define A1_     2
#define A2_     3
#define B1_     4
#define B2_     5
#define IN_     6
#define OUT_    7

#define NARGS   8

#define ENC     1
#define DEC     2

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

int args[8] = {OB,OB,OB,OB,OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
mpz_t m, a1, a2, b1, b2;
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
            args[A1_] = ++i;
            args[A2_] = ++i;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            args[B1_] = ++i;
            args[B2_] = ++i;
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
    mpz_init(a1);
    mpz_init(b1);
    mpz_init(a2);
    mpz_init(b2);
    mpz_set_str(m, argv[args[M_]], 10);
    mpz_set_str(a1, argv[args[A1_]], 10);
    mpz_set_str(b1, argv[args[B1_]], 10);
    mpz_set_str(a2, argv[args[A2_]], 10);
    mpz_set_str(b2, argv[args[B2_]], 10);
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

int print_args(char *argv[]) {
    printf("Modo: %d\n", mode);
    gmp_printf("m = %Zd\n", m);
    gmp_printf("a = (%Zd, %Zd)\n", a1, a2);
    gmp_printf("b = (%Zd, %Zd)\n", b1, b2);
    if (args[IN_] == OP){
      printf("stdin\n");
    }else{
      printf("%s\n", argv[args[IN_]]);
    }
    if (args[OUT_] == OP){
      printf("stdout\n");
    }else{
      printf("%s\n", argv[args[OUT_]]);
    }
}


int check_args(){
  mpz_t gcd1, gcd2, gcd3, gcd4;
  if(mpz_sgn(b1) == -1 | mpz_cmp(b1, m) > 0 | mpz_sgn(b2) == -1 | mpz_cmp(b2, m) > 0){
    printf("Error: argumento -b no válido para afín.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    return ERR;
  }

  if(mpz_sgn(a1) <= 0 | mpz_sgn(a2) <= 0){
    printf("Error: argumento -a no válido para afín.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    return ERR;
  }

  mpz_init(gcd1);
  mpz_init(gcd2);
  mpz_init(gcd3);
  mpz_init(gcd4);
  euclid(gcd1, a1, b1);
  euclid(gcd2, a1, b2);
  euclid(gcd3, a2, b1);
  euclid(gcd4, a2, b2);
  if(mpz_cmp_ui(gcd1, 1) != 0 | mpz_cmp_ui(gcd2, 1) != 0 | mpz_cmp_ui(gcd3, 1) != 0 | mpz_cmp_ui(gcd4, 1) != 0){
    printf("Error: argumentos -a y -b no son coprimos.\n");
    mpz_clear(gcd1);
    mpz_clear(gcd2);
    mpz_clear(gcd3);
    mpz_clear(gcd4);
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    return ERR;
  }
  mpz_clear(gcd1);
  mpz_clear(gcd2);
  mpz_clear(gcd3);
  mpz_clear(gcd4);
}

int encode_char(int c, int i){
  mpz_t res1, res2;
  c = c - 'A';
  if(c < 0 | mpz_cmp_ui(m, c) <= 0){
    printf("Caracter en texto plano no pertenece al alfabeto.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    return ERR;
  }
  mpz_init(res1);
  mpz_init(res2);
  if(i < 2){
    mpz_mul_ui(res1, a1, c);
  }else{
    mpz_mul_ui(res1, a2, c);
  }
  if((i % 2) == 0){
    mpz_add(res2, res1, b1);
  }else{
    mpz_add(res2, res1, b2);
  }
  mpz_mod(res1, res2, m);
  c = 'A' + mpz_get_ui(res1);
  mpz_clear(res1);
  mpz_clear(res2);
  return c;
}

int decode_char(int c, int i){
  mpz_t res1, res2, res3;
  c = c - 'A';
  if(c < 0 | mpz_cmp_ui(m, c) <= 0){
    printf("Caracter en texto cifrado no pertenece al alfabeto.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    return ERR;
  }
  mpz_init(res1);
  mpz_init(res2);
  mpz_init(res3);
  if(i < 2){
    mpz_invert(res1, a1, m);
  }else{
    mpz_invert(res1, a2, m);
  }
  if((i % 2) == 0){
    mpz_ui_sub(res2, c, b1);
  }else{
    mpz_ui_sub(res2, c, b2);
  }
  mpz_mul(res3, res1, res2);
  mpz_mod(res1, res3, m);
  c = 'A' + mpz_get_ui(res1);
  mpz_clear(res1);
  mpz_clear(res2);
  mpz_clear(res3);
  return c;
}

int affine_var_encode(){
  int c, i;
  if(check_args() == ERR){
    return ERR;
  }
  i = 0;
  while ((c = fgetc(in)) != EOF){
    if(c == '\n'){
      break;
    }
    c = encode_char(c, i);
    if(c == ERR){
      return ERR;
    }
    fputc(c, out);
    i = (i + 1) % 4;
  }
  mpz_clear(a1);
  mpz_clear(b1);
  mpz_clear(a2);
  mpz_clear(b2);
  mpz_clear(m);
  return OK;
}

int affine_var_decode(){
  int c, i;
  if(check_args() == ERR){
    return ERR;
  }
  i = 0;
  while ((c = fgetc(in)) != EOF){
    if(c == '\n'){
      break;
    }
    c = decode_char(c, i);
    if(c == ERR){
      return ERR;
    }
    fputc(c, out);
    i = (i + 1) % 4;
  }
  mpz_clear(a1);
  mpz_clear(b1);
  mpz_clear(a2);
  mpz_clear(b2);
  mpz_clear(m);
  return OK;
}

int main (int argc, char *argv[]){
  int ret;
  if (parse_args(argc, argv) == ERR){
    return ERR;
  }
  if(load_args(argv) == ERR){
    return ERR;
  }
  print_args(argv);
  if(mode == ENC){
    ret = affine_var_encode();
    if(in != stdin){
      fclose(in);
    }
    if(out != stdout){
      fclose(out);
    }
    return ret;
  }else if(mode == DEC){
    ret = affine_var_decode();
    if(in != stdin){
      fclose(in);
    }
    if(out != stdout){
      fclose(out);
    }
    return ret;
  }
  printf("Error en el modo seleccionado.");
  mpz_clear(a1);
  mpz_clear(b1);
  mpz_clear(a2);
  mpz_clear(b2);
  mpz_clear(m);
  if(in != stdin){
    fclose(in);
  }
  if(out != stdout){
    fclose(out);
  }
  return ERR;
}
