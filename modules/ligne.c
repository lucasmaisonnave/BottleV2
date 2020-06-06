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

void sendBlockChain(int fdsocket)
{
  char ligne[LIGNE_MAX];
  struct bloc* current = Genesis.premier;
  char *separateur = "~";
  char end = END;
  int ret = 0;
  while(current != NULL && ret != -1)
  {
    toString(current, ligne); // il n'y a pas le hash
    strcat(ligne, current->Hash);
    strcat(ligne,separateur);
    ret = ecrireLigne(fdsocket, ligne, end);
    current = current->lien;
  }
  //On envoie "fin BC|" pour signifié au client que la transmition de la BlockChain est finie
  strcpy(ligne, "fin BC");
  ecrireLigne(fdsocket, ligne, end);
}

int getBlockChain(int fdsocket)
{
  initGenesis();    //On remet à zéro la BC
  char blocString[BLOCK_STR_SIZE];
  char end = END;
  lireLigne(fdsocket, blocString, end);
  while(strcmp(blocString, "fin BC") != 0)
  {
    if(stringToBlock(blocString) == -1)
      return -1;
    lireLigne(fdsocket, blocString, end);
  }
  return 1;
}


int stringToBlock(char blocString[BLOCK_STR_SIZE]) 
{
  char tabfeature[8][LIGNE_MAX];  //Tab de char qui va contenir les donées du bloc
  int decalage = 0;
  char buffer[LIGNE_MAX];
  struct bloc* current = (struct bloc*)malloc(sizeof(struct bloc));
  char separateur = SEPARATEUR;
  for(int i = 0; i<8; i++)
  {
    lireLigne2(blocString + decalage, buffer, separateur);
    strcpy(tabfeature[i], buffer);      
    decalage += strlen(buffer)+1;  //On supprime buffer de blocString
  }  
  struct bloc *nouveau = (struct bloc *)malloc(sizeof(struct bloc));;
  nouveau->donnee = (donnee*)malloc(sizeof(donnee));
  
  nouveau->nonce = (int)strtol(tabfeature[0], NULL, 10);  
  nouveau->index = (int)strtol(tabfeature[1], NULL, 10);  
  strcpy(nouveau->precHash, tabfeature[2]);
  strcpy(nouveau->donnee->date, tabfeature[3]);
  strcpy(nouveau->donnee->dest, tabfeature[4]);
  strcpy(nouveau->donnee->exp, tabfeature[5]);
  strcpy(nouveau->donnee->message, tabfeature[6]);
  strcpy(nouveau->Hash, tabfeature[7]);
  
  nouveau->lien = NULL;
  current = Genesis.premier;
  if(current != NULL)
    while(current->lien != NULL)
          current = current->lien;
  if(IsValidBlock(nouveau, current))
  {
    if(current == NULL)
      Genesis.premier = nouveau;
    else
      current->lien = nouveau;
    Genesis.taille++;
  }
  else
    return -1;
  
  return 1;
}


void sendTabID(int fdsocket)
{
  char ligne[LIGNE_MAX];
  int taille = TabID.taille;
  char *separateur = "~";
  char end = END;
  for(int i = 0; i<taille; i++)
  {
    strcpy(ligne, TabID.ID[i].username);
    strcat(ligne,separateur);
    strcat(ligne, TabID.ID[i].password);
    strcat(ligne,separateur);
    ecrireLigne(fdsocket, ligne, end);
  }
  strcpy(ligne, "fin TabID");
  ecrireLigne(fdsocket, ligne, end);
}

void stringToID(char IDstring[LIGNE_MAX])
{
  int decalage = 0;
  struct Identifiant newID;
  char buffer[LIGNE_MAX];
  char tabfeature[2][LIGNE_MAX];
  char separateur = SEPARATEUR;
  for(int i = 0; i<2; i++)
  {
    lireLigne2(IDstring + decalage, buffer, separateur);
    strcpy(tabfeature[i], buffer);
    decalage += strlen(buffer)+1;  //On supprime buffer de IDstring
  }
  strcpy(newID.username, tabfeature[0]);
  strcpy(newID.password, tabfeature[1]);
  insertElementToTabID(&TabID, &newID);
}


void getTabID(int fdsocket)
{
  initTabID(&TabID); //On remet à zéro le TabID
  char IDstring[LIGNE_MAX];
  lireLigne(fdsocket, IDstring, END);
  while(strcmp(IDstring, "fin TabID") != 0 && IDstring != NULL)
  { 
    
    stringToID(IDstring);
    lireLigne(fdsocket, IDstring, END);
  }
}

void refreshBC(int fd)
{
  ask(fd, ASK_SEND_BC);
  getBlockChain(fd);
}

void refreshTabID(int fd)
{
  ask(fd, ASK_SEND_TABID);
  getTabID(fd);
}

void ask(int canal, char* message)
{
  char buffer[LIGNE_MAX];
  char end = '\n';
  strcpy(buffer, message);
  ecrireLigne(canal,buffer, end);
}