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
    toString(current, ligne);
    ret = ecrireLigne(fdsocket, ligne, end);
  }
  return ret;
}

int getBlockChain(int fdsocket)
{
  initGenesis();
  struct bloc* current = Genesis->premier;
  char blocString[BLOCK_STR_SIZE];
  int ret = 0;
  char end = '|';
  char separateur = '~';
  char feature[LIGNE_MAX];
  char buffer[LIGNE_MAX];
  while(lireLigne(fdsocket, blocString, end)>0)
  {
    strcpy(buffer, strchr(blocString, separateur));
    while(buffer != NULL)
    {
      strrcpy(feature, blocString);
      
      strcpy(blocString, buffer+1); //On copie ce qu'il y a après ~

    }
  }

}

