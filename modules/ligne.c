/* Module de gestion de lignes de texte
   Voir documentation dans "ligne.h"
   (c) P Lalevée */

#include "pse.h"

int lireLigne(int fd, char *buffer, char end) {
  ssize_t nbLus;
  int i;
  char car;

  if (buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  i = 0;
  while (i < LIGNE_MAX - 1) {
    nbLus = read(fd, &car, 1);   
    if (nbLus == -1) {
      return -1;
    }      
    else if (nbLus == 0) {
      buffer[i] = '\0';
      return LIGNE_EOF;
    }
    else if (car == end) {  //car == '\n'|| 
      buffer[i] = '\0';
      i++;
      return i;
    }
    buffer[i] = car;
    i++;      
  }
  
  buffer[i] = '\0';
  return LIGNE_MAX;
}


int lireLigne2(char* chaine, char *buffer, char end)
{
  ssize_t taille = strlen(chaine);
  char car;
  int i;

  if (buffer == NULL || taille == 0) {
    errno = EINVAL;
    return -1;
  }
  
  for (i = 0; i < taille; i++) {
    car = chaine[i];
    if (car == end) {  
      buffer[i] = '\0';
      i++;
      return i;
    }
    buffer[i] = car;    
  }
  
  buffer[i] = '\0';
  return LIGNE_MAX;
}

int ecrireLigne(int fd, char *buffer, char end) {
  char *position;
  int taille, ecr, nbecr;

  position = strchr(buffer, end);
  if (position == NULL) {
    taille = strlen(buffer);
    if (taille >= LIGNE_MAX - 1) {
      errno = EINVAL;
      return -1;
    }
    buffer[taille++] = end;
    buffer[taille] = '\0';
  }
  else {
    taille = position - buffer + 1;
  }

  nbecr = taille;

  while (taille > 0) {
    ecr = write(fd, buffer, taille);
    if (ecr == -1) {
      return -1;
    }
    taille -= ecr;
  }
  return nbecr;
}

int sendBlockchain(int fdsocket)
{
  char ligne[LIGNE_MAX];
  char end = '|';
  int ret = 0;
  struct bloc* current = Genesis->premier;
  while(current != NULL && ret != -1)
  {
    toString(current, ligne); // à modifier
    ret = ecrireLigne(fdsocket, ligne, end);
  }
  return ret;
}

int getBlockChain(int fdsocket)
{
  char blocString[BLOCK_STR_SIZE];
  int ret = 0;
  char end = '|';
  char separateur = '~';
  char tabfeature[8][LIGNE_MAX];  //Tab de char qui va contenir les donées du bloc
  char buffer[LIGNE_MAX];
  int decalage;
  int i = 0;
  while(lireLigne(fdsocket, blocString, end)>0)
  {
    decalage = 0;
    i = 0;
    while(lireLigne2(blocString + decalage, buffer, separateur)>0)
    {
      strcpy(tabfeature[i], buffer);
      decalage = strlen(buffer)+1;  //On supprime le début de blocString
      i++;
    }
    
  }

}


void stringToBlock(char tabfeatures[7][BLOCK_STR_SIZE]) 
{
  struct bloc current;
  strcpy(current.precHash, tabfeatures[2]);
  current.index = strtol(tabfeatures[0], NULL, 10);
  current.nonce = strtol(tabfeatures[1], NULL, 10);
  
  strcpy(current.donnee->date, tabfeatures[3]);
  strcpy(current.donnee->dest, tabfeatures[4]);
  strcpy(current.donnee->exp, tabfeatures[5]);
  strcpy(current.donnee->message, tabfeatures[6]);

}
