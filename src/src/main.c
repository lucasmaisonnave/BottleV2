#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compte.h"
#include "sdl.h"
#include <pthread.h>
#include <errno.h>

int main(void)
{
    Genesis = (struct genesis*)malloc(sizeof(struct genesis));
    initTabID(&TabID);
    initGenesis();
    init_global(&Bottle);
    
    Etat = ETAT_MENU;

    LoadTabIDFromFile(&TabID, FileNameID);
    LoadBlockChainFromFile1(FileNameBC);

    
    /*-------Threads--------*/
    pthread_t id[4];
    int ret1 = pthread_create(&id[0], NULL, MenuThread, NULL);
    int ret2 = pthread_create(&id[1], NULL, CompteConnecterThread, NULL);
    int ret3 = pthread_create(&id[2], NULL, DestinataireThread, NULL);
    int ret4 = pthread_create(&id[3], NULL, MessagerieThread, NULL);
    /*----------------------*/    

    while(Etat != ETAT_QUIT)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    Etat = ETAT_QUIT;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    Input.PointPressed.x = event.button.x;
                    Input.PointPressed.y = event.button.y;
                    printf("x = %d, y = %d\n", Input.PointPressed.x,Input.PointPressed.y);
                    break;
                case SDL_KEYDOWN:
                    Input.BouttonClavier = event.key.keysym.sym;
                    break;

            }
        }
    }
    SDL_Dest(&Bottle);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
