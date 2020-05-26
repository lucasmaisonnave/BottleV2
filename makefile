all: bottle

bottle: blockchain.o main.o compte.o sdl.o
	gcc -o bottle blockchain.o main.o compte.o sdl.o -lssl -lcrypto -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

blockchain.o: src/blockchain.c
	gcc -o blockchain.o -c src/blockchain.c -W -Wall -std=c99 -lssl -lcrypto -lSDL2main -lSDL2

compte.o: src/compte.c src/blockchain.h
	gcc -o compte.o -c src/compte.c -W -Wall -std=c99 

sdl.o: src/sdl.c src/compte.h
	gcc -o sdl.o -c src/sdl.c -W -Wall -std=c99 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

main.o: src/main.c src/blockchain.h src/compte.h src/sdl.h
	gcc -o main.o -c src/main.c -W -Wall -std=c99


clean:
	rm -rf *.o

mrproper: clean
	rm -rf blockchain

run : all
	./bottle