#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "arvore.h"
#include "fila.h"

bool compactar(char *nomeArquivo);
bool descompactar(char *nomeArquivo);
void gerarCodigos(No* no, unsigned char codigo[256][256], unsigned int tamanhoCodigo[256], unsigned int profundidade, unsigned int valor);
void codificarArvore(No* no, FILE* arquivo);
No* decodificarArvore(FILE* arquivo);

int main() {
    printf("Deseja compactar ou descompactar? (c/d) ");
    char opcao;
    scanf("%c", &opcao);
    printf("Digite o nome do arquivo: ");
    char nomeArquivo[100];
    scanf("%s", nomeArquivo);

    if (opcao == 'c') {
        if (compactar(nomeArquivo)) {
            printf("Compactação realizada com sucesso!\n");
            return 0;
        }

        printf("Erro ao compactar arquivo!\n");
    } else if (opcao == 'd') {
        if (descompactar(nomeArquivo)) {
            printf("Descompactação realizada com sucesso!\n");
            return 0;
        }

        printf("Erro ao descompactar arquivo!\n");
    } else {
        printf("Opção inválida!\n");
    }

    return 0;
}

bool compactar(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb+");
    if (arquivo == NULL) {
        printf("Arquivo não encontrado!\n");
        return false;
    }

    No nodes[256];
    for (unsigned int i = 0; i < 256; i++) {
        nodes[i].caracter = i;
        nodes[i].frequencia = 0;
        nodes[i].esquerda = NULL;
        nodes[i].direita = NULL;
    }

    unsigned int bytesLidos = 0;
    unsigned char buffer[1024];
    do {
        bytesLidos = fread(buffer, 1, 1024, arquivo);
        for (unsigned int i = 0; i < bytesLidos; i++) {
            No *no = &nodes[buffer[i]];
            no->frequencia++;
        }
    } while (bytesLidos == 1024);
    fclose(arquivo);

    Fila* fila = criarFila();
    for (unsigned int i = 0; i < 256; i++) {
        No *no = &nodes[i];
        if (no->frequencia == 0)
            continue;

        if(!inserir(fila, no, no->frequencia)) {
            printf("Erro ao inserir nó na fila!\n");
            return false;
        }
    }
    exibirFila(fila);

    No* arvore = gerarArvore(fila);
    if (arvore == NULL) {
        printf("Erro ao gerar árvore!\n");
        return false;
    }

    unsigned char codigo[256][256];
    unsigned int tamanhoCodigo[256];
    for (unsigned int i = 0; i < 256; i++) {
        tamanhoCodigo[i] = 0;
    }

    gerarCodigos(arvore, codigo, tamanhoCodigo, 0, 0);

    arquivo = fopen(nomeArquivo, "rb+");
    if (arquivo == NULL) {
        printf("Arquivo não encontrado!\n");
        return false;
    }

    char nomeArquivoDestino[1000];
    sprintf(nomeArquivoDestino, "%s.rzip", nomeArquivo);

    FILE *arquivoDestino = fopen(nomeArquivoDestino, "wb+");
    if (arquivoDestino == NULL) {
        printf("Erro ao criar arquivo destino!\n");
        return false;
    }

    codificarArvore(arvore, arquivoDestino);

    unsigned char byteAtual = 0;
    unsigned int bitsEscritos = 0;
    bytesLidos = 0;
    do {
        bytesLidos = fread(buffer, 1, 1024, arquivo);

        for (unsigned int i = 0; i < bytesLidos; i++) {
            unsigned int tamanho = tamanhoCodigo[buffer[i]];
            for (unsigned int j = 0; j < tamanho; j++) {
                unsigned char codigoAtual = codigo[buffer[i]][j];
                byteAtual = byteAtual << 1;
                if (codigoAtual == '1')
                    byteAtual = byteAtual | 1;

                bitsEscritos++;

                if (bitsEscritos == 8) {
                    fwrite(&byteAtual, sizeof(unsigned char), 1, arquivoDestino);
                    bitsEscritos = 0;
                    byteAtual = 0;
                }
            }
        }
    } while (bytesLidos == 1024);
    if (bitsEscritos != 0) {
        fwrite(&byteAtual, sizeof(unsigned char), 1, arquivoDestino);
    }

    fclose(arquivo);
    fclose(arquivoDestino);


    printf("Arquivo destino: %s\n", nomeArquivoDestino);
    return true;
}

bool descompactar(char *nomeArquivo) {
    FILE *arquivoCompactado = fopen(nomeArquivo, "rb");
    if (arquivoCompactado == NULL) {
        printf("Arquivo não encontrado!\n");
        return false;
    }

    char nomeArquivoDestino[1000];
    sprintf(nomeArquivoDestino, "%s.descompactado", nomeArquivo);

    FILE *arquivoDestino = fopen(nomeArquivoDestino, "wb");
    if (arquivoDestino == NULL) {
        printf("Erro ao criar arquivo destino!\n");
        return false;
    }

    unsigned char bit;
    unsigned char buffer;
    int bitsRestantes = 0;

    No *arvore = decodificarArvore(arquivoCompactado);

    No* noAtual = arvore;
    while (true) {
        if (bitsRestantes == 0) {
            buffer = fgetc(arquivoCompactado);
            if (feof(arquivoCompactado)) {
                break;
            }
            bitsRestantes = 8;
        }
        
        bit = (buffer >> (bitsRestantes - 1)) & 1;
        bitsRestantes--;

        if (bit == 0) {
            noAtual = noAtual->esquerda;
        } else {
            noAtual = noAtual->direita;
        }

        if (noAtual->esquerda == NULL && noAtual->direita == NULL) {
            fputc(noAtual->caracter, arquivoDestino);
            noAtual = arvore;
        }
    }

    fclose(arquivoCompactado);
    fclose(arquivoDestino);

    return true;
}

void codificarArvore(No* no, FILE* arquivo) {
    if (no == NULL) {
        return;
    }

    if (no->esquerda == NULL && no->direita == NULL) {
        fputc('1', arquivo);
        fputc(no->caracter, arquivo);
        return;
    }

    fputc('0', arquivo);

    codificarArvore(no->esquerda, arquivo);
    codificarArvore(no->direita, arquivo);
}

No* decodificarArvore(FILE* arquivo) {
    unsigned char bit = fgetc(arquivo);
    No* no = (No*) malloc(sizeof(No));
    if (no == NULL) {
        printf("Erro ao alocar memória para o nó!\n");
        return NULL;
    }

    if (bit == '1') {
        no->caracter = fgetc(arquivo);
        no->esquerda = NULL;
        no->direita = NULL;
        return no;
    } else if (bit == '0') {
        no->esquerda = decodificarArvore(arquivo);
        no->direita = decodificarArvore(arquivo);
        return no;
    }

    return NULL;
}

void gerarCodigos(No* no, unsigned char codigo[256][256], unsigned int tamanhoCodigo[256], unsigned int profundidade, unsigned int valor) {
    if (no->esquerda == NULL && no->direita == NULL) {
        for (unsigned int i = 0; i < profundidade; i++) {
            unsigned int codigoAtual = ((valor >> (profundidade - i - 1)) & 1);
            codigo[no->caracter][i] = codigoAtual == 0 ? '0' : '1';
            tamanhoCodigo[no->caracter]++;
        }

        return;
    }

    if (no->esquerda != NULL) {
        gerarCodigos(no->esquerda, codigo, tamanhoCodigo, profundidade + 1, valor << 1);
    }

    if (no->direita != NULL) {
        gerarCodigos(no->direita, codigo, tamanhoCodigo, profundidade + 1, (valor << 1) | 1);
    }
}
