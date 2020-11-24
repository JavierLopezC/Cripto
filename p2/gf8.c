#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "gf8.h"

#define DIRECT 0
#define INVERSE 1

#define OK 0
#define ERR -1

struct div gf_div(int a, int b){
	struct div result;
	int a_aux, b_aux, count_size=0, i, cmp;
	result.cociente = 0;
	result.resto = a;
	if(a < b){
		return result;
	}

	while(1){
		for(i=0; i < 32; i++){
			cmp = (int)pow(2, i);
			if(cmp > result.resto){
				break;
			}
		}
		if(cmp <= b){
			result.cociente *= 2;
			if(count_size == 0){
				return result;
			}
			a_aux = (a % (int)pow(2, (double)count_size));
			a_aux -= (a_aux % (int)pow(2, (double)count_size - 1));
			if(a_aux == 0){
				result.resto = result.resto * 2;
			}else{
				result.resto = result.resto * 2 + 1;
			}
			count_size--;
			continue;
		}
		result.cociente = result.cociente * 2 + 1;
		a_aux = result.resto;
		for(i=0; i < 32; i++){
			cmp = (int)pow(2, i);
			if(cmp > b){
				break;
			}
		}
		while(cmp <= a_aux){
			a_aux = floor((double)a_aux/2);
			count_size++;
		}

		result.resto = a_aux ^ b;
		if(count_size == 0){
			return result;
		}
		a_aux = (a % (int)pow(2, (double)count_size));
		a_aux -= (a_aux % (int)pow(2, (double)count_size - 1));
		if(a_aux == 0){
			result.resto = result.resto * 2;
		}else{
			result.resto = result.resto * 2 + 1;
		}
		count_size--;
	}
}

int xtime(int a){
	int aux, res;
	aux = (a % (int)pow(2, 8));
	aux -= (aux % (int)pow(2, (double)7));
	if(aux == 0){
		 res = a * 2;
		 return res;
	}else{
		res = (a * 2) ^ 283;
		return res;
	}
}

int *descomp_gf(int a){
	int i, cmp, *res, aux;
	res = (int*)malloc(8 * sizeof(int));
	for(i=0; i < 8; i++){
		if(i==0){
			res[i] = a % 2;
			continue;
		}
		aux = (a % (int)pow(2, i + 1));
		aux -= (aux % (int)pow(2, (double)(i)));
		if(aux == 0){
			res[i] = 0;
		}else{
			res[i] = 1;
		}
	}
	return res;
}

int recomp_gf(int* x){
	int res=0, i;
	for(i=0; i<8; i++){
		if(x[i] == 1){
			res += (int)pow(2, i);
		}
	}
	free(x);
	return res;
}


int gf_mul(int a, int b){
	int i, *desc, res = 0, ret = 0;;
	desc = descomp_gf(b);
	if(desc[0] == 1){
		ret = a;
	}
	res = a;
	for(i=1;i<8;i++){
		res = xtime(res);
		if(desc[i] == 1){
			ret = ret ^ res;
		}
	}
	free(desc);
	return ret;
}

void ext_euclid_gf(struct ext_ret *ret, int a, int b){
    int old_r, r, old_s, s, old_t, t, q, aux;
    struct div res;

    old_r = a;
    r = b;
    old_s = 1;
    s = 0;
    old_t = 0;
    t = 1;
    while(r != 0){
        res = gf_div(old_r, r);
        q = res.cociente;
        old_r = r;
        r = res.resto;
        aux = s;
        s = old_s ^ gf_mul(q, s);
        old_s = aux;
        aux = t;
        t = old_t ^ gf_mul(q, t);
        old_t = aux;
    }
    ret->gcd = old_r;
    ret->bez_a = old_s;
    ret->bez_b = old_t;
}

int euclid_gf(int a, int b){
    struct ext_ret ret;
    ext_euclid_gf(&ret, a, b);
    return ret.gcd;
}

int inv_gf8(int x){
	struct ext_ret ret;
    ext_euclid_gf(&ret, 283, x);
    return ret.bez_b;
}	