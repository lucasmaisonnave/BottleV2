# TP7 : Fichier Makefile
#
include ../Makefile.inc

EXE = serveur client

all: ${EXE}

${EXE): ${PSE_LIB}

clean:
	rm -f *.o *~ ${EXE}

runServeur :
	./serveur 2000
runBottle:
	./client localhost 2000
