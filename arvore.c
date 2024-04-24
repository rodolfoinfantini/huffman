#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "arvore.h"

bool criarNo(unsigned char caracter, unsigned int frequencia, No **no) {
    *no = (No *) malloc(sizeof(No));
    if (*no == NULL) {
        return false;
    }

    (*no)->caracter = caracter;
    (*no)->frequencia = frequencia;
    (*no)->esquerda = NULL;
    (*no)->direita = NULL;

    return true;
}

void emOrdem(No* no) {
    if (no == NULL) {
        return;
    }

    emOrdem(no->esquerda);
    printf("Caracter: %c, Frequência: %d\n", no->caracter, no->frequencia);
    emOrdem(no->direita);
}
