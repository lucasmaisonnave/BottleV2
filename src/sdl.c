#include <SDL2/SDL.h> //apt-get install libsdl1.2-dev
#include "compte.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdl.h"
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


int Connecter(Data* Bottle, TABID *TabID, int *connect, char* username, int creercompte)
{
    
    int m = 1;
    int x = 0; //Position de la souris
    int y = 0;
    int r = 1;  //Quitter
    int ID = 0;
    int MDP = 0;
    Message *mes = (Message*)malloc(sizeof(Message));
    mes->textRect.x = 760;     //La position du message
    mes->textRect.y = 315;
    mes->textRect.h = 30;    //Choisir la longeur du message
    mes->textRect.w = 25;   //Choisir la largeur du message
    mes->tailleP = 40;
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;
    struct Identifiant* Element = (struct Identifiant*)malloc(sizeof(struct Identifiant));
    strcpy(Element->password, "");
    strcpy(Element->username, "");

    while(m != 0)
    {
        SDL_Event event;
        
        while(SDL_PollEvent(&event) && !ID && !MDP)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    m = 0;
                    r = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    x = event.button.x;
                    y = event.button.y;
                    if(y > 305 && y<355 && x > 765 && x < 1300)     //Identifiant
                    {
                        ID = 1;   
                        mes->textRect.x = 760;     //La position du message
                        mes->textRect.y = 315;                     
                    }
                    else if(x > 765 && x < 1300 && y>425 && y<462)       //Mot de passe
                    {
                        MDP = 1;
                        mes->textRect.x = 760;     //La position du message
                        mes->textRect.y = 425;
                    }
                    else if(y > 523 && y < 555 && x > 1200 && x < 1300) //Retour
                    {
                        m = 0;
                        SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                        SDL_RenderPresent(Bottle->Main_Renderer);
                    }
                    continue;

                case SDL_KEYDOWN: //On test si les ids sont correct
                    if(event.key.keysym.sym == SDLK_RETURN && checkExistenceElementInTabID(TabID, Element)&& !creercompte)  //Se connecter
                    {
                        m = 0;
                        *connect = 1;                                
                    }
                    else if(event.key.keysym.sym == SDLK_RETURN && !checkExistenceElementInTabID(TabID, Element)&& creercompte) //Création de compte
                    {
                        SignUp(TabID, Element);
                        
                        m=0;
                        SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                        SDL_RenderPresent(Bottle->Main_Renderer);
                    }
                    break;
                    

                    
            }
        }

        while(ID && !MDP)   //On a appuyer sur la case Identifiant
        {
            //BarreSaisie(texteID, m,r,ID,MDP,Bottle,mes);     
            while(SDL_PollEvent(&event))
            {                
                switch (event.type)
                {
                    case SDL_KEYDOWN:                        
                        if(event.key.keysym.sym != SDLK_SPACE && event.key.keysym.sym != SDLK_CAPSLOCK && strlen(Element->username) < MAX_WORD_LENGHT && strlen(SDL_GetKeyName(event.key.keysym.sym)) <= 1)
                        {   
                            strcat(Element->username,SDL_GetKeyName(event.key.keysym.sym));
                            strcpy(mes->texte, SDL_GetKeyName(event.key.keysym.sym));
                            mes->textRect.x += 25;
                            Write(Bottle, mes);
                        }
                        
                        else if(event.key.keysym.sym == SDLK_RETURN && checkExistenceElementInTabID(TabID, Element) && !creercompte)
                        {
                            m = 0;
                            ID = 0;
                            *connect = 1;                                
                        }
                        else if(event.key.keysym.sym == SDLK_RETURN && !checkExistenceElementInTabID(TabID, Element)&& creercompte) //Création de compte
                        {
                            SignUp(TabID, Element);
                            m=0;
                            ID=0;
                            SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                            SDL_RenderPresent(Bottle->Main_Renderer);
                        }    
                        break;
                    case SDL_QUIT:
                        m = 0;
                        r = 0;
                        ID = 0;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        x = event.button.x;
                        y = event.button.y;
                        if(x > 600 && x < 1290 && y>425 && y<462)       //Mot de passe
                        {
                            ID = 0;
                            MDP = 1;
                            mes->textRect.x = 760;     //La position du message
                            mes->textRect.y = 425;
                        }
                        else if(y > 523 && y < 555 && x > 1200 && x < 1300) //Retour
                        {
                            m = 0;
                            ID = 0;
                            SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                            SDL_RenderPresent(Bottle->Main_Renderer);
                        }

                        else
                        {
                            ID=0;
                        }
                        break;
                    default:
                        break;
                }
            }       
        }

        while (MDP && !ID)
        {
            while(SDL_PollEvent(&event))
            {                
                switch (event.type)
                {
                    case SDL_KEYDOWN:
                        
                        if(event.key.keysym.sym != SDLK_SPACE && event.key.keysym.sym != SDLK_CAPSLOCK && strlen(Element->password) < MAX_WORD_LENGHT && strlen(SDL_GetKeyName(event.key.keysym.sym)) <= 1)
                        {   
                            strcat(Element->password,SDL_GetKeyName(event.key.keysym.sym));
                            strcpy(mes->texte, SDL_GetKeyName(event.key.keysym.sym));
                            mes->textRect.x += 25;
                            Write(Bottle, mes);
                        }
                        if(event.key.keysym.sym == SDLK_RETURN && checkExistenceElementInTabID(TabID, Element) && !creercompte)
                        {
                            m = 0;
                            MDP = 0;
                            *connect = 1;
                        }
                        else if(event.key.keysym.sym == SDLK_RETURN && !checkExistenceElementInTabID(TabID, Element)&& creercompte) //Création de compte
                        {
                            SignUp(TabID, Element);
                            m=0;
                            MDP=0;
                            SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                            SDL_RenderPresent(Bottle->Main_Renderer);
                        }                  
                        break;
                    case SDL_QUIT:
                        m = 0;
                        r = 0;
                        MDP = 0;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        x = event.button.x;
                        y = event.button.y;
                        if(y > 305 && y<355 && x > 765 && x < 1300)       //Identifiant
                        {
                            ID = 1;
                            MDP = 0;
                            mes->textRect.x = 760;     //La position du message
                            mes->textRect.y = 315;
                        }
                        else if(y > 523 && y < 555 && x > 1200 && x < 1300) //Retour
                        {
                            m = 0;
                            MDP = 0;
                            SDL_RenderCopy(Bottle->Main_Renderer, Bottle->menu_Texture, NULL, NULL);
                            SDL_RenderPresent(Bottle->Main_Renderer);
                        }
                        else
                        {
                            MDP=0;
                        }
                        break;
                    default:
                        break;
                }
            }
        }       
    }
    strcpy(username, Element->username);
    free(mes);
    free(Element);
    return r;
}

int CreerCompte(Data* Bottle, TABID *TabID)
{
    int connect = 0;
    char username[MAX_WORD_LENGHT];
    return Connecter(Bottle, TabID,&connect, username, 1);
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

int BarreSaisie(char* texte, Message* mes, Data* Bottle)
{
    SDL_Event event;
    int start = 1;
    int quit = 0;   //Si on  appui sur la croix
    strcpy(texte, "");
    while(start)
    {
        while(SDL_PollEvent(&event))
        {                
            switch (event.type)
            {
                case SDL_KEYDOWN:
                                    
                    if(event.key.keysym.sym != SDLK_SPACE && event.key.keysym.sym != SDLK_CAPSLOCK && strlen(texte)<=MAX_WORD_LENGHT && strlen(SDL_GetKeyName(event.key.keysym.sym)) <= 1)
                    {   
                        strcat(texte,SDL_GetKeyName(event.key.keysym.sym));
                        strcpy(mes->texte, SDL_GetKeyName(event.key.keysym.sym));
                        mes->textRect.x += 25;
                        Write(Bottle, mes);
                    }
                    if(event.key.keysym.sym == SDLK_RETURN)
                    {
                        start = 0;
                    }
                    break;
                case SDL_QUIT:
                    quit = 1;
                    start = 0;
                    break;
                default:
                    break;
            }
        }
    }
    return quit;
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

int BarreSaisieMessagerie(char* texte, Message* mes, Data* Bottle)
{
    SDL_Event event;
    int start = 1;
    int quit = 0;   //1 Si on  appui sur la croix et 2 si on appuie sur retour
    int x,y;        //Position de la souris
    while(start)
    {
        while(SDL_PollEvent(&event))
        {                
            switch (event.type)
            {
                case SDL_KEYDOWN:
                                    
                    if(event.key.keysym.sym != SDLK_CAPSLOCK && strlen(texte)<=14 && strlen(SDL_GetKeyName(event.key.keysym.sym)) <= 1)
                    {   
                        strcat(texte,SDL_GetKeyName(event.key.keysym.sym));
                        strcpy(mes->texte, SDL_GetKeyName(event.key.keysym.sym));
                        mes->textRect.x += 25;
                        Write(Bottle, mes);
                    }
                    if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        strcat(texte,"_");
                        mes->textRect.x += 25;
                    }
                    if(event.key.keysym.sym == SDLK_RETURN)
                    {
                        start = 0;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    x = event.button.x;
                    y = event.button.y;
                    if(x > 1240 && x < 1340 && y > 730 && y < 760)  //Bouton retour
                    {
                        quit = 2;
                        start = 0;
                    }
                    break;
                case SDL_QUIT:
                    quit = 1;
                    start = 0;
                    break;
                default:
                    break;
            }
        }
    }
    return quit;
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