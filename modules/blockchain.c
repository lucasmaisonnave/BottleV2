#include "blockchain.h"
#include "openssl/sha.h"
#include "openssl/crypto.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



/*              Fonctions utilitaires               */



char *toString(struct bloc *bloc, char *str)    //Conversion d'un block en type "string"
{
    if(bloc == NULL)
        return NULL;
    char block_string[BLOCK_STR_SIZE] = "";
    strcat(block_string, bloc->precHash);
    strcat(block_string, "~");
    sprintf(block_string, "%d~%d", bloc->nonce, bloc->index);
    strcat(block_string, "~");    
    strcat(block_string, bloc->donnee->date);
    strcat(block_string, "~");
    strcat(block_string, bloc->donnee->dest);
    strcat(block_string, "~");
    strcat(block_string, bloc->donnee->exp);
    strcat(block_string, "~");
    strcat(block_string, bloc->donnee->message);
    strcat(block_string, "~");


    strcpy(str, block_string);
    return str;
}


void printBlock(struct bloc *blocs)     //Imprime un block
{
    printf("bloc %d : ", blocs->index);
    printf("precHash : ");
    printf(" %s", blocs->precHash);
    printf(" [%s]->", blocs->donnee->exp);
    printf("[%s] ", blocs->donnee->dest);
    printf("%s : ", blocs->donnee->date);
    printf("%s //", blocs->donnee->message);
    printf("Hash : ");
    printf(" %s", blocs->Hash);
    printf("  %p\n\n",blocs->lien);
}


void printAllBlock(void)        //Imprime tous les blocks de la blockchain
{
    struct bloc * curr = Genesis->premier;
    while(curr)
    {
        printBlock(curr);
        curr = curr->lien;
    }
}


void init_Data(donnee* data)        //Initialise le message, l'expéditeur, le destinataire et la date
{
    strcpy(data->date, "");
    strcpy(data->dest, "");
    strcpy(data->exp, "");
    strcpy(data->message, "");
}


void hexToBinary(char input[HASH_HEX_SIZE], char output[BINARY_SIZE])   //Conversion Héxadécimal->Binaire
{
    long int i = 0;
    while (input[i])
    {

        switch (input[i])
        {
            case '0':
                strcat(output, "0000");
                break;
            case '1':
                strcat(output, "0001");
                break;
            case '2':
                strcat(output, "0010");
                break;
            case '3':
                strcat(output, "0011");
                break;
            case '4':
                strcat(output, "0100");
                break;
            case '5':
                strcat(output, "0101");
                break;
            case '6':
                strcat(output, "0110");
                break;
            case '7':
                strcat(output, "0111");
                break;
            case '8':
                strcat(output, "1000");
                break;
            case '9':
                strcat(output, "1001");
                break;
            case 'A':
            case 'a':
                strcat(output, "1010");
                break;
            case 'B':
            case 'b':
                strcat(output, "1011");
                break;
            case 'C':
            case 'c':
                strcat(output, "1100");
                break;
            case 'D':
            case 'd':
                strcat(output, "1101");
                break;
            case 'E':
            case 'e':
                strcat(output, "1110");
                break;
            case 'F':
            case 'f':
                strcat(output, "1111");
                break;
            default:
                printf("Caractère héxadécimal invalide %c\n",
                       input[i]);
        }
        i++;
    }
}


void hash256(unsigned char *output, const char *input)  //Calcule le hash (non exploitable) : utilisation de la librairie de OpenSSL
{

    size_t length = strlen(input);
    unsigned char md[HASH_SIZE] = {0};
    SHA256((const unsigned char*)input, length, md);
    memcpy(output,md, HASH_SIZE);

    return;
}


char *Hex_Hash(struct bloc *Bloc, char *output)     //Calcul le hachage en Héxadécimal
{

    char bloc_string[BLOCK_STR_SIZE];
    toString(Bloc, bloc_string);

    unsigned char hash_value[HASH_SIZE];
    hash256(hash_value, bloc_string);

    char buffer[3];
    char hex_hash[HASH_HEX_SIZE] = {0};

    for(int i = 0; i < HASH_SIZE; i++) {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"%02x", hash_value[i]);
        strcat(hex_hash, buffer);
    }

    strcpy(output,hex_hash);

    output[HASH_HEX_SIZE] = 0;

    return output;
}


/*              Fonctions Principales               */

char* getTime(char* Time)
{
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    
    info = localtime( &rawtime );
    char str2[30];

    /* copy to sized buffer (overflow safe): */
    strncpy ( str2, asctime(info), sizeof(str2) );

    /* partial copy (only 5 chars): */
    strncpy ( Time, str2, 24 );
    Time[24] = '\0';   /* null character manually added */
    return Time;
}

void initGenesis()
{
    Genesis->premier = NULL;
    Genesis->taille = 0;
}

void ajout_block(donnee* message)       //Pour l'ajout d'un nouveau block
{
    struct bloc *currentbloc = Genesis->premier;

    struct bloc *nouv_bloc = (struct bloc *)malloc(sizeof(struct bloc));
    nouv_bloc->donnee = (donnee*)malloc(sizeof(donnee));
    strcpy(nouv_bloc->donnee->date, message->date);
    strcpy(nouv_bloc->donnee->exp, message->exp);
    //printf("nouv_bloc->exp : %s\n", nouv_bloc->donnee->exp);
    strcpy(nouv_bloc->donnee->dest, message->dest);

    strcpy(nouv_bloc->donnee->message, message->message);
    
    if(currentbloc != NULL)
    {
        nouv_bloc->index = currentbloc->index + 1;
        strcpy(nouv_bloc->precHash, currentbloc->Hash);
    }
    else
    {
        nouv_bloc->index = 1;
        strcpy(nouv_bloc->precHash, "0");
    }
    calculHash(nouv_bloc);
    if(IsValidBlock(nouv_bloc,currentbloc))     //Test de validité du block
    {
        nouv_bloc->lien = currentbloc;          //Ajout du block au début de la blockchain
        Genesis->premier = nouv_bloc;
        Genesis->taille++;
    }
    else                                        //Bloc Invalide
    {
        printf("\nBloc invalide, veuillez retenter s'il vous plaît\n");
    }
}



bool IsValidBlock(struct bloc* newBlock, struct bloc* previousBlock)    //Test de la validité d'un block
{
    char hash_to_test[HASH_HEX_SIZE];

    Hex_Hash(newBlock, hash_to_test);
    
    if(previousBlock != NULL)
    {
        if (previousBlock->index + 1 != newBlock->index)    //Index exact?
        {
            printf("\nBlock invalide : indexs invalide (%d + 1 != %d).\n", previousBlock->index, newBlock->index);
            return false;
        }

        else if (strcmp(previousBlock->Hash, newBlock->precHash) != 0)  //Bon hachage du précédent block?
        {
            printf("\nBlock invalide : hash précédent invalide.\n");
            return false;
        }
    }   

    if (strcmp(hash_to_test, newBlock->Hash) != 0)     //Recalcul du hachage
    {
        printf("\nBlock invalide: hashs différents (%s != %s)\n", hash_to_test, newBlock->Hash);
        return false;
    }

    return true;
}


void calculHash(struct bloc* Bloc)  //Calcul du Hash d'un block
{
    Bloc->nonce = 0;        //Mise en place d'une variable nonce qui sera celle qui changera pour modifier le hash (PoW)
    char hash_hex[HASH_HEX_SIZE];
    Hex_Hash(Bloc, hash_hex);
    strcpy(Bloc->Hash, hash_hex);

    while(!HashMatchesDifficulty(hash_hex))
    {
        Bloc->nonce++;
        Hex_Hash(Bloc, hash_hex);       //Conversion du hash en hex
    }
    strcpy(Bloc->Hash, hash_hex);       //Mise à jour du hash

    return;
}


bool HashMatchesDifficulty(char Hex[HASH_HEX_SIZE])     //Vérifie que le hash possède le bon nombre de 0
{
    char binaryHash[BINARY_SIZE] =  {0};
    hexToBinary(Hex, binaryHash);

    // Creation de la chaine de caractère "0"*difficulty"
    char *prefix = malloc(DIFFICULTY + 1);
    memset(prefix, '0', DIFFICULTY);
    prefix[DIFFICULTY] = '\0';

    char *checker = NULL;
    checker = strstr(binaryHash, prefix);
    return checker == binaryHash;
}

/*----------------------------Sauvegarde----------------------------------*/

void SaveBlockChain(char* filename)
{
    FILE* file = fopen(filename, "w");
    struct bloc* current = Genesis->premier;
    while(current != NULL)
    {
        printBlock(current);
        fprintf(file, "%s %d %d %s %s %s %s %s\n", current->precHash,current->nonce, current->index, current->Hash, current->donnee->date, current->donnee->dest, current->donnee->exp, current->donnee->message);
        current = current->lien;
    }

    fclose(file);
}

void LoadBlockChainFromFile1(char* filename)
{
    FILE* file = fopen(filename, "r");
    struct bloc *current = (struct bloc *)malloc(sizeof(struct bloc));
    current->donnee = (donnee*)malloc(sizeof(donnee));
    struct bloc *previous = (struct bloc *)malloc(sizeof(struct bloc));
    char jour[10];   //Mon Tue ...
    char mois[10];   //Mar Apr..
    char jourM[10];   //1 22 ou 12
    char heure[10];
    char annee[10];
                             //0c7b0cfd110a8d979fb2b42091790609825a2050efa9176d88f09b81e156333d 17 2 0ea22659df682dfd90dcb462f77f4721ffbdd506f663ccf45736ab849b6b13fd Mon Apr  6 16:25:28 2020 TRISTAN LUCAS DR
    while(fscanf(file, "%s %d %d %s %s %s  %s %s %s %s %s %s\n", current->precHash,&current->nonce, &current->index, current->Hash, jour, mois, jourM, heure, annee, current->donnee->dest, current->donnee->exp, current->donnee->message) == 12)
    {
        strcpy(current->donnee->date, "");
        strcat(current->donnee->date, jour);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, mois);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, jourM);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, heure);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, annee);

        current->lien = Genesis->premier;        
        previous = current->lien;

        if(IsValidBlock(current, previous))
        {
            Genesis->premier = current;
        }
        else
        {
            printBlock(current);
            fclose(file);
            file = fopen(filename, "w");    //On supprime le contenue de SaveBC
            fclose(file);
            initGenesis();
            return;
        }     
        current = (struct bloc *)malloc(sizeof(struct bloc));
        current->donnee = (donnee*)malloc(sizeof(donnee));
    }
    fclose(file);
}

void LoadBlockChainFromFile2(char* filename)
{
    FILE* file = fopen(filename, "r");
    struct bloc *current = (struct bloc *)malloc(sizeof(struct bloc));
    current->donnee = (donnee*)malloc(sizeof(donnee));
    char jour[10];   //Mon Tue ...
    char mois[10];   //Mar Apr..
    char jourM[10];   //1 22 ou 12
    char heure[10];
    char annee[10];
                             //0c7b0cfd110a8d979fb2b42091790609825a2050efa9176d88f09b81e156333d 17 2 0ea22659df682dfd90dcb462f77f4721ffbdd506f663ccf45736ab849b6b13fd Mon Apr  6 16:25:28 2020 TRISTAN LUCAS DR
    while(fscanf(file, "%s %d %d %s %s %s  %s %s %s %s %s %s\n", current->precHash,&current->nonce, &current->index, current->Hash, jour, mois, jourM, heure, annee, current->donnee->dest, current->donnee->exp, current->donnee->message) == 12)
    {
        strcpy(current->donnee->date, "");
        strcat(current->donnee->date, jour);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, mois);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, jourM);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, heure);
        strcat(current->donnee->date, " ");
        strcat(current->donnee->date, annee);

        current->lien = Genesis->premier;        
        Genesis->premier = current;
        current = (struct bloc *)malloc(sizeof(struct bloc));
        current->donnee = (donnee*)malloc(sizeof(donnee));
    }
    fclose(file);
}

void SaveBlockChain2(char* filename)    /*On save et Load 2 fois car la blockchain a été codé comme une pile et donc save inverse le sens de la pile*/
{
    SaveBlockChain(filename);
    initGenesis();
    LoadBlockChainFromFile2(filename);
    SaveBlockChain(filename);
    initGenesis();
    LoadBlockChainFromFile2(filename);
}