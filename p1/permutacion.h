#ifndef PERMUTACION_H
#define PERMUTACION_H

#define M       4       /* M = número de filas */
#define N       4       /* N = número de columnas */

int set_keys(int k1_[M], int k2_[N]);
void encode(int **in_, int **out_);
void decode(int **in_, int **out_);

#endif
