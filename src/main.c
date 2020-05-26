#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compte.h"
#include "sdl.h"

int main(void)
{
    TABID *TabID = (TABID*)malloc(sizeof(TABID));
    Genesis = (struct genesis*)malloc(sizeof(struct genesis));
    Data *Bottle = (Data*)malloc(sizeof(Data));
    initTabID(TabID);
    initGenesis();
    init_global(Bottle);
    

    LoadTabIDFromFile(TabID, FileNameID);

    LoadBlockChainFromFile1(FileNameBC); //On le fait 2 fois car la blockchain a été codé comme une pile et quand on sauvegarde et charge 1 fois un inverse le sens
    /*SaveBlockChain(FileNameBC);
    initGenesis();
    LoadBlockChainFromFile(FileNameBC);*/
    
    /*-------------------------Affichage d'une texture-------------------------*/
    SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
    SDL_RenderPresent(Bottle->Main_Renderer);
    
    /*-------------------------------------------------------------------------*/  

    int m = 1;
    int x = 0; //Position de la souris
    int y = 0;
    char exp[MAX_WORD_LENGHT];
    char dest[MAX_WORD_LENGHT] = "";
    char texte[MaxMessage]; //Message à ajouter dans la blockchain
    char Time[30];
    int connect = 0;        //Variable pour savoir si on est connecté
    int messagerie = 0;     //Egale à 1 quand on passe à la messagerie
    int choix_dest = 0;     //On passe à la phase choix de destinataire quand c'est égale à 1
    Message *mes = (Message*)malloc(sizeof(Message));
    donnee *message = (donnee*)malloc(sizeof(donnee));
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;
    while(m != 0)
    {

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    m = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    x = event.button.x;
                    y = event.button.y;
                    if(x > 600 && x < 960 && y > 285 && y<335)  //Se connecter
                    {
                        SDL_RenderCopy(Bottle->Main_Renderer, Bottle->connexion_Texture, NULL, NULL);
                        SDL_RenderPresent(Bottle->Main_Renderer);
                        m = Connecter(Bottle, TabID, &connect,exp,0);
                    }
                    if(x > 600 && x < 1290 && y>425 && y<462)   //Créer un compte
                    {
                        SDL_RenderCopy(Bottle->Main_Renderer, Bottle->compte_Texture, NULL, NULL);
                        SDL_RenderPresent(Bottle->Main_Renderer);
                        m = CreerCompte(Bottle, TabID);
                    }
                    continue;
            }
        }
        while(connect && m)
        {
            mes->textRect.x = 100;      //La position du message
            mes->textRect.y = 228;
            mes->textRect.h = 35;       //Choisir la longeur du message
            mes->textRect.w = 25;       //Choisir la largeur du message
            mes->tailleP = 30;
            messagerie = 0;
            choix_dest = 1;
            DisplayUsers(TabID, Bottle);
            while(choix_dest && m)
            {  
                if(BarreSaisie(dest, mes, Bottle))
                {
                    m=0;    //On a appuyé sur la croix
                }
                if(IsValidUsername(dest, TabID)) //ON test si le choix est valide , A CHANGER
                {
                    messagerie = 1;
                }
                choix_dest = 0;
            }
            while(messagerie && m)
            {
                SDL_RenderCopy(Bottle->Main_Renderer, Bottle->messagerie_Texture, NULL, NULL);
                SDL_RenderPresent(Bottle->Main_Renderer);
                DisplayMessagerie(exp, dest, Bottle);
                strcpy(texte,"");
                mes->textRect.x = 30;   //On change la position de la barre de saisie
                mes->textRect.y = 670;
                switch(BarreSaisieMessagerie(texte, mes, Bottle))
                {
                    case 1: //Quitter
                        m=0;
                        break;
                    case 2: //Retour au menu
                        connect = 0;
                        messagerie = 0;
                        SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                        SDL_RenderPresent(Bottle->Main_Renderer);
                        break;
                    case 0: //Envoie du message
                        strcpy(message->exp,exp);
                        strcpy(message->dest, dest);
                        
                        strcpy(message->message, texte);
                        strcpy(message->date, getTime(Time));
                        ajout_block(message);
                        //printAllBlock();

                        /*On save et Load 2 fois car la blockchain a été codé comme une pile et donc save inverse le sens de la pile*/
                        SaveBlockChain(FileNameBC);
                        initGenesis();
                        LoadBlockChainFromFile2(FileNameBC);
                        SaveBlockChain(FileNameBC);
                        initGenesis();
                        LoadBlockChainFromFile2(FileNameBC);
                        break;
                }
                
            }
            

            //connect = 0;
        }
    }   
    
    TTF_Quit();
    SDL_Quit();

    return 0;
}
