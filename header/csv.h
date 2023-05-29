#pragma once

#include <stdio.h>
#include <stdlib.h>

/*
    Função para ler o arquivo uma linha do arquivo CSV

    Parêmtros: 
        file: ponteiro de leitura do arquivo csv.
        colunas: ponteiro que receberá a quantidade de colunas da linha do csv.

    Retorno: um array de strings com os campos da linha do csv.
*/
char** csv_ler_linha(FILE *f, int *colunas);

/*
    Função para dealocar o array de strings do csv.

    Parâmetros:
        linha: array de strings dos campos da linha do csv.
*/
void destruir_linha(char **linha);