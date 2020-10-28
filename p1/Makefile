CC = gcc
CFLAGS = -g -Wall
GMP_FLAGS = -lgmp
EJS = afin afin_var vigenere stream permutacion opcional
######################################################################################################################################

all: $(EJS)

euclid.o: euclid.c euclid.h
	$(CC) $(CFLAGS) -c euclid.c

afin: afin.c euclid.o
	$(CC) $(CFLAGS) -c afin.c $(GMP_FLAGS) -o afin

afin_var: afin_var.c euclid.o
	$(CC) $(CFLAGS) -c afin_var.c $(GMP_FLAGS) -o afin_var

vigenere: vigenere.c
	$(CC) $(CFLAGS) -c vigenere.c -o vigenere

stream: stream.c
	$(CC) $(CFLAGS) -c stream.c -o stream

permutacion: permutacion.c permutacion.h
	$(CC) $(CFLAGS) -c permutacion.c -o permutacion

opcional: opcional.c permutacion_.c opcional.h permutacion.h
	$(CC) $(CFLAGS) -c opcional.c -o opcional


clean:
	rm -rf *.o tokens.h.gch $(EJS)
