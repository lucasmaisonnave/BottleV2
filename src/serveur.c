// serveur.c
#include "pse.h"

#define CMD         "serveur"

#define NB_WORKERS  2

void creerCohorteWorkers(void);
int chercherWorkerLibre(void);
void *threadWorker(void *arg);
void sessionClient(DataSpec *dataSpec);
void ecrireDansFdBC(void);
void ecrireDansFdID(void);
void remiseAZeroFdBC(void);
void remiseAZeroFdID(void);
void lockMutexFd(pthread_mutex_t* mutexFd);
void unlockMutexFd(pthread_mutex_t* mutexFd);
int openFd(char* Save);
void closeFd(int fd);

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
  initGenesis();
  
  initTabID(&TabID);

  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]);

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

    sessionClient(dataSpec);

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

void sessionClient(DataSpec *dataSpec) {
  int fin = FAUX;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;
  char end;
  int canal = dataSpec->canal;
  int tid = dataSpec->tid;

  while (!fin) 
  {
    lgLue = lireLigne(canal, ligne, '\n');
    if (lgLue == -1)
      erreur_IO("lecture canal");
    else if (lgLue == 0) // arret du client (CTRL-D, interruption)
    {  
      fin = VRAI;
      printf("%s: arret du client %d\n", CMD, tid);
    }
    else // lgLue > 0
    {
      if (strcmp(ligne, ASK_FIN) == 0)
      {
        fin = VRAI;
        printf("%s: fin session client\n", CMD);
      }
      else if (strcmp(ligne, ASK_SEND_TABID) == 0)  //Client demande qu'on lui envoie TabID
      {
        printf("%s : (requête client %d) %s\n", CMD, tid, ligne);
        fdID = openFd(SaveID); 
        getTabID(fdID);
        close(fdID);
        sendTabID(canal);
      }
      else if (strcmp(ligne, ASK_SEND_BC) == 0)  //Client demande qu'onn lui envoie la BlockChain
      {
        printf("%s : (requête client %d) %s\n", CMD, tid, ligne);
        fdBC = openFd(SaveBC);
        getBlockChain(fdBC);
        closeFd(fdBC);
        sendBlockChain(canal);
      }
      else if (strcmp(ligne, ASK_RECEIVE_TABID) == 0) //Client demande une reception de TabID
      {
        printf("%s : (requête client %d) %s\n", CMD, tid, ligne);
        getTabID(canal);          //On récupère TabID
        ecrireDansFdID();
      }
      else if (strcmp(ligne, ASK_RECEIVE_BC) == 0)  //Client demande une reception de la BlockChain
      {
        printf("%s : (requête client %d) %s\n", CMD, tid, ligne);
        getBlockChain(canal);     //On récupère BlockChain
        ecrireDansFdBC();
      }
    }
  }
  if (close(canal) == -1)
    erreur_IO("fermeture canal");
}

  

void ecrireDansFdBC(void)
{
  remiseAZeroFdBC();        //On sauvegarde la BlockChain dans fdBC donc on le réinitialise avant
  lockMutexFd(&mutexFdBC);
  fdBC = openFd(SaveBC);
  sendBlockChain(fdBC);
  closeFd(fdBC);
  unlockMutexFd(&mutexFdBC);
  printf("%s : fichier %s mis à jour\n", CMD, SaveBC);
}
void ecrireDansFdID(void)
{
  remiseAZeroFdID();        //On sauvegarde la TabID dans fdID donc on le réinitialise avant
  lockMutexFd(&mutexFdID);
  fdID = openFd(SaveID);
  sendTabID(fdID);
  close(fdID);
  unlockMutexFd(&mutexFdID);
  printf("%s : fichier %s mis à jour\n", CMD, SaveID);
}

void remiseAZeroFdBC(void)
{
  lockMutexFd(&mutexFdBC);
  
  fdBC = open(SaveBC, O_TRUNC|O_WRONLY|O_APPEND);
  if (fdBC < 0)
    erreur_IO("reouverture SaveBC");
  if (close(fdBC) < 0)
    erreur_IO("fermeture fichier pour remise a zero");

  unlockMutexFd(&mutexFdBC);
}

void remiseAZeroFdID(void)
{
  lockMutexFd(&mutexFdID);

  fdID = open(SaveID, O_TRUNC|O_WRONLY|O_APPEND);
  if (fdID < 0)
    erreur_IO("reouverture SaveID");
  if (close(fdID) < 0)
    erreur_IO("fermeture fichier pour remise a zero");

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

int openFd(char* Save)
{
  int fd;
  fd = open(Save, O_RDWR|O_APPEND, 0644);
  if (fd == -1)
    erreur_IO("ouverture fichier\n");
  return fd;
}
void closeFd(int fd)
{
  if (close(fd) == -1)
    erreur_IO("fermeture fichier");
}
