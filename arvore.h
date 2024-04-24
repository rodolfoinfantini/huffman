#ifndef ARVORE

#define ARVORE

typedef struct No {
    unsigned char caracter;
    unsigned int frequencia;
    struct No *esquerda;
    struct No *direita;
} No;

bool criarNo(unsigned char caracter, unsigned int frequencia, No **no);
void emOrdem(No *no);

#endif
