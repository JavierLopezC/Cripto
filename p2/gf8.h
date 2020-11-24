#ifndef GF8_H
#define GF8_H

struct div{
    int cociente;
    int resto;
};

struct ext_ret{
    int bez_a;
    int bez_b;
    int gcd;
};

struct div gf_div(int a, int b);

int xtime(int a);

int *descomp_gf(int a);

int recomp_gf(int* x);

int gf_mul(int a, int b);

void ext_euclid_gf(struct ext_ret *ret, int a, int b);

int euclid_gf(int a, int b);

int inv_gf8(int x);
	

#endif
