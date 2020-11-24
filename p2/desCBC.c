/*
    Implementación de DES

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#include "desCBC.h"

#define M_      0
#define K_      1
#define IV_     2
#define IN_     3
#define OUT_    4
#define V_      5

#define NARGS   6

#define OB      -1
#define OP      0

#define ENC     0
#define DEC     1

#define ERR     -1
#define OK      0

#define MASK(k) (0xFFFFFFFFFFFFFFFF >> (64 - k))

int args[NARGS] = {OB,OB,OB,OP,OP,OP};     /* OB es para args. obligatorios */

int mode;
FILE *in;
FILE *out;
int verbose;

uint64_t key;
uint64_t cbc_iv;
uint64_t block;
uint64_t enc_subkeys[ROUNDS] = {0};
uint64_t dec_subkeys[ROUNDS] = {0};


/*
    desCBC      {-C | -D} { -k clave } { -iv vector inicializacion }
                [-i file_in] [-o file_out] [ -v ]
*/

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
        else if (strcmp(argv[i], "-C") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-D") == 0)
            args[M_] = i;
        else if (strcmp(argv[i], "-k") == 0)
            args[K_] = ++i;
        else if (strcmp(argv[i], "-iv") == 0)
            args[IV_] = ++i;
        else if (strcmp(argv[i], "-i") == 0)
            args[IN_] = ++i;
        else if (strcmp(argv[i], "-o") == 0)
            args[OUT_] = ++i;
        else if (strcmp(argv[i], "-v") == 0)
            args[V_] = i;
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
    if (strcmp(argv[args[M_]], "-C") == 0)
        mode = ENC;
    else /* if (strcmp(argv[args[M_]], "-D") == 0) */
        mode = DEC;
    key = (uint64_t) strtoull(argv[args[K_]], NULL, 16);
    cbc_iv = (uint64_t) strtoull(argv[args[IV_]], NULL, 16);
    /* Argumentos opcionales */
    in = stdin;
    out = stdout;
    verbose = 0;
    if (args[IN_] != OP)
        in = fopen(argv[args[IN_]], "r");
    if (args[OUT_] != OP)
        out = fopen(argv[args[OUT_]], "w");
    if (args[V_] != OP)
        verbose = 1;
    return OK;
}

int print_args(char *argv[]) {
    printf("Modo C/D: %d\n", mode);
    printf("Clave:\t");
    print_hex(key);
    printf("IV:\t");
    print_hex(cbc_iv);
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
    if (args[V_] != OP)
        printf("Verbose = true\n");
}

void clean()
{
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

/*
    PARTE 1: Funciones para DES
*/

void print_bin(uint64_t n)
{
    for ( int i = 0; i < 64; i++)
        printf("%lld", (n >> (63 - i)) & 0x1ULL);
    printf("\n");
}

void print_hex(uint64_t n)
{
    printf("%"PRIx64"\n", n);
}

uint64_t reverse(uint64_t b, int n)
{
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < n; i++)
        foo |= ((b >> i) & 0x1ULL) << (n - 1 - i);
    return foo;
}

void rotate_halves(const short n)
{
    /*
        0xFFFFFFF := primeros 28 bits a 1, resto 0
    */
    uint64_t l = key & 0xFFFFFFFULL;
    uint64_t r = (key >> 28) & 0xFFFFFFFULL;
    r = ((r >> n) | (r << (28 - n))) & 0xFFFFFFFULL;
    l = ((l >> n) | (l << (28 - n))) & 0xFFFFFFFULL;
    key = (r << 28) | l;
}

uint64_t permute(uint64_t b, const short p[], int n)
{
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < n; i++)
        foo |= ((b >> (p[i]-1)) & 0x1ULL) << i;
    return foo;
}

void generate_subkeys()
{
    key = reverse(key, 64);
    key = permute(key, PC1, BITS_IN_PC1);
    for (int i = 0; i < ROUNDS; i++)
    {
        rotate_halves(ROUND_SHIFTS[i]);
        enc_subkeys[i] = permute(key, PC2, BITS_IN_PC2);
        dec_subkeys[ROUNDS - 1 - i] = enc_subkeys[i];
    }
    key = reverse(key, 64);
}

uint64_t rand_byte()
{
    /*
        0xFF := primer byte a 1, resto 0
    */
    return rand() & 0xFFULL;
}

int get_block()
{
    int amount;
    amount = fread(&block, 1, 8, in);
    if (0 < amount && amount < 8)
    {
        block &= MASK(8*amount);
        for (int i = amount; i < 8; i++)
            block |= rand_byte() << 8*i;
    }
    return amount;
}

void put_block()
{
    fwrite(&block, 1, 8, out);
}

uint64_t s_box(uint64_t input, int s)
{
    /*
        0x1  := 000001
        0x20 := 100000
        0x1E := 011110
    */
    uint64_t f = 0x0ULL, c = 0x0ULL, output =0x0ULL;
    input = reverse(input, 6);
    f = (input & 0x1ULL) | ((input & 0x20ULL) >> 4);
    c = (input & 0x1EULL) >> 1;
    output |= S_BOXES[s][f][c] & 0xFULL;
    output = reverse(output, 4);
    return output;
}

uint64_t feistel(uint64_t right, uint64_t subkey)
{
    /*
        0x3F := primeros 6 bits a 1, resto 0
    */
    uint64_t input = 0x0ULL, output = 0x0ULL;
    input = permute(right, E, BITS_IN_E);
    input ^= subkey;
    for (int i = 0; i < NUM_S_BOXES; i++)
        output |= s_box( (input >> 6*i) & 0x3FULL , i ) << 4*i;
    output = permute(output, P, BITS_IN_P);
    return output;
}

void swap(uint64_t *left, uint64_t *right)
{
    uint64_t foo;
    foo = *right;
    *right = *left;
    *left = foo;
}

void des(uint64_t subkeys[])
{
    /*
        0xFFFFFFFF := primeros 32 bits a 1, resto 0
    */
    uint64_t left, right;
    block = reverse(block, 64);
    block = permute(block, IP, BITS_IN_IP);
    left = block & 0xFFFFFFFFULL;
    right = (block >> 32) & 0xFFFFFFFFULL;
    for (int i = 0; i < ROUNDS; i++)
    {
        left ^= feistel(right, subkeys[i]);
        swap(&left, &right);
    }
    swap(&left, &right);
    block = (right << 32) | left;
    block = permute(block, IP_INV, BITS_IN_IP);
    block = reverse(block, 64);
}

void cbc_enc()
{
    int i = 0;
    uint64_t x, y, z;
    generate_subkeys();
    while (get_block() != 0) {
        x = block;
        block ^= cbc_iv;
        y = block;
        des(enc_subkeys);
        z = block;
        put_block();
        cbc_iv = block;

        if (verbose == 1)
        {
            printf("-------- BLOQUE %d\n", ++i);
            printf("BLOQUE PLANO:\t\t");
            print_hex(x);
            printf("BLOQUE + IV:\t\t");
            print_hex(y);
            printf("BLOQUE CIFRADO:\t\t");
            print_hex(z);
        }
    }
}

void cbc_dec()
{
    int i = 0;
    uint64_t x, y, z;
    uint64_t foo;
    generate_subkeys();
    while (get_block() != 0) {
        foo = block;
        x = block;
        des(dec_subkeys);
        y = block;
        block ^= cbc_iv;
        z = block;
        put_block();
        cbc_iv = foo;

        if (verbose == 1)
        {
            printf("-------- BLOQUE %d\n", ++i);
            printf("BLOQUE CIFRADO:\t\t\t");
            print_hex(x);
            printf("BLOQUE CIF. DESCIF.:\t\t");
            print_hex(y);
            printf("BLOQUE ANTES + IV:\t\t");
            print_hex(z);
        }
    }
}

/*
    PARTE 2: Funciones para el triple DES
*/

uint64_t tdea_keys[3];
uint64_t tdea_cbc_iv;


void set_key(uint64_t key_, int i)
{
    tdea_keys[i] = key;
}

void set_cbc_iv(uint64_t cbc_iv_)
{
    tdea_cbc_iv = cbc_iv_;
}


void tdea_cbc_enc()
{
    int i = 0;
    uint64_t x, y, z;
    generate_subkeys();
    while (get_block() != 0) {
        x = block;
        block ^= cbc_iv;
        y = block;
        des(enc_subkeys);
        z = block;
        put_block();
        cbc_iv = block;

        if (verbose == 1)
        {
            printf("-------- BLOQUE %d\n", ++i);
            printf("BLOQUE PLANO:\t\t");
            print_hex(x);
            printf("BLOQUE + IV:\t\t");
            print_hex(y);
            printf("BLOQUE CIFRADO:\t\t");
            print_hex(z);
        }
    }
}

void tdea_cbc_dec()
{
    int i = 0;
    uint64_t x, y, z;
    uint64_t foo;
    generate_subkeys();
    while (get_block() != 0) {
        foo = block;
        des(dec_subkeys);
        block ^= cbc_iv;
        put_block();
        cbc_iv = foo;

    }
}

int main (int argc, char *argv[])
{
    /* Parsea, carga e imprime los argumentos */
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    if (mode == ENC)
        cbc_enc();
    else /* if (mode == DEC) */
        cbc_dec();
    clean();

    return OK;
}
