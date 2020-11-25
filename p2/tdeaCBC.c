/*
    Implementación de Triple DES con tres claves y modo operación CBC

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
#define K1_     1
#define K2_     2
#define K3_     3
#define IV_     4
#define IN_     5
#define OUT_    6
#define V_      7

#define NARGS   8

#define OB      -1
#define OP      0

#define ENC     0
#define DEC     1

#define ERR     -1
#define OK      0

#define MASK(k) (0xFFFFFFFFFFFFFFFF >> (64 - k))

int args[NARGS] = {OB,OP,OP,OP,OP,OP,OP,OP};     /* OB es para args. obligatorios */

int mode;
FILE *in;
FILE *out;
int verbose;

uint64_t key;
uint64_t keys[3];
uint64_t cbc_iv;
uint64_t block;
uint64_t enc_subkeys[3][ROUNDS] = {0};
uint64_t dec_subkeys[3][ROUNDS] = {0};


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
        else if (strcmp(argv[i], "-k1") == 0)
            args[K1_] = ++i;
        else if (strcmp(argv[i], "-k2") == 0)
            args[K2_] = ++i;
        else if (strcmp(argv[i], "-k3") == 0)
            args[K3_] = ++i;
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
    if (args[K1_] != OP)
        keys[0] = (uint64_t) strtoull(argv[args[K1_]], NULL, 16);
    if (args[K2_] != OP)
        keys[1] = (uint64_t) strtoull(argv[args[K2_]], NULL, 16);
    if (args[K3_] != OP)
        keys[2] = (uint64_t) strtoull(argv[args[K3_]], NULL, 16);
    if (args[IV_] != OP)
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
    for (int i = 0; i < 3; i++)
    {
        printf("Clave %d:\t", i+1);
        print_hex(keys[i]);
    }
    printf("IV:\t\t");
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


/*
    Imprime los 64 bits de b, empezando por el más significativo a la izq.
*/
void print_bin(uint64_t b)
{
    for ( int i = 0; i < 64; i++)
        printf("%lld", (b >> (63 - i)) & 0x1ULL);
    printf("\n");
}


/*
    Imprime los 8 bytes de b en hexadecimal
*/
void print_hex(uint64_t b)
{
    printf("%016"PRIx64"\n", b);
}


/*
    Invierte el orden de los primeros n bits de b
*/
uint64_t reverse(uint64_t b, int n)
{
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < n; i++)
        foo |= ((b >> i) & 0x1ULL) << (n - 1 - i);
    return foo;
}

/*
    Invierte el orden de los bytes de b
*/
uint64_t reverse_bytes(uint64_t b)
{
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < 8; i++)
        foo |= ((b >> 8*i) & 0xFF) << 8*(7 - i);
    return foo;
}

/*
    Implementación la rotación de las mitades del key schedule de DES, siendo
    n el numero de bits a rotar. Asume que la clave está cargada en key
*/
void rotate_halves(const short n)
{
    /*
        0xFFFFFFF := primeros 28 bits a 1, resto 0
    */
    uint64_t l = (key >> 28) & 0xFFFFFFFULL;
    uint64_t r = key & 0xFFFFFFFULL;
    l = ((l << n) | (l >> (28 - n))) & 0xFFFFFFFULL;
    r = ((r << n) | (r >> (28 - n))) & 0xFFFFFFFULL;
    key = (l << 28) | r;
}

/*
    Permuta los primeros n bits de b con respecto a la permutacion p; n viene
    a ser el tamaño del resultado de la permutación
*/
uint64_t permute(uint64_t b, const short p[], int n, int m)
{
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < n; i++)
        foo |= ((b >> (m - p[i])) & 0x1ULL) << (n - 1 - i);
    return foo;
}

/*
    Genera las subclaves de DES, suponiendo que las claves está guardad en keys
*/
void generate_subkeys()
{
    for (int j = 0; j < 3; j++)
    {
        keys[j] = permute(keys[j], PC1, BITS_IN_PC1, 64);
        key = keys[j];
        for (int i = 0; i < ROUNDS; i++)
        {
            rotate_halves(ROUND_SHIFTS[i]);
            enc_subkeys[j][i] = permute(key, PC2, BITS_IN_PC2, 56);
            dec_subkeys[j][ROUNDS - 1 - i] = enc_subkeys[j][i];
        }
    }
}

/*
    Devuelve un byte aleatorio
*/
uint64_t rand_byte()
{
    /*
        0xFF := primer byte a 1, resto 0
    */
    return rand() & 0xFFULL;
}

/*
    Devuelve un uint64_t aleatorio mediante llamadas sucesivas a rand_byte()
*/
uint64_t rand_uint64()
{
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < 8; i++)
        foo |= rand_byte() << 8*i;
    return foo;
}

/*
    Dado un byte b, devuelve el byte con el último bit como bit de paridad
    impar
*/
uint64_t set_parity_byte(uint64_t b)
{
    /*
        Inicializamos foo a 1 ya que nos piden bit de paridad impar
    */
    uint64_t foo = 0x1ULL;
    for (int i = 1; i < 8; i++)
        foo ^= (b >> i) & 0x1ULL;
    foo |= (b & ~0x1ULL);
    return foo;
}

/*
    Devuelve b con todos sus bits de paridad establecidos
*/
uint64_t set_parity(uint64_t b)
{
    /*
        0xFF := primer byte a 1, resto 0
    */
    uint64_t foo = 0x0ULL;
    for (int i = 0; i < 8; i++)
        foo |= set_parity_byte( (b >> 8*i) & 0xFF ) << 8*i;
    return foo;
}

/*
    Devuelve 1 si los bits de paridad de b son correctos; 0 en caso contrario
*/
int check_parity(uint64_t b)
{
    return (b == set_parity(b));
}

/*
    Lee un bloque de 64 bits del fichero de entrada. Si se lee un bloque
    completo, todo correcto; si se lee un bloque incompleto, se rellena con
    bytes aleatorios; si no se lee nada, se avisa de que el fichero a terminado
    mediante un retorno de 0

    El bloque leído se almacena en la variable global block
*/
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
    block = reverse_bytes(block);
    return amount;
}

/*
    Escribe en el fichero de salida el bloque almacenado en la variable global
    block
*/
void put_block()
{
    block = reverse_bytes(block);
    fwrite(&block, 1, 8, out);
}

/*
    Devuelve el resultado de la s-esima S-box con valores de entrada input
*/
uint64_t s_box(uint64_t input, int s)
{
    /*
        0x1  := 000001
        0x20 := 100000
        0x1E := 011110
    */
    uint64_t f = 0x0ULL, c = 0x0ULL, output =0x0ULL;
    f = (input & 0x1ULL) | ((input & 0x20ULL) >> 4);
    c = (input & 0x1EULL) >> 1;
    output |= S_BOXES[s][f][c] & 0xFULL;
    return output;
}

/*
    Devuelve el resultado de aplicar la función de feistel F sobre right y
    clave subkey
*/
uint64_t feistel(uint64_t right, uint64_t subkey)
{
    /*
        0x3F := primeros 6 bits a 1, resto 0
    */
    uint64_t input = 0x0ULL, output = 0x0ULL;
    input = permute(right, E, BITS_IN_E, 32);
    input ^= subkey;
    for (int i = 0; i < NUM_S_BOXES; i++)
        output |= s_box( (input >> 6*(NUM_S_BOXES - 1 - i)) & 0x3FULL , i ) << 4*(NUM_S_BOXES - 1 - i);
    output = permute(output, P, BITS_IN_P, 32);
    return output;
}

/*
    Intercambia el contenido de left y right
*/
void swap(uint64_t *left, uint64_t *right)
{
    uint64_t foo;
    foo = *right;
    *right = *left;
    *left = foo;
}

/*
    Aplica el algoritmo de DES sobre la variable global block usando el array
    de subclaves indicado:
        - enc_subkeys, en caso que se quiera cifrar
        - dec_subkeys, en caso de descifrado
    La diferencia entre los dos arrays es el orden de las subclaves
*/
void des(uint64_t subkeys[][ROUNDS], int j)
{
    /*
        0xFFFFFFFF := primeros 32 bits a 1, resto 0
    */
    uint64_t left, right;
    block = permute(block, IP, BITS_IN_IP, 64);
    left = (block >> 32) & 0xFFFFFFFFULL;
    right = block & 0xFFFFFFFFULL;
    for (int i = 0; i < ROUNDS; i++)
    {
        left ^= feistel(right, subkeys[j][i]);
        swap(&left, &right);
    }
    swap(&left, &right);
    block = (left << 32) | right;
    block = permute(block, IP_INV, BITS_IN_IP, 64);
}

/*
    Cifra el contenido del fichero de entrada usando el modo de operacion CBC.
    El resultado se almacena en el fichero de salida.

    Asume que:
        - la clave se encuentra en key
        - el vector de inic. se encuentra en cbc_iv
*/
void cbc_enc()
{
    int i = 0;
    uint64_t x, y, z;
    generate_subkeys();
    while (get_block() != 0) {
        x = block;
        block ^= cbc_iv;
        y = block;
        des(enc_subkeys, 0);
        des(dec_subkeys, 1);
        des(enc_subkeys, 2);
        z = block;
        put_block();
        cbc_iv = reverse_bytes(block);

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

/*
    Descifra el contenido del fichero de entrada usando el modo de operacion
    CBC. El resultado se almacena en el fichero de salida.

    Asume que:
        - la clave se encuentra en key
        - el vector de inic. se encuentra en cbc_iv
*/
void cbc_dec()
{
    int i = 0;
    uint64_t x, y, z;
    uint64_t foo;
    generate_subkeys();
    while (get_block() != 0) {
        foo = block;
        x = block;
        des(dec_subkeys, 2);
        des(enc_subkeys, 1);
        des(dec_subkeys, 0);
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
    Programa principal. Si se pide cifrar:
        - genera clave con bits de paridad
        - genera vector de inicializacion
        - cifra in y escribe en out
        - imprime por pantalla la clave y iv
    En caso que se pida descifrar:
        - comprueba que la clave tiene los bits
            de paridad correctos, devolviendo error
            en caso contrario
        - descifrar in y escribe en out
*/
int main (int argc, char *argv[])
{
    uint64_t keys_[3], cbc_iv_;
    /* Parsea, carga e imprime los argumentos */
    srand(time(NULL));
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    if (mode == ENC)
    {
        for (int i = 0; i < 3; i++)
        {
            keys[i] = set_parity(rand_uint64());
            keys_[i] = keys[i];
        }
        cbc_iv = rand_uint64();
        cbc_iv_ = cbc_iv;
        cbc_enc();
        printf("--------\n");
        printf("GENERATED KEYS:\n");
        for (int i = 0; i < 3; i++)
        {
            printf("\t\t\t");
            print_hex(keys_[i]);
        }
        printf("\nFOR SLL:\t\t");
        for (int i = 0; i < 3; i++)
            printf("%016"PRIx64"", keys_[i]);
        printf("\n\nGENERATED IV:\t\t");
        print_hex(cbc_iv_);
    }
    else /* if (mode == DEC) */
    {
        for (int i = 0; i < 3; i++)
        {
            if (check_parity(keys[i]) == 0)
            {
                printf("Error: bits de paridad incorrectos\n");
                return ERR;
            }
        }
        cbc_dec();
    }

    clean();

    return OK;
}
