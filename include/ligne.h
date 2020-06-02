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
  Désigne la fin d'un mot : Bloc0 | Bloc1
*/
#define END '|'

/*
  Désigne la séparation entre les caractéristiques
  du mot : Username~Password
*/
#define SEPARATEUR '~'

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
void sendBlockchain(int fdsocket);

/*
  met à jour la blockchain lisant le contenu de fdsocket
*/
void getBlockChain(int fdsocket);

/*
  Transforme blocString (string) en block qui est ensuite ajouter à la
  blockchain
*/
void stringToBlock(char blocString[BLOCK_STR_SIZE]);

/*
  Transforme IDstring (string) en ID qui est ensuite ajouter à la
  TabID
*/
void stringToID(char IDstring[LIGNE_MAX]);


/*
  Envoie le tableau des ID dans fsocket
*/
void sendTabID(int fdsocket);

/*
  met à jour le Tableau des identifiants en lisant le contenu de fdsocket
*/
void getTabID(int fdsocket);

void refreshBC(int fd);

void refreshTabID(int fd);

#endif

