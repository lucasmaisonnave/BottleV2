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
int ecrireDansJournal(char *ligne);
void remiseAZeroJournal(void);
void lockMutexFdJournal(void);
void unlockMutexFdJournal(void);
void lockMutexCanal(int numWorker);
void unlockMutexCanal(int numWorker);

int fdBC, fdID;
DataSpec dataSpec[NB_WORKERS];
sem_t semWorkersLibres;

// mutex pour acces concurrent au descripteur du fichier journal et au canal de
// chaque worker
pthread_mutex_t mutexFdJournal;
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

  if (close(fdJournal) == -1)
    erreur_IO("fermeture journal");

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
    lockMutexCanal(i);
    canal = dataSpec[i].canal;
    unlockMutexCanal(i);
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

    lockMutexCanal(dataSpec->tid);
    dataSpec->canal = -1;
    unlockMutexCanal(dataSpec->tid);

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

  while (!fin) {
    lgLue = lireLigne(canal, ligne);
    if (lgLue == -1)
      erreur_IO("lecture canal");

    else if (lgLue == 0) {  // arret du client (CTRL-D, interruption)
      fin = VRAI;
      printf("%s: arret du client\n", CMD);
    }
    else {  // lgLue > 0
      if (strcmp(ligne, "fin") == 0) {
        fin = VRAI;
        printf("%s: fin session client\n", CMD);
      }
      else if (strcmp(ligne, "init") == 0) {
        remiseAZeroJournal();
        printf("%s: remise a zero du journal\n", CMD);
      }
      else {
        lgEcr = ecrireLigne(fdJournal, ligne);
        if (lgEcr < 0)
          erreur_IO("ecriture journal");
        printf("%s: ligne de %d octets ecrite dans journal\n", CMD, lgEcr);
      }
    }
  }

  if (close(canal) == -1)
    erreur_IO("fermeture canal");
}

int ecrireDansJournal(char *ligne) {
  int lg;

  lockMutexFdJournal();
  lg = ecrireLigne(fdJournal, ligne);
  unlockMutexFdJournal();

  return lg;
}

void remiseAZeroJournal(void) {
  lockMutexFdJournal();

  if (close(fdJournal) < 0)
    erreur_IO("fermeture journal pour remise a zero");

  fdJournal = open("journal.log", O_TRUNC|O_WRONLY|O_APPEND);
  if (fdJournal < 0)
    erreur_IO("reouverture journal");

  unlockMutexFdJournal();
}

void lockMutexFdJournal(void)
{
  int ret;

  ret = pthread_mutex_lock(&mutexFdJournal);
  if (ret != 0)
    erreur_IO("lock mutex descipteur journal");
}

void unlockMutexFdJournal(void)
{
  int ret;

  ret = pthread_mutex_unlock(&mutexFdJournal);
  if (ret != 0)
    erreur_IO("unlock mutex descipteur journal");
}

void lockMutexCanal(int numWorker)
{
  int ret;

  ret = pthread_mutex_lock(&mutexCanal[numWorker]);
  if (ret != 0)
    erreur_IO("lock mutex canal");
}

void unlockMutexCanal(int numWorker)
{
  int ret;

  ret = pthread_mutex_unlock(&mutexCanal[numWorker]);
  if (ret != 0)
    erreur_IO("unlock mutex canal");
}
