CC = gcc
CFLAGS = -g
GMP_FLAGS = -lgmp
EJS = afin afin_var vigenere stream permutacion opcional ic ic_key
######################################################################################################################################

all: $(EJS)

# euclid.o: euclid.c euclid.h
# 	$(CC) $(CFLAGS) -c euclid.c

afin: afin.c euclid.o
	$(CC) $(CFLAGS) euclid.c afin.c $(GMP_FLAGS) -o afin

afin_var: afin_var.c euclid.o
	$(CC) $(CFLAGS) euclid.c afin_var.c $(GMP_FLAGS) -o afin_var

vigenere: vigenere.c
	$(CC) $(CFLAGS) vigenere.c -o vigenere

ic: ic.c
	$(CC) $(CFLAGS) vigenere.c -o vigenere -lm

ic_key: ic_key.c
	$(CC) $(CFLAGS) ic_key.c -o ic_key -lm

stream: stream.c
	$(CC) $(CFLAGS) stream.c -o stream

permutacion: permutacion.c permutacion.h
	$(CC) $(CFLAGS) permutacion.c -o permutacion

opcional: opcional.c permutacion_.c opcional.h permutacion.h
	$(CC) $(CFLAGS) permutacion_.c opcional.c -o opcional -lm


clean:
	rm -rf *.o tokens.h.gch $(EJS)
