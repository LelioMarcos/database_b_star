#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/csv.h"
#include "../header/readline.h"

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/


/*
    Função para ler o arquivo uma linha do arquivo CSV

    Parêmtros: 
        file: ponteiro de leitura do arquivo csv.
        colunas: ponteiro que receberá a quantidade de colunas da linha do csv.

    Retorno: um array de strings com os campos da linha do csv.
*/
char** csv_ler_linha(FILE *file, int *colunas) {
    *colunas = 0;

    if (file == NULL) {
        return NULL;
    }

    char **linha = NULL;

    char *read = readline(file);
    char *token; 

    // Separando a string pelo ",".
    while ((token = strsep(&read, ",")) != NULL) {
        (*colunas)++;
        linha = realloc(linha, (*colunas)*sizeof(char*));
        linha[*colunas - 1] = token;
    }

    free(read);

    // Verificar se a linha é vazia.
    if (*colunas == 1 && linha[0][0] == 0) {
        destruir_linha(linha);
        linha = NULL;
        *colunas = 0;
    }

    return linha;
}

/*
    Função para dealocar o array de strings do csv.

    Parâmetros:
        linha: array de strings dos campos da linha do csv.
*/
void destruir_linha(char **linha) {
    free(linha[0]);
    free(linha);
}