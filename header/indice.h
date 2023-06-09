#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct header_indice header_indice_t;
typedef struct no no_t;

#define LEITURA 0
#define ESCRITA 1

int busca_indice(FILE* arq_indice, header_indice_t *header_indice, int item);
/*
    Cria cabeçalho de arquivo de índice.
    Parâmetro:
*/
header_indice_t *cria_cabecalhoIndice();


/*

    Destrói o cabeçalho de arquivo de índice.
    Parâmetro: 
        cabecalho_indicie -> Ponteiro de ponteiro do cabeçalho

*/
void destruir_cabecalhoIndice(header_indice_t** cabecalho_indicie);

/*

    Atualiza o cabeçalho do arquivo de índices com seus valores modificados
    Parâmetro:
        arq -> ponteiro arquivo de índices
        cabecalho -> cabeçalho do arquivo de índices

*/
void atualiza_cabecalhoIndice(FILE *arq, header_indice_t *cabecalho);


/*

    Cria um registro de arquivo de índices (inteiro)
    Parâmetro:

*/
indice_inteiro_t *cria_indice_inteiro();


/*

    Cria um registro de arquivo de índices (string)
        Parâmetro:

*/
indice_string_t *cria_indice_string();


/*
    Abre o arquivo de índices de forma que os registros de índice
    possam ser escritos.
    Parâmetros:
        nomeArquivo -> Nome do arquivo de índices
        cabecalho -> cabeçalho de índice de registros

*/
FILE *abreArquivoIndice_escrita(char *nomeArquivo, header_indice_t *cabecalho);


/*
    Fecha o arquivo de índice e atualiza seu cabeçalho.
    Parâmetros:
        file -> arquivo de índices
        cabecalho -> cabeçalho do arquivo de índices

*/
void fechar_arquivoIndice(FILE *file, header_indice_t *cabecalho);


/*
    Cria um vetor de registros escolhidos de acordo com o campo indexado (int)
    Parâmetros:

*/
indice_inteiro_t **cria_vetorIndices_inteiro();


/*
    Cria um vetor de registros escolhidos de acordo com o campo indexado (string)
    Parâmetros:
        
*/
indice_string_t **cria_vetorIndices_strings();


/*
    Compara dois registros escolhidos de acordo com o campo indexado (int)
    Parâmetros:
        registroA -> Primeiro registro a ser comparado
        registroB -> Segundo registro a ser comparado 

*/
int compararIndiceInteiros(const void* registroA, const void* registroB);


/*
    Compara dois registros escolhidos de acordo com o campo indexado (string)
    Parâmetros:
        registroA -> Primeiro registro a ser comparado
        registroB -> Segundo registro a ser comparado 

*/
int compararIndiceStrings(const void *registroA, const void *registroB);


/*
    Lê o binário original, filtra de acordo com o 
    campo indexado (tipo inteiro), calcula o byteoffset de cada registro selecionado, 
    ordena e escreve no arquivo de índices.
    Parâmetro: 
        header -> cabeçalho do arquivo de índice
        arq_binario -> Arquivo de onde serão lidos os registros
        arq_index -> Arquivo onde serão escritos os índices
        campo_indexado -> Campo que será utilizado para criar o arquivo de índice (apenas inteiros)

*/
void escreveIndice_inteiros(header_indice_t *header, FILE *arq_binario, FILE *arq_index, char *campo_indexado);


/*
    Lê o binário original, filtra de acordo com o 
    campo indexado (tipo string), calcula o byteoffset de cada registro selecionado, 
    ordena e escreve no arquivo de índices.
    Parâmetro: 
        header -> cabeçalho do arquivo de índice
        arq_binario -> Arquivo de onde serão lidos os registros
        arq_index -> Arquivo onde serão escritos os índices
        campo_indexado -> Campo que será utilizado para criar o arquivo de índice (apenas strings)

*/
void escreveIndice_strings(header_indice_t *header, FILE *arq_binario, FILE *arq_index, char *campo_indexado);


/*
    Lê todos os registros do arquivo de indice (int) e salva em um vetor.
    Parâmetro:
        arq_index->Ponteiro do arquivo de índices

*/
indice_inteiro_t **leitura_indicesInteiros(FILE *arq_index, header_indice_t *cabecalho_indicie);


/*
    Lê todos os registros do arquivo de indice (string) e salva em um vetor.
    Parâmetro:
        arq_index->Ponteiro do arquivo de índices

*/
indice_string_t **leitura_indicesStrings(FILE *arq_index, header_indice_t* cabeaclho_indicie);


/*
    Compara uma chave fornecida com a chave de busca de um registro de índices (tipo inteiro).
    Retorna o byteoffset se forem iguais e -1 se não forem.
    Parâmetros:
        ind_int -> Registro de índice de inteiro
        num -> Chave a ser comparada
*/
long int compara_indicie_inteiro(indice_inteiro_t* ind_int, int num);


/*
    
    Compara uma chave fornecida com a chave de busca de um registro de índices (tipo string).
    Retorna o byteoffset se forem iguais e -1 se não forem.
    Parâmetros:
        ind_str -> Registro de índice de string
        num -> Chave a ser comparada
*/
long int compara_indicie_string(indice_string_t* ind_str, char *str);


/*

    Retorna a quantidade de índices salva em um cabeçalho de arquivo de índice
        Parâmetros:
            cabecalho_indicie -> Cabeçalho do arquivo de índice

*/
int retorna_quant_indicies(header_indice_t* cabecalho_indicie);

/*
    Escreve no arquivo de índices a partir de um vetor já preenchido, seja ele do tipo inteiro
    ou de strings.
    Parâmetros:
        arq_index -> Ponteiro do arquivo de indíce
        vetor_int -> Vetor de registros de índice (inteiro)
        vetor_str -> Vetor de registros de índice (string)
        cabecalho_indicie -> Ponteiro do cabeçalho do arquivo de índice
    
*/
void escreve_indice(FILE* arq_index, indice_inteiro_t **vetor_int, indice_string_t** vetor_str, header_indice_t* cabecalho_indice);

/*

    Insere um novo registro de índices em um vetor já preenchido
    Parâmetros:
        ind_int -> Vetor de registros de índice (inteiro)
        cabecalho_indice -> Cabeçalho do arquivo de índice
        num -> Valor da chave de busca do registro a ser inserido
        byteoffset -> Próximo byteoffset livre no arquivo de índice


*/
void adicionar_indice_inteiro(indice_inteiro_t*** ind_int, header_indice_t* cabecalho_indicie, int num, long int byteoffset);

/*
    Desaloca o vetor de índices.
    Parâmetros:
        ind_int -> Vetor de registros de índice (inteiro)
        ind_str -> Vetor de registros de índice (inteiro)
        cabecalho -> Cabeçalho do arquivo de índice
*/
void destruir_indices(indice_inteiro_t** ind_int, indice_string_t** ind_str, header_indice_t* cabecalho_indice);
 

/*

    Insere um novo registro de índices em um vetor já preenchido
    Parâmetros:
        ind_int -> Vetor de registros de índice (string)
        cabecalho_indice -> Cabeçalho do arquivo de índice
        str -> Valor da chave de busca do registro a ser inserido
        byteoffset -> Próximo byteoffset livre no arquivo de índice


*/
void adicionar_indice_string(indice_string_t*** ind_str, header_indice_t* cabecalho_indicie, char *str, long int byteoffset);

/*
    Dado um byteoffset, busca e retorna a posição de um registro em um vetor de registro 
    de índice (inteiro ou string)
    Parâmetro:
        vet_int -> Vetor de registro de índice (inteiro)
        vet_str -> Vetor de registro de índice (string)
        cabecalho_indice -> Cabeçalho do arquivo de índice 
        chave -> byteoffset do registro

*/

int buscaPosicao_indice(indice_inteiro_t **vet_int, indice_string_t **vet_str, int tam_vetor, int chaveInt, char *chaveStr, int long byteoffset);


int buscabin_int(indice_inteiro_t **vet_int, int tam_vetor, int chave);

int buscabin_str(indice_string_t **vet_str, int tam_vetor, char *chave);


/*
    Deleta o registro do arquivo de índice de forma definitiva
    Parâmetros:
        vetor_int -> Vetor de registros de índice (inteiros)
        vetor_str -> Vetor de registros de índice (strings)
        byteoffset -> ByteOffset do registro no arquivo de índice
        cabecalho -> Cabeçalho do arquivo de índice

*/
void deleta_crimeIndice(indice_inteiro_t ***vetor_int, indice_string_t ***vetor_str, int chaveInt, char *chaveStr, long int byteoffset, char *campo_indexado, header_indice_t *cabecalho);
/*
    Deleta o registro do arquivo de índice de forma definitiva
    Parâmetros:
        vetor_int -> Vetor de registros de índice (inteiros)
        vetor_str -> Vetor de registros de índice (strings)
        byteoffset -> ByteOffset do registro no arquivo de índice
        cabecalho -> Cabeçalho do arquivo de índice

*/

/*
    Atualiza o arquivo de índice com os registros alterados
    Parâmetros:
        ind_int -> Vetor de registros de índice (inteiro)
        ind_str -> Vetor de registros de índice (inteiro)
        cabecalho -> Cabeçalho do arquivo de índice
        num -> Conteúdo da chave de busca do registro a ser alterado, caso o registro seja de inteiro
        str -> Conteúdo da chave de busca do registro a ser alterado, caso o registro seja de string
        byteoffset_antigo -> Byteoffset antigo do registro 
        byteoffset_novo  -> Byteoffset novo


*/
void atualiza_indice(indice_inteiro_t*** ind_int, indice_string_t*** ind_str, header_indice_t* cabecalho_indice, int num_antigo, char *str_antigo, int num, char *str, long int byteoffset_antigo, long int byteoffset);