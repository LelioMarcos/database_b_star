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
    //*
    header_indice_t *header_indice = criaHeaderIndice();

    //Abre arquivo de índice
    FILE *arq_indice = abrir_arquivo_indice("indice1.bin", header_indice, 2);
    if(arq_indice == NULL){
        printf("Falha no 1processamento do arquivo.\n");
        return ERRO;
    }

    for (int i = 0; i < 23; i++) {
        int a = rand() % 10000;
        inserir_indice(header_indice, arq_indice, a, i);
    }

    printar_nos(header_indice, arq_indice);
    fechar_arquivo_indice(arq_indice, header_indice, 2);
    /*/
    
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
    //*/
     return 0;
}