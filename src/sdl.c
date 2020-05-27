#include <SDL2/SDL.h> //apt-get install libsdl1.2-dev
#include "compte.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdl.h"
#include "blockchain.h"
#include <SDL2/SDL_ttf.h>   //apt-get install libsdl2-ttf-dev
#include <SDL2/SDL_image.h> //apt-get install libsdl2-image-dev

void init_global(Data* Bottle)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Bottle->Main_Window = SDL_CreateWindow("Bottle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1374, 775, 0);
    Bottle->Main_Renderer = SDL_CreateRenderer(Bottle->Main_Window, -1, SDL_RENDERER_ACCELERATED);

    strcpy(Bottle->image,"images/menu.bmp");
    Bottle->menu_Texture = init_texture(Bottle, Bottle->menu_Texture);
    
    strcpy(Bottle->image,"images/compte.bmp");
    Bottle->compte_Texture = init_texture(Bottle, Bottle->compte_Texture);

    strcpy(Bottle->image,"images/connexion.bmp");
    Bottle->connexion_Texture = init_texture(Bottle, Bottle->connexion_Texture);

    strcpy(Bottle->image,"images/messagerie.bmp");
    Bottle->messagerie_Texture = init_texture(Bottle, Bottle->messagerie_Texture);

    strcpy(Bottle->image,"images/destinataire.bmp");

    Bottle->destinataire_Texture = init_texture(Bottle, Bottle->destinataire_Texture);
    
}


SDL_Texture* init_texture(Data* Bottle, SDL_Texture* texture)
{
    int textureW, textureH;
    Bottle->Loading_Surf = SDL_LoadBMP(Bottle->image);
    texture = SDL_CreateTextureFromSurface(Bottle->Main_Renderer, Bottle->Loading_Surf);
    SDL_QueryTexture(texture, NULL, NULL, &textureW,&textureH);
    SDL_FreeSurface(Bottle->Loading_Surf); /* we got the texture now -> free surface */
    return texture;
}



void Write(Data* Bottle, Message* mes)
{
    TTF_Font *police = TTF_OpenFont("./lsans.ttf",mes->tailleP);
    if(police == NULL)
    {
        printf("%s\n", TTF_GetError());
    }

    mes->texteSurface = TTF_RenderText_Blended(police, mes->texte,mes->couleur);
    if(mes->texteSurface == NULL)
    {
        printf("%s\n", TTF_GetError());
    }
    
    mes->texture = SDL_CreateTextureFromSurface(Bottle->Main_Renderer, mes->texteSurface);
    if(mes->texture == NULL)
    {
        printf("%s", SDL_GetError());
    }
    SDL_QueryTexture(Bottle->message, NULL, NULL, &mes->textRect.w, &mes->textRect.h);
    SDL_FreeSurface(mes->texteSurface);

    SDL_RenderCopy(Bottle->Main_Renderer, mes->texture, NULL, &mes->textRect);
    SDL_RenderPresent(Bottle->Main_Renderer);
}


void SDL_Dest(Data *donnee)//Détruit les texture le rendu et la fenêtre
{
    SDL_DestroyTexture(donnee->menu_Texture);
    SDL_DestroyRenderer(donnee->Main_Renderer);
    //SDL_DestroyWindow(donnee->Main_Window); 
    SDL_RenderPresent(donnee->Main_Renderer);
}

void DisplayUsers(TABID* TabID, Data* Bottle)  //Retourne le choix
{
    int taille = TabID->taille;
    
    SDL_RenderCopy(Bottle->Main_Renderer, Bottle->destinataire_Texture, NULL, NULL);
    SDL_RenderPresent(Bottle->Main_Renderer);
    Message* mes = (Message*)malloc(sizeof(Message));
    mes->textRect.x = 610;      //La position du message
    mes->textRect.y = 335;
    mes->textRect.h = 50;       //Choisir la longeur du message
    mes->tailleP = 60;
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;
    for(int i = 0; i<taille; i++)
    {
        mes->textRect.w = 25*strlen(TabID->ID[i].username);
        mes->textRect.y += mes->textRect.h;
        
        strcpy(mes->texte,TabID->ID[i].username );
        Write(Bottle, mes);
    }
    free(mes);
}

void DisplayMessagerie(char* exp, char* dest, Data* Bottle)
{
    Message* mes = (Message*)malloc(sizeof(Message));
    
    mes->textRect.y = 165;
    mes->textRect.h = 40;    //Choisir la largeur du message
    mes->tailleP = 40;
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;

    mes->textRect.w = 25*strlen(exp);
    mes->textRect.x = 535-mes->textRect.w;     //La position du message    
    strcpy(mes->texte, exp);

    Write(Bottle, mes);


    mes->textRect.w = 25*strlen(dest);
    mes->textRect.x = 820;
    strcpy(mes->texte, dest);
    Write(Bottle, mes);


    mes->textRect.x = 60;
    struct bloc *currentbloc = Genesis->premier;
    int cmpt = 0;               //compteur du nombre de messages affiché et ne doit pas dépasser NbMessages
    mes->textRect.y = 590;
    mes->textRect.h = 25;       //Choisir la largeur du message
    mes->tailleP = 20;
    while(cmpt <= NbMessages && currentbloc != NULL)
    {   
        if((strcmp(currentbloc->donnee->exp, exp) == 0 && strcmp(currentbloc->donnee->dest, dest) == 0) || (strcmp(currentbloc->donnee->dest, exp) == 0 && strcmp(currentbloc->donnee->exp, dest) == 0))    //On test qu'on a bien les bon exp et dest
        {
            cmpt++;
            strcpy(mes->texte, "[");
            strcat(mes->texte, currentbloc->donnee->date);
            strcat(mes->texte, "]");
            strcat(mes->texte, " ");
            strcat(mes->texte, currentbloc->donnee->exp);
            strcat(mes->texte, " : ");
            strcat(mes->texte, currentbloc->donnee->message);
            mes->textRect.w = 12*strlen(mes->texte);
            mes->textRect.y -= mes->textRect.h ; 
            Write(Bottle, mes);
        }
        currentbloc = currentbloc->lien;          
    }
    free(mes);

    
}

bool IsValidUsername(char* username, TABID* TabID)
{
    int taille = TabID->taille;

    for(int i = 0; i<taille; i++)
    {
        if(strcmp(username, TabID->ID[i].username) == 0)
        {
            return true;
        }
    }
    return false;
}

void* MenuThread(void* arg)
{
    while(1)
    {
        while(Etat != ETAT_MENU);
        SDL_RenderCopy(Bottle.Main_Renderer, Bottle.menu_Texture, NULL, NULL);
        SDL_RenderPresent(Bottle.Main_Renderer);        
        while(Etat == ETAT_MENU)
        {
            if(Input.pressedX > 600 && Input.pressedX < 960 && Input.pressedY > 285 && Input.pressedY <335) //Se connecter
            {
                Input.pressedX = 0;
                Input.pressedY = 0;
                Etat = ETAT_CONNECTION;
            }
            if(Input.pressedX > 600 && Input.pressedX < 1290 && Input.pressedY > 425 && Input.pressedY < 462)   //Créer Compte
            {
                Input.pressedX = 0;
                Input.pressedY = 0;
                Etat = ETAT_COMPTE;
            }
        }
    }
}

void* CompteConnecterThread(void* arg)
{

}

void* DestinataireThread(void* arg)
{

}

void* MessagerieThread(void* arg)
{

}

void BarreSaisie()
{

}
