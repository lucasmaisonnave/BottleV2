#ifndef LIGNE_INCLUDE_H
#define LIGNE_INCLUDE_H
#include "blockchain.h"
/* module ligne: lecture et ecriture de lignes de texte dans un fichier
*/

/*
  Taille maximale d'une ligne de texte
*/
#define LIGNE_MAX 160

/*
  Marqueur de fin de fichier
*/
#define LIGNE_EOF 0

/*
  lit une ligne de texte depuis le fichier de descripteur fd : lit des
  caracteres jusqu'a rencontrer le caractere end et les stocke dans le buffer
  indique
  retourne le nombre de caracteres lus, ou 0 (fin de fichier), ou -1 (erreur)
*/
int lireLigne (int fd, char *buffer, char end) ;

/*
  lit une ligne de texte depuis lea chaine: lit des
  caracteres jusqu'a rencontrer le caractere end et les stocke dans le buffer
  indique
  retourne le nombre de caracteres lus, ou 0 (fin de fichier), ou -1 (erreur)
*/
int lireLigne2(char* chaine, char *buffer, char end);
/*
  ecrit une ligne de texte dans le fichier de descripteur fd : ecrit la chaine
  de caracteres contenue dans le buffer indique et ajoute le caractere end
  retourne le nombre de caracteres ecrits ou -1 (erreur)
*/
int ecrireLigne (int fd, char *buffer, char end) ;


/*
  Envoie la blockchain dans le canal fdsocket
  retourne -1 s'il y a une erreur
*/
int sendBlockchain(int fdsocket);

/*
  met à jour la blockchain lisant le contenu de fdsocket
*/
int getBlockChain(int fdsocket);

/*
  Transforme le tableau des features (string) en block à ajouter dans la
  blockchain
*/
void stringToBlock(char tabfeatures[7][BLOCK_STR_SIZE], struct bloc* current);

/*
  Envoie le tableau des ID dans fsocket
*/
int sendTabID(int fdsocket);

/*
  met à jour le Tableau des identifiants en lisant le contenu de fdsocket
*/
int getTabID(int fdsocket);

#endif

