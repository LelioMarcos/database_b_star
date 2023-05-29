#pragma once

#define OK 0
#define ERRO 0

/*
    Função para criar um arquivo binário com as informações de um arquivo csv.

    Parâmetros:
        csv_file: nome do arquivo csv de entrada;
        binary_file: nome do arquivo binário de saída.

    Retorno: 0 se tudo ocorrer bem. Senão, retorna 1; 
*/
int create_table(char *csv_file, char *binary_file);

/*
    Função que percorre sequencialmente um arquivo arquivo binário
    e lista todos os crimes registrados.

    Parâmetros:
        binary_file: nome do arquivo binário

*/
void selecte(char *binary_file);

/*
    Função que cria um arquivo de índices a partir de um arquivo binário original
    e do campo que será indexado.

    Parâmetro:
        binary_file: nome do arquivo binário 

*/
int indexa(char *binary_file, char campo_indexado[20], char tipo_dado[20], char arquivo_index[100]);

/*
    Função que realiza as operações repetitivas das funcionalidades 4 a 7 e seleciona uma 
    destas funcionalidades em específico.
    
    Parâmetros:
        binary_file: Nome do arquivo binário
        campo_indexado: Campo que será/foi indexado
        tipo_dado: Tipo do dado usado (inteiro ou string)
        index_file: nome 
        n: Quantidade de buscas que serão realizadas
        func: Inteiro correspondente ao número da funcionalidade que será chamada
*/
int funcionalidades_index(char *binary_file, char campo_indexado[20], char tipo_dado[20], char index_file[100], int n, int func);









