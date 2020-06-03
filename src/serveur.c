// serveur.c
#include "pse.h"

#define CMD         "serveur"
#define SaveBC "SaveBC.log"
#define SaveID "SaveID.log"
#define NB_WORKERS  2

void creerCohorteWorkers(void);
int chercherWorkerLibre(void);
void *threadWorker(void *arg);
void sessionClient(int canal);
void ecrireDansFdBC(void);
void ecrireDansFdID(void);
void remiseAZeroFdBC(void);
void remiseAZeroFdID(void);
void lockMutexFd(pthread_mutex_t* mutexFd);
void unlockMutexFd(pthread_mutex_t* mutexFd);

int fdBC, fdID;
DataSpec dataSpec[NB_WORKERS];
sem_t semWorkersLibres;

// mutex pour acces concurrent au descripteur du fichier journal et au canal de
// chaque worker
pthread_mutex_t mutexFdBC;
pthread_mutex_t mutexFdID;

pthread_mutex_t mutexCanal[NB_WORKERS];

int main(int argc, char *argv[]) {
  short port;
  int ecoute, canal, ret;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;
  int numWorkerLibre;

  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]);

  fdBC = open(SaveBC, O_CREAT|O_WRONLY|O_APPEND, 0644);
  if (fdBC == -1)
    erreur_IO("ouverture SaveBC");

  fdID = open(SaveID, O_CREAT|O_WRONLY|O_APPEND, 0644);
  if (fdID == -1)
    erreur_IO("ouverture SaveID");

  creerCohorteWorkers();

  ret = sem_init(&semWorkersLibres, 0, NB_WORKERS);
  if (ret == -1)
    erreur_IO("init semaphore workers libres");

  printf("%s: creating a socket\n", CMD);
  ecoute = socket(AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");
  
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind(ecoute, (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
  if (ret < 0)
    erreur_IO("bind");
  
  printf("%s: listening to socket\n", CMD);
  ret = listen(ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");

  while (VRAI) {
    printf("%s: accepting a connection\n", CMD);
    lgAdrClient = sizeof(adrClient);
    canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
    if (canal < 0)
      erreur_IO("accept");

    printf("%s: adr %s, port %hu\n", CMD,
        stringIP(ntohl(adrClient.sin_addr.s_addr)), ntohs(adrClient.sin_port));

    ret = sem_wait(&semWorkersLibres);
    if (ret == -1)
      erreur_IO("wait semaphore workers libres");
    numWorkerLibre = chercherWorkerLibre();

    dataSpec[numWorkerLibre].canal = canal;
    ret = sem_post(&dataSpec[numWorkerLibre].sem);
    if (ret == -1)
      erreur_IO("post semaphore worker");
  }

  if (close(ecoute) == -1)
    erreur_IO("fermeture ecoute");

  if (close(fdBC) == -1)
    erreur_IO("fermeture SaveBC");
  if (close(fdID) == -1)
    erreur_IO("fermeture SaveID");

  exit(EXIT_SUCCESS);
}

void creerCohorteWorkers(void) {
  int i, ret;

  for (i = 0; i < NB_WORKERS; i++) {
    dataSpec[i].canal = -1;
    dataSpec[i].tid = i;

    ret = sem_init(&dataSpec[i].sem, 0, 0);
    if (ret == -1)
      erreur_IO("init semaphore worker");

    ret = pthread_create(&dataSpec[i].id, NULL, threadWorker, &dataSpec[i]);
    if (ret != 0)
      erreur_IO("creation worker");
  }
}

// retourne le no. du worker ou -1 si pas de worker libre
int chercherWorkerLibre(void) {
  int i, canal;

  for (i = 0; i < NB_WORKERS; i++) {
    lockMutexFd(&mutexCanal[i]);
    canal = dataSpec[i].canal;
    unlockMutexFd(&mutexCanal[i]);
    if (canal < 0)
      return i;
  }

  return -1;
}

void *threadWorker(void *arg) {
  DataSpec *dataSpec = (DataSpec *)arg; 
  int ret;

  while (VRAI) {
    ret = sem_wait(&dataSpec->sem);
    if (ret == -1)
      erreur_IO("wait semaphore worker");

    printf("worker %d: reveil\n", dataSpec->tid);

    sessionClient(dataSpec->canal);

    lockMutexFd(&mutexCanal[dataSpec->tid]);
    dataSpec->canal = -1;
    unlockMutexFd(&mutexCanal[dataSpec->tid]);

    printf("worker %d: sommeil\n", dataSpec->tid);

    ret = sem_post(&semWorkersLibres);
    if (ret == -1)
      erreur_IO("post semaphore workers libres");
  }

  pthread_exit(NULL);
}

void sessionClient(int canal) {
  int fin = FAUX;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;
  char end;

  while (!fin) 
  {
    lgLue = lireLigne(canal, ligne, '\n');
    printf("ligne lue : %s\n", ligne);
    if (lgLue == -1)
      erreur_IO("lecture canal");

    else if (lgLue == 0) // arret du client (CTRL-D, interruption)
    {  
      fin = VRAI;
      printf("%s: arret du client\n", CMD);
    }
    else // lgLue > 0
    {
      if (strcmp(ligne, "fin") == 0) 
      {
        fin = VRAI;
        printf("%s: fin session client\n", CMD);
      }
      else if (strcmp(ligne, "Demande envoie TabID") == 0)  //Client demande qu'on lui envoie TabID
      {
        getTabID(fdID);
        printTabID(&TabID);
        sendTabID(canal);
      }
      else if (strcmp(ligne, "Demande envoie BC") == 0)  //Client demande qu'onn lui envoie la BlockChain
      {
        getBlockChain(fdBC);
        sendBlockchain(canal);        
      }
      else if (strcmp(ligne, "Demande reception TabID") == 0)
      {
        getTabID(canal);          //On récupère TabID
        remiseAZeroFdID();        //On sauvegarde la TabID dans fdID donc on le réinitialise avant
        ecrireDansFdID();
      }
      else if (strcmp(ligne, "Demande recption BC") == 0)
      {
        getBlockChain(canal);     //On récupère BlockChain
        remiseAZeroFdBC();    //On sauvegarde la BlockChain dans fdBC donc on le réinitialise avant
        ecrireDansFdBC();
      }
    }
  }
  if (close(canal) == -1)
    erreur_IO("fermeture canal");
}

  

void ecrireDansFdBC(void)
{
  char buffer[LIGNE_MAX];
  strcpy(buffer, "fin BC");
  char end = END;
  lockMutexFd(&mutexFdBC);
  sendBlockchain(fdBC);
  ecrireLigne(fdBC, buffer, end);
  unlockMutexFd(&mutexFdBC);
}
void ecrireDansFdID(void)
{
  char buffer[LIGNE_MAX];
  strcpy(buffer, "fin ID");
  char end = END;
  lockMutexFd(&mutexFdID);
  sendTabID(fdID);
  ecrireLigne(fdBC, buffer, end);
  unlockMutexFd(&mutexFdID);
}

void remiseAZeroFdBC(void)
{
  lockMutexFd(&mutexFdBC);

  if (close(fdBC) < 0)
    erreur_IO("fermeture fichier pour remise a zero");

  fdBC = open("SaveBC.log", O_TRUNC|O_WRONLY|O_APPEND);
  if (fdBC < 0)
    erreur_IO("reouverture SaveBC");

  unlockMutexFd(&mutexFdBC);
}

void remiseAZeroFdID(void)
{
  lockMutexFd(&mutexFdID);

  if (close(fdID) < 0)
    erreur_IO("fermeture fichier pour remise a zero");

  fdID = open("SaveID.log", O_TRUNC|O_WRONLY|O_APPEND);
  if (fdID < 0)
    erreur_IO("reouverture SaveID");

  unlockMutexFd(&mutexFdID);
}

void lockMutexFd(pthread_mutex_t* mutexFd)
{
  int ret;

  ret = pthread_mutex_lock(mutexFd);
  if (ret != 0)
    erreur_IO("lock mutex descipteur");
}
void unlockMutexFd(pthread_mutex_t* mutexFd)
{
  int ret;

  ret = pthread_mutex_unlock(mutexFd);
  if (ret != 0)
    erreur_IO("unlock mutex descipteur");
}

