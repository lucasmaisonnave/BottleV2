all: bottle

bottle: blockchain.o main.o compte.o sdl.o
	gcc -o bottle blockchain.o main.o compte.o sdl.o -lssl -lcrypto -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

blockchain.o: blockchain.c
	gcc -o blockchain.o -c blockchain.c -W -Wall -std=c99 -lssl -lcrypto -lSDL2main -lSDL2

compte.o: compte.c blockchain.h
	gcc -o compte.o -c compte.c -W -Wall -std=c99 

sdl.o: sdl.c compte.h
	gcc -o sdl.o -c sdl.c -W -Wall -std=c99 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

main.o: main.c blockchain.h compte.h sdl.h
	gcc -o main.o -c main.c -W -Wall -std=c99


clean:
	rm -rf *.o

mrproper: clean
	rm -rf blockchain