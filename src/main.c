#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compte.h"
#include "sdl.h"

int main(void)
{
    Genesis = (struct genesis*)malloc(sizeof(struct genesis));
    initTabID(&TabID);
    initGenesis();
    init_global(&Bottle);
    
    Etat = ETAT_MENU;

    LoadTabIDFromFile(&TabID, FileNameID);
    LoadBlockChainFromFile1(FileNameBC);

    Message *mes = (Message*)malloc(sizeof(Message));
    donnee *message = (donnee*)malloc(sizeof(donnee));
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;
    char exp[MAX_WORD_LENGHT];
    char dest[MAX_WORD_LENGHT] = "";
    char texte[MaxMessage]; //Message à ajouter dans la blockchain
    char Time[30];
    
    /*-------Threads--------*/

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
                    Input.pressedX = event.button.x;
                    Input.pressedY = event.button.y;
                    break;
                case SDL_KEYDOWN:
                    Input.BouttonClavier = event.key.keysym.sym;
                    break;

            }
        }
    }
    
    TTF_Quit();
    SDL_Quit();

    return 0;
}
