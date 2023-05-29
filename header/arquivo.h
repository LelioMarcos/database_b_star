#pragma once

#include <stdio.h> 
#include <stdlib.h> 
#include "crime.h" 

typedef struct header header_t;

void deleta_e_escreve(FILE* arq, header_t* cabecalho, crime_t* crime, long int *byteoffset);
void escrever_crime_por_cima(header_t* cabecalho, FILE *file, crime_t *crime);
FILE *abrir_arq_leitura_escrita(header_t* cabecalho, char* nome_arquivo);

void ler_cabecalho(FILE* binfile, header_t* cabecalho);
/*
    Cria um cabeçalho com os valores iniciais.

    Retorno: o cabeçalho criado, caso dê certo. Senão, retorna nulo. 
*/
header_t* criar_cabecalho();

/*
    Abre o arquivo binário para escrita.
    
    Parâmetros: 
        cabecalho: struct do cabeçalho do arquivo;
        nome_arquivo: string do nome do arqquivo binário que será criado;

    Retorno: o ponteiro para o arquivo, caso consiga criar. Senão, retorna nulo.
*/
FILE *abrir_arquivo_escrita(header_t *cabecalho, char *nome_arquivo);

/*
    Fecha a escrita do arquivo.

    Parâmetros: 
        file: ponteiro de escrita do arquivo;
        cabecalho: struct do cabeçalho do arquivo;
*/
void fechar_arquivo_escrita(FILE *file, header_t *cabecalho);

/*
    Escreve um crime no arquivo binário.

    Parâmetros: 
        cabecalho: struct do cabeçalho do arquivo;
        file: ponteiro de escrita do arquivo;
        crime: struct do crime que será escrito.
*/
void escrever_crime(header_t* cabecalho, FILE *file, crime_t *crime);

/*
    Abre o arquivo binário para a leitura e deixa o cursor
    logo após o final do cabeçalho do arquivo

    Parâmetros:
        cabecalho: struct do cabeçalho do arquivo;
        nome_arquivo: string do nome do arquivo que será lido
*/
FILE *abrir_arquivo_binario_leitura(header_t *cabecalho, char *nome_arquivo);


/*
    Dealoca o cabeçalho criado.

    Parâmetros: 
        cabecalho: ponteiro duplo da struct do cabeçalho do arquivo;
*/
void destruir_cabecalho(header_t **cabecalho);

/*
    Retorna o número de registros de um arquivo com base no
    registro de cabeçalho.
    
    Parâmetros:
        cabecalho: Ponteiro do cabeçalho do arquivo
*/
int retorna_nroRegArq_cabecalho(header_t *cabecalho);

int retorna_nroRegRem_cabecalho(header_t *cabecalho);
int retorna_proxByteOffset_cabecalho(header_t *cabecalho);

void aumenta_regRem(header_t *cabecalho);