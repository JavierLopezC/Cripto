/*
    Programa que comprueba la no linealidad de las S-Boxes de DES

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

#define V_      0

#define NARGS   1

#define OB      -1
#define OP      0

#define ENC     0
#define DEC     1

#define ERR     -1
#define OK      0

#define MASK(k) (0xFFFFFFFFFFFFFFFF >> (64 - k))

int args[NARGS] = {OP};     /* OB es para args. obligatorios */

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
    /* Argumentos opcionales */
    verbose = 0;
    if (args[V_] != OP)
        verbose = 1;
    return OK;
}

int print_args(char *argv[]) {
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
void print_bin(uint64_t b, int n)
{
    for ( int i = 0; i < n; i++)
        printf("%lld", (b >> (n - 1 - i)) & 0x1ULL);
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
    Genera las subclaves de DES, suponiendo que la clave está guardad en key
*/
void generate_subkeys()
{
    key = permute(key, PC1, BITS_IN_PC1, 64);
    for (int i = 0; i < ROUNDS; i++)
    {
        rotate_halves(ROUND_SHIFTS[i]);
        enc_subkeys[i] = permute(key, PC2, BITS_IN_PC2, 56);
        dec_subkeys[ROUNDS - 1 - i] = enc_subkeys[i];
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
void des(uint64_t subkeys[])
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
        left ^= feistel(right, subkeys[i]);
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
        des(enc_subkeys);
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
    Calcula un indice de no-linealidad para cada S-Box e imprime los resultados
    por la salida estandar.
*/
int main (int argc, char *argv[])
{
    /*
        0x3f := 00111111 6 bits a 1, resto 0
    */
    double nl[NUM_S_BOXES];

    uint64_t Sxy, Sx, Sy;

    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    for (int s = 0; s < NUM_S_BOXES; s++)
    {
        nl[s] = 0;
        if (verbose == 1)
            printf("-------- S-BOX %d\n", s+1);
        for (uint64_t x = 0x0ULL; x <= 0x3FULL; x++)
        {
            for (uint64_t y = 0x0ULL; y <= 0x3fULL; y++)
            {
                Sxy = s_box(x^y, s);
                Sx = s_box(x, s);
                Sy = s_box(y, s);
                if ( Sxy != Sx^Sy )
                    nl[s]++;
                else
                {
                    if (verbose == 1)
                    {
                        print_bin(Sxy, 4);
                        printf(" = S_%d(", s+1);
                        print_bin(x, 6);
                        printf("+");
                        print_bin(y, 6);
                        printf(") = S_%d(", s+1);
                        print_bin(x, 6);
                        printf(")+S_%d(", s+1);
                        print_bin(y, 6);
                        printf(") = ");
                        print_bin(Sx^Sy, 4);
                        printf("\n");
                    }
                }
            }
        }
    }
    printf("-------- Resultados\n");
    printf("-------- Tabla con indice de no-linealidad de cada S-Box:\n");
    int total = 1 << 12;
    for (int i = 0; i < NUM_S_BOXES; i++)
    printf("S-BOX %d\t", i+1);
    printf("\n");
    for (int i = 0; i < NUM_S_BOXES; i++)
    printf("%.4lf\t", nl[i]/total);
    printf("\n");
    double nl_total = 0;
    for (int i = 0; i < NUM_S_BOXES; i++)
    nl_total += nl[i];
    total = total*NUM_S_BOXES;
    printf("-------- Indice de no-linealidad total:\t\t%.4lf\n", nl_total/total);
}
