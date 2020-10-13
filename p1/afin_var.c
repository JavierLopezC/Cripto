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
char *in, *out;

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
        in = NULL;
    }
    else
    {
        in = (char*)malloc(sizeof(char)*(1 + strlen(argv[args[IN_]])));
        if(in == NULL){
          printf("Error de memoria.\n");
          mpz_clear(a1);
          mpz_clear(a2);
          mpz_clear(b1);
          mpz_clear(b2);
          mpz_clear(m);
          return ERR;
        }
        strcpy(in, argv[args[IN_]]);
    }
    if (args[OUT_] == OP)
    {
        out = NULL;
    }
    else
    {
        out = (char*)malloc(sizeof(char)*(1 + strlen(argv[args[OUT_]])));
        if(out == NULL){
          printf("Error de memoria.\n");
          mpz_clear(a1);
          mpz_clear(a2);
          mpz_clear(b1);
          mpz_clear(b2);
          mpz_clear(m);
          if(in){
            free(in);
          }
          return ERR;
        }
        strcpy(out, argv[args[OUT_]]);
    }
    return OK;
}

int print_args(char *argv[]) {
    printf("Modo: %d\n", mode);
    gmp_printf("m = %Zd\n", m);
    gmp_printf("a = (%Zd, %Zd)\n", a1, a2);
    gmp_printf("b = (%Zd, %Zd)\n", b1, b2);
    printf("%s\n", in);
    printf("%s\n", out);
}

int affineDeCypher(){
  mpz_t gcd1, gcd2, gcd3, gcd4, res1, res2, res3;
  char ch;
  FILE *fin, *fout;
  int i;

  if(mpz_sgn(b1) == -1 | mpz_cmp(b1, m) > 0 | mpz_sgn(b2) == -1 | mpz_cmp(b2, m) > 0){
    printf("Error: argumento -b no válido para afín.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
    return ERR;
  }

  if(mpz_sgn(a1) <= 0 | mpz_sgn(a2) <= 0){
    printf("Error: argumento -a no válido para afín.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
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
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
    return ERR;
  }
  mpz_clear(gcd1);
  mpz_clear(gcd2);
  mpz_clear(gcd3);
  mpz_clear(gcd4);

  if(in == NULL){
    fin = stdin;
  }else{
    fin = fopen(in, "r");
    if(fin == NULL){
      printf("Error: no se pudo abrir el archivo de entrada.\n");
      mpz_clear(a1);
      mpz_clear(b1);
      mpz_clear(a2);
      mpz_clear(b2);
      mpz_clear(m);
      if(in){
        free(in);
      }
      if(out){
        free(out);
      }
      return ERR;
    }
  }

  if(out == NULL){
    fout = stdout;
  }else{
    fout = fopen(out, "w");
    if(fout == NULL){
      printf("Error: no se pudo abrir el archivo de salida.\n");
      mpz_clear(a1);
      mpz_clear(b1);
      mpz_clear(a2);
      mpz_clear(b2);
      mpz_clear(m);
      if(in){
        free(in);
      }
      if(out){
        free(out);
      }
      return ERR;
    }
  }
  i = 0;
  while((ch = fgetc(fin)) != EOF){
    ch = ch - 'A';
    if(ch < 0 | mpz_cmp_ui(m, ch) <= 0){
      printf("Caracter en texto cifrado no pertenece al alfabeto.\n");
      mpz_clear(a1);
      mpz_clear(b1);
      mpz_clear(a2);
      mpz_clear(b2);
      mpz_clear(m);
      if(in){
        free(in);
      }
      if(out){
        free(out);
      }
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
      mpz_ui_sub(res2, ch, b1);
    }else{
      mpz_ui_sub(res2, ch, b2);
    }
    mpz_mul(res3, res1, res2);
    mpz_mod(res1, res3, m);
    ch = 'A' + mpz_get_ui(res1);
    mpz_clear(res1);
    mpz_clear(res2);
    mpz_clear(res3);
    fprintf(fout, "%c", ch);
    i = (i + 1) % 4;
  }

  if(in != NULL){
    fclose(fin);
  }
  if(out != NULL){
    fclose(fout);
  }
  mpz_clear(a1);
  mpz_clear(b1);
  mpz_clear(a2);
  mpz_clear(b2);
  mpz_clear(m);
  if(in){
    free(in);
  }
  if(out){
    free(out);
  }

  return OK;
}

int affineCypher(){
  mpz_t gcd1, gcd2, gcd3, gcd4, res1, res2;
  char ch;
  FILE *fin, *fout;
  int i;

  if(mpz_sgn(b1) == -1 | mpz_cmp(b1, m) > 0 | mpz_sgn(b2) == -1 | mpz_cmp(b2, m) > 0){
    printf("Error: argumento -b no válido para afín.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
    return ERR;
  }

  if(mpz_sgn(a1) <= 0 | mpz_sgn(a2) <= 0){
    printf("Error: argumento -a no válido para afín.\n");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
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
    printf("Error: algun par de argumentos -a y -b no son coprimos.\n");
    mpz_clear(gcd1);
    mpz_clear(gcd2);
    mpz_clear(gcd3);
    mpz_clear(gcd4);
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
    return ERR;
  }
  mpz_clear(gcd1);
  mpz_clear(gcd2);
  mpz_clear(gcd3);
  mpz_clear(gcd4);

  if(in == NULL){
    fin = stdin;
  }else{
    fin = fopen(in, "r");
    if(fin == NULL){
      printf("Error: no se pudo abrir el archivo de entrada.\n");
      mpz_clear(a1);
      mpz_clear(b1);
      mpz_clear(a2);
      mpz_clear(b2);
      mpz_clear(m);
      if(in){
        free(in);
      }
      if(out){
        free(out);
      }
      return ERR;
    }
  }
  if(out == NULL){
    fout = stdout;
  }else{
    fout = fopen(out, "w");
    if(fout == NULL){
      printf("Error: no se pudo abrir el archivo de salida.\n");
      mpz_clear(a1);
      mpz_clear(b1);
      mpz_clear(a2);
      mpz_clear(b2);
      mpz_clear(m);
      if(in){
        free(in);
        fclose(fin);
      }
      if(out){
        free(out);
      }
      return ERR;
    }
  }
  i = 0;
  while((ch = fgetc(fin)) != EOF){
    ch = ch - 'A';
    if(ch < 0 | mpz_cmp_ui(m, ch) <= 0){
      printf("Caracter en texto plano no pertenece al alfabeto.\n");
      mpz_clear(a1);
      mpz_clear(b1);
      mpz_clear(a2);
      mpz_clear(b2);
      mpz_clear(m);
      if(in){
        free(in);
        fclose(fin);
      }
      if(out){
        free(out);
        fclose(fout);
      }
      return ERR;
    }
    mpz_init(res1);
    mpz_init(res2);
    if(i < 2){
      mpz_mul_ui(res1, a1, ch);
    }else{
      mpz_mul_ui(res1, a2, ch);
    }
    if((i % 2) == 0){
      mpz_add(res2, res1, b1);
    }else{
      mpz_add(res2, res1, b2);
    }
    mpz_mod(res1, res2, m);
    ch = 'A' + mpz_get_ui(res1);
    mpz_clear(res1);
    mpz_clear(res2);
    fprintf(fout, "%c", ch);
    i = (i + 1) % 4;
  }

  if(in != NULL){
    fclose(fin);
  }
  if(out != NULL){
    fclose(fout);
  }
  mpz_clear(a1);
  mpz_clear(b1);
  mpz_clear(a2);
  mpz_clear(b2);
  mpz_clear(m);
  if(in){
    free(in);
  }
  if(out){
    free(out);
  }
  return OK;
}



int main (int argc, char *argv[])
{
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    if(load_args(argv) == ERR)
    {
        return ERR;
    }
    print_args(argv);
    if(mode == ENC){
      return affineCypher();
    }else if(mode == DEC){
      return affineDeCypher();
    }
    printf("Error en el modo seleccionado.");
    mpz_clear(a1);
    mpz_clear(b1);
    mpz_clear(a2);
    mpz_clear(b2);
    mpz_clear(m);
    if(in){
      free(in);
    }
    if(out){
      free(out);
    }
    return ERR;
}
