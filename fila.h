#ifndef FILA

#define FILA

#include "arvore.h"

typedef struct ItemFila {
    No* item;
    unsigned int prioridade;
    struct ItemFila *proximo;
    struct ItemFila *anterior;
} ItemFila;

typedef struct Fila {
    ItemFila *inicio;
    ItemFila *fim;
} Fila;

Fila* criarFila();
bool inserir(Fila *fila, No* item, unsigned int prioridade);
void exibirFila(Fila *fila);
No* gerarArvore(Fila* fila);
static No* removerInicioRetornando(Fila* fila);

#endif
