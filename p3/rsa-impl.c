/*
    Programa que implementa el criptosistema RSA

    Autores:
        Mario García Pascual
        Javier López Cano
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "rsa.h"

#define BUF_SIZE 8096

mpz_t n, e, d, block;
int key_bitcnt, block_size;

FILE *in, *out, *fkey = NULL;

/*
    Genera clave RSA, donde los primos tienen n_bits bits y una probabilidad
    de error de 10^-sec
*/
void gen_key(int n_bits, double sec)
{
    mpz_t p, q, p_, q_, phi, foo, gcd;
    mpz_inits(p, q, p_, q_, phi, foo, gcd, NULL);
    init_rstate();
    /* generamos los dos pseudoprimos */
    gen_prime_mr(p, n_bits, pow(10, -sec));
    gen_prime_mr(q, n_bits, pow(10, -sec));
    /* n = p*q */
    mpz_mul(n, p, q);
    /* phi = (p-1)(q-1) */
    mpz_sub_ui(p_, p, 1);
    mpz_sub_ui(q_, q, 1);
    mpz_mul(phi, p_, q_);
    mpz_sub_ui(foo, phi, 2);
    do
    {
        /* 1 <= e aleatorio <= phi-1 */
        gmp_randn(e, foo);
        mpz_add_ui(e, e, 1);
        mpz_gcd(gcd, e, phi);
        /* mientras gcd(e, phi) != 1 ...*/
    }
    while (mpz_cmp_ui(gcd, 1) != 0);
    /* d = e^-1 mod phi */
    mpz_invert(d, e, phi);
    mpz_clears(p, q, p_, q_, phi, foo, gcd, NULL);
    clear_rstate();
    key_bitcnt = 2*n_bits;
}

/*
    Escribe la clave en el fichero fkey con nuestro formato
*/
void put_key()
{
    fprintf(fkey, "---- MODULO -----\n");
    char *foo = mpz_get_str(NULL, 10, n);
    fprintf(fkey, "%s\n", foo);
    free(foo);
    fprintf(fkey, "---- FIN MODULO -----\n");
    fprintf(fkey, "---- EXP. CIFRADO -----\n");
    foo = mpz_get_str(NULL, 10, e);
    fprintf(fkey, "%s\n", foo);
    free(foo);
    fprintf(fkey, "---- FIN EXP. CIFRADO -----\n");
    fprintf(fkey, "---- EXP. DESCIFRADO -----\n");
    foo = mpz_get_str(NULL, 10, d);
    fprintf(fkey, "%s\n", foo);
    free(foo);
    fprintf(fkey, "---- FIN EXP. DESCIFRADO -----\n");
    fprintf(fkey, "---- NUMERO DE BITS -----\n");
    fprintf(fkey, "%d\n", key_bitcnt);
    fprintf(fkey, "---- FIN NUMERO DE BITS -----\n");


}

/*
    Carga la clave almacenada en fkey parseando nuestro formato
*/
void get_key()
{
    char buf[BUF_SIZE];
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    mpz_set_str(n, buf, 10);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    mpz_set_str(e, buf, 10);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    mpz_set_str(d, buf, 10);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    fgets(buf, BUF_SIZE, fkey);
    sscanf(buf, "%d\n", &key_bitcnt);
    fgets(buf, BUF_SIZE, fkey);
}

/*
    Cifra el contenido de block
*/
void rsa_enc()
{
    mpz_powm(block, block, e, n);
}

/*
    Descifra el contenido de block
*/
void rsa_dec()
{
    mpz_powm(block, block, d, n);
}

/*
    RSA {-K -b bits -t sec} [-o fileout]
*/
void key_mode(int argc, char const *argv[])
{
    int n_bits, sec;
    if (argc != 6 && argc != 8)
    {
        printf("Error: argumentos incorrectos\n");
        return;
    }
    mpz_inits(n, e, d, block, NULL);
    sscanf(argv[3], "%d", &n_bits);
    sscanf(argv[5], "%d", &sec);
    if (argc == 8)
        fkey = fopen(argv[7], "w");
    else
        fkey = stdout;
    gen_key(n_bits, sec);
    put_key();
    fclose(fkey);
    mpz_clears(n, e, d, block, NULL);
}

/*
    Lee un bloque del fichero de entrada in y lo almacena en block
*/
int get_block()
{
    uint8_t *buf = (uint8_t *)calloc(block_size, sizeof(uint8_t));
    int ret = fread(buf, 1, block_size, in);
    mpz_import (block, block_size, 1, 1, 0, 0, buf);
    free(buf);
    return (ret == block_size);
}

/*
    Escribe el contenido de block en el fichero de salida out
*/
void put_block()
{
    uint8_t *buf = (uint8_t *)calloc(block_size, sizeof(uint8_t));
    mpz_export(buf, NULL, 1, 1, 1, 0, block);
    fwrite(buf, 1, block_size, out);
    free(buf);
}

/*
    Función que implementa el modo de operación descifrado
*/
void enc_mode(int argc, char const *argv[])
{
    if (argc != 8 && argc != 10)
    {
        printf("Error: argumentos incorrectos\n");
        return;
    }
    mpz_inits(n, e, d, block, NULL);
    if (argc == 8)
    {
        fkey = fopen(argv[3], "r");
        get_key();
        in = fopen(argv[5], "r");
        out = fopen(argv[7], "w");
    }
    else if (argc == 10)
    {
        mpz_set_str(n, argv[3], 10);
        mpz_set_str(e, argv[5], 10);
        in = fopen(argv[7], "r");
        out = fopen(argv[9], "w");
    }

    block_size = key_bitcnt / 8;
    int ret;
    do
    {
        ret = get_block();
        rsa_enc();
        put_block();
    }
    while(ret == 1);

    fclose(fkey);
    fclose(in);
    fclose(out);
    mpz_clears(n, e, d, block, NULL);
}

/*
    Función que implementa el modo de operación de descifrado
*/
void dec_mode(int argc, char const *argv[])
{
    if (argc != 8 && argc != 10)
    {
        printf("Error: argumentos incorrectos\n");
        return;
    }
    mpz_inits(n, e, d, block, NULL);
    if (argc == 8)
    {
        fkey = fopen(argv[3], "r");
        get_key();
        in = fopen(argv[5], "r");
        out = fopen(argv[7], "w");
    }
    else if (argc == 10)
    {
        mpz_set_str(n, argv[3], 10);
        mpz_set_str(d, argv[5], 10);
        in = fopen(argv[7], "r");
        out = fopen(argv[9], "w");
    }

    block_size = key_bitcnt / 8;
    int ret;
    do
    {
        ret = get_block();
        rsa_dec();
        put_block();
    }
    while(ret == 1);

    fclose(fkey);
    fclose(in);
    fclose(out);
    mpz_clears(n, e, d, block, NULL);
}

int main(int argc, char const *argv[]) {

    if (argc < 2)
    {
        printf("Error: número de argumentos incorrectos\n");
        return 2;
    }
    else
    {
        if (strcmp(argv[1], "-K") == 0)
            key_mode(argc, argv);
        else if (strcmp(argv[1], "-C") == 0)
            enc_mode(argc, argv);
        else if (strcmp(argv[1], "-D") == 0)
            dec_mode(argc, argv);
        else
        {
            printf("Error: argumentos incorrectos\n");
            return 2;
        }
    }
    return 0;
}
