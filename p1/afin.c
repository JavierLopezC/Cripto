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
        in = NULL;
    }
    else
    {
        in = (char*)malloc(sizeof(char)*(1 + strlen(argv[args[IN_]])));
        if(in == NULL){
          printf("Error de memoria.\n");
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
    gmp_printf("a = %Zd\n", a);
    gmp_printf("b = %Zd\n", b);
    printf("%s\n", in);
    printf("%s\n", out);
}

int affineDeCypher(){
  mpz_t gcd, res1, res2, res3;
  char ch;
  FILE *fin, *fout;

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

  if(in == NULL){
    fin = stdin;
  }else{
    fin = fopen(in, "r");
    if(fin == NULL){
      printf("Error: no se pudo abrir el archivo de entrada.\n");
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
  }

  if(out == NULL){
    fout = stdout;
  }else{
    fout = fopen(out, "w");
    if(fout == NULL){
      printf("Error: no se pudo abrir el archivo de salida.\n");
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
  }

  while((ch = fgetc(fin)) != EOF){
    ch = ch - 'A';
    if(ch < 0 | mpz_cmp_ui(m, ch) <= 0){
      printf("Caracter en texto cifrado no pertenece al alfabeto.\n");
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
    mpz_init(res1);
    mpz_init(res2);
    mpz_init(res3);
    mpz_invert(res1, a, m);
    mpz_ui_sub(res2, ch, b);
    mpz_mul(res3, res1, res2);
    mpz_mod(res1, res3, m);
    ch = 'A' + mpz_get_ui(res1);
    mpz_clear(res1);
    mpz_clear(res2);
    mpz_clear(res3);
    fprintf(fout, "%c", ch);
  }

  if(in != NULL){
    fclose(fin);
  }
  if(out != NULL){
    fclose(fout);
  }

  return OK;
}

int affineCypher(){
  mpz_t gcd, res1, res2;
  char ch;
  FILE *fin, *fout;

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

  if(in == NULL){
    fin = stdin;
  }else{
    fin = fopen(in, "r");
    if(fin == NULL){
      printf("Error: no se pudo abrir el archivo de entrada.\n");
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
  }
  if(out == NULL){
    fout = stdout;
  }else{
    fout = fopen(out, "w");
    if(fout == NULL){
      printf("Error: no se pudo abrir el archivo de salida.\n");
      mpz_clear(a);
      mpz_clear(b);
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

  while((ch = fgetc(fin)) != EOF){
    ch = ch - 'A';
    if(ch < 0 | mpz_cmp_ui(m, ch) <= 0){
      printf("Caracter en texto plano no pertenece al alfabeto.\n");
      mpz_clear(a);
      mpz_clear(b);
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
    mpz_mul_ui(res1, a, ch);
    mpz_add(res2, res1, b);
    mpz_mod(res1, res2, m);
    ch = 'A' + mpz_get_ui(res1);
    mpz_clear(res1);
    mpz_clear(res2);
    fprintf(fout, "%c", ch);
  }

  if(in != NULL){
    fclose(fin);
  }
  if(out != NULL){
    fclose(fout);
  }
  mpz_clear(a);
  mpz_clear(b);
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
