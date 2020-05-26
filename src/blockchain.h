#include <stdbool.h>


#ifndef BLOCKCHAIN_H
    
    #define BLOCKCHAIN_H

    #define N 200               //Taille max de la blockchain
    #define MaxMessage 50      //Taille max des messages
    #define MAX_WORD_LENGHT	20	// Maximum word length à réajuster pour les mots de passe et identifiants
    #define DIFFICULTY 4        //Difficulté de la proof of work
    #define FileNameBC "SaveBC.txt"

    #define HASH_SIZE 32
    #define HASH_HEX_SIZE 65
    #define BINARY_SIZE (HASH_HEX_SIZE*4+1)
    #define BLOCK_STR_SIZE 400

    typedef struct{
        char message[MaxMessage];
        char exp[MAX_WORD_LENGHT];
        char dest[MAX_WORD_LENGHT];
        char date[50];
    }donnee;

    struct bloc{
        char precHash[HASH_HEX_SIZE];
        char Hash[HASH_HEX_SIZE];
        int index;
        donnee* donnee;
        int nonce;      //utile pour la Pow
        struct bloc *lien;
    };

    struct genesis
    {
        struct bloc *premier;
        int taille;
    }*Genesis;




    void ajout_block(donnee* message);
    char *toString(struct bloc *blocks, char *str);
    void printBlock(struct bloc *blocs);
    void printAllBlock(void);
    void init_Data(donnee* data);
    bool HashMatchesDifficulty(char Hex[HASH_HEX_SIZE]);
    void hexToBinary(char input[HASH_HEX_SIZE], char output[BINARY_SIZE]);
    void hash256(unsigned char *output, const char *input);
    bool IsValidBlock(struct bloc* newBlock, struct bloc* previousBlock);
    void calculHash(struct bloc* Block);
    char *Hex_Hash(struct bloc *Bloc, char *output);
    void LoadBlockChainFromFile1(char* filename);
    void LoadBlockChainFromFile2(char* filename);
    void SaveBlockChain(char* filename);
    void initGenesis();
    char* getTime(char* Time);
#endif