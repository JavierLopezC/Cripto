/*
    Contiene el código para calcular la clave de vigenere sabiendo la longitud
    de clave y el idioma

    Autores:
        Mario García Pascual
        Javier López Cano
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define L_      0
#define IN_     1
#define OUT_    2
#define LAN_    3

#define NARGS   4

#define OB      -1
#define OP      0

#define ERR     -1
#define OK      0

#define ALPH_SIZE       26
#define IS_IN_ALPH(X)   ((X) >= 'A' && (X) <= 'Z')

#define SPA         0
#define ENG         1

#define IC_SPA      0.078
#define IC_ENG      0.065
#define IC_RAN      0.038
#define THOLD       0.005

#define MAX_BUF     256

int args[6] = {OB,OP,OP,OB};     /* OB es para args. obligatorios */

int lan;
int key_len;
FILE *in, *out;

double *lan_freqs[2] = {0};
const double lan_ics[2] = { IC_SPA, IC_ENG };

char *derived_key = NULL;

double **tmp_ics = NULL;

int **freqs = NULL;
double *totals = NULL;
double *ics = NULL;

/* ic_key {-ENG | -SPA} {-l longitud n-grama a probar}  [-i file_in] [-o file_out] */

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
        else if (strcmp(argv[i], "-l") == 0)
        {
            args[L_] = ++i;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            args[IN_] = ++i;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            args[OUT_] = ++i;
        }
        else if (strcmp(argv[i], "-ENG") == 0)
        {
            args[LAN_] = i;
        }
        else if (strcmp(argv[i], "-SPA") == 0)
        {
            args[LAN_] = i;
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
    key_len = atoi(argv[args[L_]]);
    if (strcmp(argv[args[LAN_]], "-ENG") == 0)
        lan = ENG;
    else
        lan = SPA;
    /* Argumentos opcionales */
    if (args[IN_] == OP)
    {
        in = stdin;
    }
    else
    {
        in = fopen(argv[args[IN_]], "r");
    }
    if (args[OUT_] == OP)
    {
        out = stdout;
    }
    else
    {
        out = fopen(argv[args[OUT_]], "w");
    }
    return OK;
}

int print_args(char *argv[]) {
    printf("Idioma hipótesis a probar: %d\n", lan);
    printf("Tamaño clave a probar: %d\n", key_len);
    if (args[IN_] == OP)
    {
        printf("stdin\n");
    }
    else
    {
        printf("%s\n", argv[args[IN_]]);
    }
    if (args[OUT_] == OP)
    {
        printf("stdout\n");
    }
    else
    {
        printf("%s\n", argv[args[OUT_]]);
    }
}

void clean()
{
    fflush(in);
    fclose(in);
    fflush(out);
    fclose(out);
}

/*
    PARTE 1: Funciones necesarias para hallar la clave mediante criptoanálisis
*/

/*
    Carga las frecuencias de las letras ENG/SPA del fichero freqs.txt
*/
void load_freqs()
{
    char buf[MAX_BUF];
    FILE *fp = NULL;
    int i;

    lan_freqs[0] = (double *)malloc(sizeof(double) * ALPH_SIZE);
    lan_freqs[1] = (double *)malloc(sizeof(double) * ALPH_SIZE);

    fp = fopen("freqs.txt", "r");
    for (i = 0; i < ALPH_SIZE; i++)
    {
        fgets(buf, MAX_BUF, fp);
        sscanf(buf, "%lf\t%lf\n", &lan_freqs[0][i], &lan_freqs[1][i]);
        lan_freqs[0][i] /= 100;
        lan_freqs[1][i] /= 100;
    }
    fclose(fp);
}

/*
    Libera los arrays de frecuencias de letras ENG/SPA
*/
void unload_freqs()
{
    free(lan_freqs[0]);
    free(lan_freqs[1]);
}

/*
    Lee el fichero de entrada y almacena las frecuencias de cada caracter en
    cada subtexto
*/
void calc_freqs()
{
    int i, c;
    while ((c = fgetc(in)) != EOF && IS_IN_ALPH(c))
    {
        freqs[ i % key_len ][ c - 'A' ]++;
        totals[ i % key_len ]++;
        i++;
    }
}

/*
    Con los datos de las frecuencias calcula el IC para cada subtexto
*/
void calc_ics()
{
    int i, j;
    for (i = 0; i < key_len; i++)
    {
        ics[i] = 0;
        for (j = 0; j < ALPH_SIZE; j++)
        {
            ics[i] += pow(freqs[i][j], 2);
        }
        ics[i] /= pow(totals[i], 2);
    }
}

/*
    Obtiene el IC medio de todos los subtextos
*/
double get_avg_ic()
{
    int i;
    double tmp = 0;
    for (i = 0; i < key_len; i++)
        tmp += ics[i];
    return tmp / key_len;
}

/*
    Imprime los resultados del criptoanálisis
*/
void print_ics()
{
    int i;
    double avg;
    fprintf(out, "-------- Resultados\n");
    for (i = 0; i < key_len; i++)
    {
        fprintf(out, "Subtexto %d\t\t IC = %lf\n", i, ics[i]);
    }
    avg = get_avg_ic();
    fprintf(out, "-------- IC medio = %lf\n", avg);
    fprintf(out, "Diferencia con IC inglés:\t%lf\n", fabs(avg - IC_ENG));
    fprintf(out, "Diferencia con IC español:\t%lf\n", fabs(avg - IC_SPA));
    fprintf(out, "Diferencia con IC aleatorio:\t%lf\n", fabs(avg - IC_ENG));
    fprintf(out, "-------- Conclusiones\n");
    if (fabs(avg - IC_ENG) < THOLD) fprintf(out, "¡Ojo! Dif. con IC inglés < %lf\n", THOLD);
    if (fabs(avg - IC_SPA) < THOLD) fprintf(out, "¡Ojo! Dif. con IC español < %lf\n", THOLD);
    if (fabs(avg - IC_RAN) < THOLD) fprintf(out, "¡Ojo! Dif. con IC aleatorio < %lf\n", THOLD);
}

/*
    Para un caracter de la clave, imprime los ICs obtenidos para todos los
    desplazamientos posibles, y finalmente el que alcanza el máximo IC
*/
void print_table(int i, int max)
{
    int j;
    fprintf(out, "-------- Tabla char %d-ésimo de la clave\n", i);
    for (j = 0; j < ALPH_SIZE; j++)
        fprintf(out, "%c\t", j + 'A');
    fprintf(out, "\n");
    for (j = 0; j < ALPH_SIZE; j++)
        fprintf(out, "%.3lf\t", tmp_ics[i][j]);
    fprintf(out, "\n");
    fprintf(out, "-------- máx = %c\n", max + 'A');
}

/*
    Para un caracter de la clave, calcula los ICs obtenidos para todos los
    desplazamientos posibles, así como el máximo
*/
char calc_key_char(int i)
{
    int j, k;
    double max, max_ic = 0;

    for (j = 0; j < ALPH_SIZE; j++)
    {
        tmp_ics[i][j] = 0;
        for (k = 0; k < ALPH_SIZE; k++)
        {
            tmp_ics[i][j] += lan_freqs[lan][k] * freqs[i][ (k + j + ALPH_SIZE) % ALPH_SIZE ];
        }
        tmp_ics[i][j] /= totals[i];
        if (tmp_ics[i][j] > max_ic)
        {
            max_ic = tmp_ics[i][j];
            max = j;
        }
    }
    print_table(i, max);
    return max + 'A';
}

/*
    Reserva toda la memoria necesaria
*/
void alloc_memory()
{
    int i;
    freqs = (int **)malloc(sizeof(int *) * key_len);
    totals = (double *)calloc(key_len, sizeof(double));
    ics = (double *)malloc(sizeof(double) * key_len);
    derived_key = (char *)malloc(sizeof(char) * key_len);
    tmp_ics = (double **)malloc(sizeof(double *) * key_len);
    for (i = 0; i < key_len; i++)
    {
        freqs[i] = (int *)calloc(ALPH_SIZE, sizeof(int));
        tmp_ics[i] = (double *)malloc(sizeof(double) * ALPH_SIZE);
    }
}

/*
    Libera toda la memoria necesaria
*/
void dealloc_memory()
{
    int i;
    for (i = 0; i < key_len; i++)
    {
        free(freqs[i]);
        free(tmp_ics[i]);
    }
    free(freqs);
    free(totals);
    free(ics);
    free(derived_key);
    free(tmp_ics);
}

int main (int argc, char *argv[])
{
    /* Parsea, carga e imprime los argumentos */
    int i;
    if (parse_args(argc, argv) == ERR)
    {
        return ERR;
    }
    load_args(argv);
    print_args(argv);

    load_freqs();
    alloc_memory();
    calc_freqs();

    /* Calcula la clave derivada del criptoanálisis */
    for (i = 0; i < key_len; i++)
        derived_key[i] = calc_key_char(i);

    /* Imprime los resultados: la clave obtenida */
    fprintf(out, "-------- Conclusiones\n");
    fprintf(out, "La clave derivada del criptoanálisis es:\t");
    for (i = 0; i < key_len; i++)
        fprintf(out, "%c", derived_key[i]);
    fprintf(out, "\n");


    dealloc_memory();
    unload_freqs();
    clean();

    return OK;
}
