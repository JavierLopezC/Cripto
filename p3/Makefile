CC = gcc
CFLAGS = -g
EXTRA_FLAGS = -lgmp -lm
EJS = potencia test_potencia primo break_rsa RSA test_primo
######################################################################################################################################

all: $(EJS)

rsa.o: rsa.c rsa.h
	$(CC) $(CFLAGS) -c rsa.c $(EXTRA_FLAGS)

potencia: potencia.c rsa.h rsa.o
	$(CC) $(CFLAGS) potencia.c -o potencia rsa.o $(EXTRA_FLAGS)

test_potencia: test_potencia.c rsa.h rsa.o
	$(CC) $(CFLAGS) test_potencia.c -o test_potencia rsa.o $(EXTRA_FLAGS)

primo: primo.c rsa.h rsa.o
	$(CC) $(CFLAGS) primo.c -o primo rsa.o $(EXTRA_FLAGS)

break_rsa: break_rsa.c rsa.h rsa.o
	$(CC) $(CFLAGS) break_rsa.c -o break_rsa rsa.o $(EXTRA_FLAGS)

RSA: rsa-impl.c rsa.h rsa.o
	$(CC) $(CFLAGS) rsa-impl.c -o RSA rsa.o $(EXTRA_FLAGS)

test_primo: test_primo.c rsa.h rsa.o
	$(CC) $(CFLAGS) test_primo.c -o test_primo rsa.o $(EXTRA_FLAGS)


clean:
	rm -rf *.o tokens.h.gch $(EJS)
