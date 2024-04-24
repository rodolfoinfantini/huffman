#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "arvore.h"

Fila* criarFila() {
    Fila *fila = (Fila *) malloc(sizeof(Fila));
    fila->inicio = NULL;
    fila->fim = NULL;
    return fila;
}

bool inserir(Fila *fila, No* item, unsigned int prioridade) {
    ItemFila *novoItem = (ItemFila *) malloc(sizeof(ItemFila));
    if (novoItem == NULL)
        return false;

    novoItem->item = item;
    novoItem->prioridade = prioridade;
    novoItem->proximo = NULL;
    novoItem->anterior = NULL;

    if (fila -> inicio == NULL) {
        fila->inicio = novoItem;
        fila->fim = novoItem;
        return true;
    }

    ItemFila *atual = fila->fim;
    while (atual != NULL && atual->prioridade > prioridade) {
        atual = atual->anterior;
    }
    if (atual == NULL) {
        novoItem->proximo = fila->inicio;
        fila->inicio->anterior = novoItem;
        fila->inicio = novoItem;
        return true;
    }

    novoItem->proximo = atual->proximo;
    novoItem->anterior = atual;
    if (atual->proximo != NULL) {
        atual->proximo->anterior = novoItem;
    } else {
        fila->fim = novoItem;
    }
    atual->proximo = novoItem;

    return true;
}

void exibirFila(Fila *fila) {
    ItemFila *atual = fila->inicio;
    while (atual != NULL) {
        printf("Item: %c, Prioridade: %d\n", atual->item->caracter, atual->prioridade);
        atual = atual->proximo;
    }
}

No* gerarArvore(Fila* fila) {
    while (fila->inicio->proximo != NULL) {
        No *no1 = removerInicioRetornando(fila);
        No *no2 = removerInicioRetornando(fila);

        No *noPai;
        if (!criarNo(0, no1->frequencia + no2->frequencia, &noPai)) {
            printf("Erro ao criar nó pai!\n");
            return NULL;
        }

        noPai->esquerda = no1;
        noPai->direita = no2;

        if (!inserir(fila, noPai, noPai->frequencia)) {
            printf("Erro ao inserir nó pai na fila!\n");
            return NULL;
        }
    }
    return fila->inicio->item;
}

No* removerInicioRetornando(Fila* fila) {
    ItemFila *item = fila->inicio;
    fila->inicio = fila->inicio->proximo;
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    } else {
        fila->inicio->anterior = NULL;
    }
    No* no = item->item;
    free(item);
    return no;
}
