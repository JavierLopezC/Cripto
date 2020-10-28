/*
    Este fichero tiene el código necesario para implementar el criptosistema
    de vigenere.

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MODE_   0
#define K_      1
#define IN_     2
#define OUT_    3

#define NARGS   4

#define ENC     1
#define DEC     2

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

int args[6] = {OB,OB,OP,OP};     /* OB es para args. obligatorios */

int mode;
int key_len;
char *key;
FILE *in, *out;

/* vigenere {-C|-D} {-k clave} [-i file_in] [-o file_out] */

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
        if (strcmp(argv[i], "-C") == 0)
        {
            args[MODE_] = i;
        }
        else if (strcmp(argv[i], "-D") == 0)
        {
            args[MODE_] = i;
        }
        else if (strcmp(argv[i], "-k") == 0)
        {
            args[K_] = ++i;
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
        mode = ENC;
    else // if (strcmp(argv[i], "-D") == 0)
        mode = DEC;
    key_len = strlen(argv[args[K_]]);
    key = (char *)malloc(sizeof(char) * key_len);
    assert(key);
    strncpy(key, argv[args[K_]], key_len);
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

void clean()
{
    free(key);
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

int print_args(char *argv[]) {
    printf("Modo: %d\n", mode);
    printf("Clave: %s\n", key);
    if (args[IN_] == OP)
        printf("stdin\n");
    else
        printf("%s\n", argv[args[IN_]]);
    if (args[OUT_] == OP)
        printf("stdout\n");
    else
        printf("%s\n", argv[args[OUT_]]);
}

/*
    PARTE 1: Funciones para implementar el cifrado/descifrado de vigenere
*/

/*
    Devuelve el caracter c codificado conociendo la posición en la que estamos
    ahora en el fichero (i), y usando el caracter adecuado de la clave
    (key[i % key_len])
*/
int encode_char(int c, int i)
{
    return (c + key[i % key_len] - 2*'A') % ALPH_SIZE + 'A';
}

/*
    Lee, codifica y escribe, hasta terminar el fichero o encontrar un caracter
    fuera del alfabeto
*/
int vigenere_encode()
{
    int c, i = 0;
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
        fputc(encode_char(c, i++), out);
    fputc('\n', out);
}

/*
    Devuelve el caracter c decodificado conociendo la posición en la que estamos
    ahora en el fichero (i), y usando el caracter adecuado de la clave
    (key[i % key_len])
*/
int decode_char(int c, int i)
{
    return (c - key[i % key_len] + ALPH_SIZE) % ALPH_SIZE + 'A';
}

/*
    Lee, decodifica y escribe, hasta terminar el fichero o encontrar un caracter
    fuera del alfabeto
*/
int vigenere_decode()
{
    int c, i = 0;
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
        fputc(decode_char(c, i++), out);
    fputc('\n', out);
}

/*
    Comprueba que el string str es válida, esto es, que no tiene chars fuera
    del alfabeto. Se usa parar comprobar que la clave es válida
*/
int is_valid_str(char *str, int str_len)
{
    for (int i = 0; i < str_len; i++)
    {
        if (!IS_IN_ALPH(str[i]))
            return ERR;
    }
    return OK;
}

/*
    El programa cifra/descifra el contenido de los ficheros que toquen char a
    char hasta encontrar un caracter fuera del alfabeto o EOF (que tmbn está
    fuera del alfabeto but ok)

    todo: apañar un poco todo, cerrar ficheros liberar memoria...
*/

int main (int argc, char *argv[])
{
    /* Parsea, carga e imprime los argumentos */
    if (parse_args(argc, argv) == ERR)
        return ERR;
    load_args(argv);
    print_args(argv);

    /* Comprueba que la clave es válida */
    if (is_valid_str(key, key_len) == ERR)
    {
        printf("Error: la clave contiene chars fuera del alfabeto\n");
        return ERR;
    }

    /* Codifica o decodifica dependiendo del modo */
    if (mode == ENC)
        vigenere_encode();
    else /* if (mode == DEC) */
        vigenere_decode();

    /* Limpia la memoria utilizada */
    clean();

    return OK;
}
