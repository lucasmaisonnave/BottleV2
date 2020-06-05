#ifndef SDL_H
#define SDL_H
    #include <SDL2/SDL.h>
    #include "blockchain.h"
    #include "compte.h"
    #include <SDL2/SDL_ttf.h> 
    #include <SDL2/SDL_image.h> 
    

    #define NbMessages 12      //Nb de messages à afficher
    
    enum ETAT {ETAT_QUIT, ETAT_MENU, ETAT_CONNECTION, ETAT_COMPTE, ETAT_DESTINATAIRE, ETAT_MESSAGERIE};

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
        char texte[3*MAX_WORD_LENGHT+6];
        SDL_Rect textRect;
        SDL_Texture* texture;
    }_Message;

    typedef struct
    {
        SDL_Point PointPressed;
        SDL_Keycode BouttonClavier;
    }INPUT;


    enum ETAT Etat;
    INPUT Input;

    Data Bottle;
    

    void init_global(Data* Bottle);
    SDL_Texture* init_texture(Data* Bottle, SDL_Texture* texture);
    void SDL_Dest(Data *Bottle);
    void Write(Data* Bottle,_Message* mes);      //Ecris le message
    void DisplayUsers(TABID* TabID, Data* Bottle);
    void DisplayMessagerie(char* exp, char* dest, Data* Bottle);
    bool IsValidUsername(char* username, TABID* TabID);
    void* MenuThread(void* arg);
    void* CompteConnecterThread(void* arg);
    void* DestinataireThread(void* arg);
    void* MessagerieThread(void* arg);
    void BarreSaisie(_Message* mes, char texte[MAX_WORD_LENGHT], SDL_Rect* Barre);
    void DisplayBackground(SDL_Texture* Texture);
    void ResetInput();
    void ResetId(struct Identifiant* Id);
    void ResetMes(_Message* mes, SDL_Rect* Barre);


#endif