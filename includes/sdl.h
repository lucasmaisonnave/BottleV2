#ifndef SDL_H
    #include <SDL2/SDL.h>
    #include "blockchain.h"
    #include <SDL2/SDL_ttf.h> 
    #include <SDL2/SDL_image.h> 
    #define SDL_H

    #define NbMessages 12      //Nb de messages à afficher
    


    typedef struct {
    SDL_Renderer* Main_Renderer;
    SDL_Window* Main_Window;

    SDL_Texture* menu_Texture;
    SDL_Texture* compte_Texture;
    SDL_Texture* connexion_Texture;
    SDL_Texture* messagerie_Texture;
    SDL_Texture* destinataire_Texture;
    SDL_Texture* message;

    SDL_Surface* Loading_Surf; 
    SDL_Rect SrcR;
    SDL_Rect DestR;
    char image[30];
    int posX;
    int posY;
    }Data;

    typedef struct
    {
        int tailleP; //taille de la police
        SDL_Surface* texteSurface;
        SDL_Color couleur;
        char texte[MaxMessage];
        SDL_Rect textRect;
        SDL_Texture* texture;
    }Message;


    void init_global(Data* Bottle);

    SDL_Texture* init_texture(Data* Bottle, SDL_Texture* texture);
    void SDL_Dest(Data *Bottle);
    int Connecter(Data* Bottle, TABID *TabID,int* connect, char* username, int creercompte);        //Affiche la partie Se connecter et renvoie 0 si l'on a appuyé sur le croix pour quitter
    void Write(Data* Bottle,Message* mes);      //Ecris le message
    int BarreSaisie(char* texte, Message* mes, Data* Bottle);
    int CreerCompte(Data* Bottle, TABID *TabID);
    void DisplayUsers(TABID* TabID, Data* Bottle);
    void DisplayMessagerie(char* exp, char* dest, Data* Bottle);
    int BarreSaisieMessagerie(char* texte, Message* mes, Data* Bottle); //Ici il faut pouvoir accepter les espaces implémenté le bouon retour
    bool IsValidUsername(char* username, TABID* TabID);



#endif