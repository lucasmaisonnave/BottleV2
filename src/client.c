#include "pse.h"
#define CMD   "client"

int main(int argc, char *argv[])
{
    int sock, ret;
    struct sockaddr_in *adrServ;
    int fin = FAUX;
    char ligne[LIGNE_MAX];

    signal(SIGPIPE, SIG_IGN);

    if (argc != 3)
        erreur("usage: %s machine port\n", argv[0]);

    printf("%s: creating a socket\n", CMD);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        erreur_IO("socket");

    printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
    adrServ = resolv(argv[1], argv[2]);
    if (adrServ == NULL)
        erreur("adresse %s port %s inconnus\n", argv[1], argv[2]);

    printf("%s: adr %s, port %hu\n", CMD,
                stringIP(ntohl(adrServ->sin_addr.s_addr)),
                ntohs(adrServ->sin_port));

    printf("%s: connecting the socket\n", CMD);
    ret = connect(sock, (struct sockaddr *)adrServ, sizeof(struct sockaddr_in));
    if (ret < 0)
        erreur_IO("connect");

    Genesis = (struct genesis*)malloc(sizeof(struct genesis));
    initTabID(&TabID);
    initGenesis();
    init_global(&Bottle);
    
    Etat = ETAT_MENU;

    LoadTabIDFromFile(&TabID, FileNameID);
    LoadBlockChainFromFile1(FileNameBC);

    
    /*-------Threads--------*/
    pthread_t id[4];
    int ret1 = pthread_create(&id[0], NULL, MenuThread, NULL);
    int ret2 = pthread_create(&id[1], NULL, CompteConnecterThread, NULL);
    int ret3 = pthread_create(&id[2], NULL, DestinataireThread, NULL);
    int ret4 = pthread_create(&id[3], NULL, MessagerieThread, NULL);
    /*----------------------*/    

    while(Etat != ETAT_QUIT)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    Etat = ETAT_QUIT;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    Input.PointPressed.x = event.button.x;
                    Input.PointPressed.y = event.button.y;
                    printf("x = %d, y = %d\n", Input.PointPressed.x,Input.PointPressed.y);
                    break;
                case SDL_KEYDOWN:
                    Input.BouttonClavier = event.key.keysym.sym;
                    break;

            }
        }
    }
    SDL_Dest(&Bottle);
    TTF_Quit();
    SDL_Quit();
    if (close(sock) == -1)
        erreur_IO("fermeture socket");

    exit(EXIT_SUCCESS);
}
