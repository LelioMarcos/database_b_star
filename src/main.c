#include <stdio.h>
#include <stdlib.h>
#include "../header/banco.h"
#include "../header/crime.h"
#include "../header/indice.h"
#include "../header/funcoesFornecidas.h"
#include "../header/arquivo.h"

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/

int funcionalidade_3a7(int func) {
    char arquivo_entrada[100];
    scanf("%s", (char *)arquivo_entrada);
    
    char campo_indexado[20];
    scanf(" %s", (char *)campo_indexado);

    char tipo_dado[20];
    scanf(" %s", (char *)tipo_dado);

    char arquivo_index[100];
    scanf(" %s", (char *)arquivo_index);
    
    int n;
    
    if (func != 3 && func != 8) {
        scanf(" %d", &n);
    }

    if (func >= 4 && func <= 7) {
        return funcionalidades_index(arquivo_entrada, campo_indexado, tipo_dado, arquivo_index, n, func);
    } else {
        return indexa(arquivo_entrada, campo_indexado, tipo_dado, arquivo_index);
    }
}

void funcionalidade1() {
    char arquivo_entrada[100];
    char arquivo_saida[100];

    scanf("%s", (char *)arquivo_entrada);
    scanf("%s", (char *)arquivo_saida);
    
    if (create_table(arquivo_entrada, arquivo_saida) != OK) {
        printf("Falha no processamento do arquivo.");
    } else {
        binarioNaTela(arquivo_saida);
    }
}

void funcionalidade2() {
    char arquivo_entrada[100];

    scanf("%s", (char *)arquivo_entrada);

    selecte(arquivo_entrada);
}

int main() {
    header_indice_t *header_indice = criaHeaderIndice();

    //Abre arquivo de índice
    FILE *arq_indice = abrir_arquivo_indice("indice1.bin", header_indice, 2);
    if(arq_indice == NULL){
        printf("Falha no 1processamento do arquivo.\n");
        return ERRO;
    }


    inserir_indice(header_indice, arq_indice, 5, 1);
    inserir_indice(header_indice, arq_indice, 6, 1);
    inserir_indice(header_indice, arq_indice, 7, 1);
    inserir_indice(header_indice, arq_indice, 8, 1);
    inserir_indice(header_indice, arq_indice, 10, 1);
    inserir_indice(header_indice, arq_indice, 14, 1);
    inserir_indice(header_indice, arq_indice, 15, 1);
    inserir_indice(header_indice, arq_indice, 16, 1);
    inserir_indice(header_indice, arq_indice, 17, 1);
    inserir_indice(header_indice, arq_indice, 20, 1);
    inserir_indice(header_indice, arq_indice, 22, 1);
    inserir_indice(header_indice, arq_indice, 25, 1);
    inserir_indice(header_indice, arq_indice, 26, 1);
    inserir_indice(header_indice, arq_indice, 28, 1);
    inserir_indice(header_indice, arq_indice, 30, 1);
    inserir_indice(header_indice, arq_indice, 33, 1);
    inserir_indice(header_indice, arq_indice, 34, 1);
    inserir_indice(header_indice, arq_indice, 36, 1);
    inserir_indice(header_indice, arq_indice, 39, 1);
    inserir_indice(header_indice, arq_indice, 40, 1);
    inserir_indice(header_indice, arq_indice, 44, 1);
    inserir_indice(header_indice, arq_indice, 1, 1);
    inserir_indice(header_indice, arq_indice, 2, 1);
    inserir_indice(header_indice, arq_indice, 23, 1);
    inserir_indice(header_indice, arq_indice, 50, 1);
    inserir_indice(header_indice, arq_indice, 35, 1);
    inserir_indice(header_indice, arq_indice, 37, 1);
    inserir_indice(header_indice, arq_indice, 38, 1);
    inserir_indice(header_indice, arq_indice, 41, 1);
    inserir_indice(header_indice, arq_indice, 42, 1);
    inserir_indice(header_indice, arq_indice, 45, 1);
    inserir_indice(header_indice, arq_indice, 43, 1);
    inserir_indice(header_indice, arq_indice, 47, 1);
    inserir_indice(header_indice, arq_indice, 46, 1);
    inserir_indice(header_indice, arq_indice, 48, 1);
    inserir_indice(header_indice, arq_indice, 49, 1);
    inserir_indice(header_indice, arq_indice, 51, 1);
    inserir_indice(header_indice, arq_indice, 52, 1);


    printar_nos(header_indice, arq_indice);
    fechar_arquivo_indice(arq_indice, header_indice, 2);

    /*
    int funcionalidade;
    scanf("%d", &funcionalidade);

    switch(funcionalidade) {
        case 1:
            funcionalidade1();
            break;
        case 2:
            funcionalidade2();
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            return funcionalidade_3a7(funcionalidade);
            break;
     }

     return 0;
    */
    
}