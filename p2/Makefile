CC = gcc
CFLAGS = -g
GMP_FLAGS = -lgmp
MATH_FLAG = -lm
EJS = seg-perf desCBC AES_lineal_check SBOX_AES
######################################################################################################################################

all: $(EJS)

gf8.o: gf8.c gf8.h
	$(CC) $(CFLAGS) -c gf8.c $(MATH_FLAG)

AES_tables.o: AES_tables.c
	$(CC) $(CFLAGS) -c AES_tables.c

seg-perf: seg-perf.c
	$(CC) $(CFLAGS) seg-perf.c $(GMP_FLAGS) $(MATH_FLAG) -o seg-perf

desCBC: desCBC.c desCBC.h
	$(CC) $(CFLAGS) desCBC.c $(GMP_FLAGS) -o desCBC

AES_lineal_check: AES_lineal_check.c AES_tables.o
	$(CC) $(CFLAGS) AES_lineal_check.c -o AES_lineal_check

SBOX_AES: AES_tables_calc.c gf8.o AES_tables.o
	$(CC) $(CFLAGS) gf8.c AES_tables_calc.c $(MATH_FLAG) -o SBOX_AES

clean:
	rm -rf *.o tokens.h.gch $(EJS)
