//Permet de gérer la création et la gestion des comptes

/*-------------------------------------------------------

1/  -Créer nouveau Compte
    -Se connecter
2/ Une fois connecter:
    -Choix de l'utilisateur avec qui connecter
3/ Affichage de de la conversation et possibilité d'envoie de messages

---------------------------------------------------------*/

#include <stdbool.h>
#include "blockchain.h"

#ifndef COMPTE_H

    #define COMPTE_H
    

    

    #define NbID 5            // Nombre maximal d'ID 
    #define FileNameID "SaveID.txt"
    

    struct Identifiant
    {
        char username[MAX_WORD_LENGHT];
        char password[HASH_HEX_SIZE];
    };

    typedef struct 
    {
        int taille;
        struct Identifiant* ID;
    }TABID;

    char expediteur[MAX_WORD_LENGHT];
    char destinataire[MAX_WORD_LENGHT];
    TABID TabID;
    //void menu(HashTable *hashTab);
    void LoadTabIDFromFile(TABID* TabID, const char* TabIDFileName);
    void insertElementToTabID(TABID* TabID, struct Identifiant* Element);
    void initTabID(TABID *TabID);
    bool checkExistenceElementInTabID(TABID* TabID, struct Identifiant* Element);
    void printTabID(void);
    void SaveTabID(TABID* TabID, const char* TabIDFileName);
    void SignUp(TABID* TabID, struct Identifiant* Element);  //Créer un nouveau compte, renvoie true si la création de compte s'est bien passé
    char *CryptPassword(struct Identifiant *Element, char *output);

#endif