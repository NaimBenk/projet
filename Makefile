#CFLAGS = -g -Wall -Wextra -pedantic -Wno-unused-parameter
CFLAGS = -g -Wno-unused-parameter
CC = gcc

PROGRAMS = main_1 main_2 main_3 test

.PHONY:	all clean

all: $(PROGRAMS)

test: test_de_la_cle.c
	gcc $(CFLAGS) -o test test_de_la_cle.c

main_1: Chaine.o ChaineMain.o SVGwriter.o Reseau.o
	$(CC) -o ChaineMain $(CFLAGS) Chaine.o ChaineMain.o Reseau.o SVGwriter.o -lm

main_2: Chaine.o ReconstitueReseau.o SVGwriter.o Reseau.o Hachage.o ArbreQuat.o
	$(CC) -o ReconstitueReseau $(CFLAGS) Chaine.o ReconstitueReseau.o Reseau.o Hachage.o ArbreQuat.o SVGwriter.o -lm

main_3: Graphe.o GrapheMain.o Struct_File.o Reseau.o
	$(CC) -o GrapheMain $(CFLAGS) Graphe.o Struct_File.o GrapheMain.o Reseau.o

ChaineMain.o: ChaineMain.c
	gcc $(CFLAGS) -c ChaineMain.c

Chaine.o: Chaine.c Chaine.h 
	gcc $(CFLAGS) -c Chaine.c

SVGwriter.o: SVGwriter.c SVGwriter.h
	gcc $(CFLAGS) -c SVGwriter.c

Reseau.o: Reseau.c Reseau.h 
	gcc $(CFLAGS) -c Reseau.c

Hachage.o: Hachage.c Hachage.h 
	gcc $(CFLAGS) -c Hachage.c

ArbreQuat.o: ArbreQuat.c ArbreQuat.h 
	gcc $(CFLAGS) -c ArbreQuat.c

GrapheMain.o: GrapheMain.c 
	gcc $(CFLAGS) -c GrapheMain.c

Struct_File.o: Struct_File.c Struct_File.h 
	gcc $(CFLAGS) -c Struct_File.c

Graphe.o: Graphe.c Graphe.h
	gcc $(CFLAGS) -c Graphe.c 
	
clean:
	rm -f *.o *~ $(PROGRAMS) ReconstitueReseau ChaineMain GrapheMain
