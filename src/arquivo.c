#include "../header/arquivo.h"
#include "../header/crime.h"

#include <stdlib.h>
#include <stdio.h>

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/

struct header {
    char status;
    long int proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};



/*
    Cria um cabeçalho com os valores iniciais.

    Retorno: o cabeçalho criado, caso dê certo. Senão, retorna nulo. 
*/
header_t *criar_cabecalho() {
    header_t* header = (header_t*)malloc(sizeof(header_t));

    if (header == NULL) {
        return NULL;
    }

    header->status = '0';
    header->proxByteOffset = 0;
    header->nroRegArq = 0;
    header->nroRegRem = 0;

    return header;
}

void ler_cabecalho(FILE* binfile, header_t* cabecalho) {
//Lendo o cabeçalho do arquivo
    fread(&(cabecalho->status), sizeof(char), 1, binfile);
    fread(&(cabecalho->proxByteOffset), sizeof(long int), 1, binfile);
    fread(&(cabecalho->nroRegArq), sizeof(int), 1, binfile);
    fread(&(cabecalho->nroRegRem), sizeof(int), 1, binfile);
}

/*
    Escreve o cabeçalho no início do arquivo binário.

    Parâmetros: 
        cabecalho: struct do cabeçalho do arquivo;
        file: ponteiro de escrita do arquivo;
*/
void escrever_cabecalho(header_t *cabecalho, FILE *file) {
    if (ftell(file) != 0) fseek(file, 0, SEEK_SET);
    
    fwrite(&(cabecalho->status), sizeof(char), 1, file);
    fwrite(&(cabecalho->proxByteOffset), sizeof(long int), 1, file);
    fwrite(&(cabecalho->nroRegArq), sizeof(int), 1, file);
    fwrite(&(cabecalho->nroRegRem), sizeof(int), 1, file);
}

/*
    Abre o arquivo binário para escrita.
    
    Parâmetros: 
        cabecalho: struct do cabeçalho do arquivo;
        nome_arquivo: string do nome do arqquivo binário que será criado;

    Retorno: o ponteiro para o arquivo, caso consiga criar. Senão, retorna nulo.
*/
FILE *abrir_arquivo_escrita(header_t *cabecalho, char *nome_arquivo) {
    FILE *file = fopen(nome_arquivo, "wb");

    if (file == NULL) {
        return NULL;
    }

    cabecalho->status = '0';                // Muda o status para inconsistente.
    escrever_cabecalho(cabecalho, file);    // Escreve essa mudança no cabeçalho.

    cabecalho->proxByteOffset = ftell(file);
    
    return file;
}

// Abre arquivo sem precisar deletar o conteúdo original.
FILE *abrir_arq_leitura_escrita(header_t* cabecalho, char* nome_arquivo) {
    FILE *file = fopen(nome_arquivo, "rb+");

    if (file == NULL) {
        return NULL;
    }

    ler_cabecalho(file, cabecalho);

    if (cabecalho->status == '1') {
        cabecalho->status = '0';                // Muda o status para inconsistente.
        fseek(file, 0, SEEK_SET);
        escrever_cabecalho(cabecalho, file);    // Escreve essa mudança no cabeçalho.
    } else {
        return NULL;
    }

    return file;
}

/*
    Fecha a escrita do arquivo.

    Parâmetros: 
        file: ponteiro de escrita do arquivo;
        cabecalho: struct do cabeçalho do arquivo;
*/
void fechar_arquivo_escrita(FILE *file, header_t *cabecalho) {
    cabecalho->status = '1';                // Muda o status para consistente.
    escrever_cabecalho(cabecalho, file);    // Escreve essa mudança no arquivo.
    
    fclose(file);
}

void escrever_crime(header_t* cabecalho, FILE *file, crime_t *crime) {
    cabecalho->nroRegArq++;
    crime_arquivo(crime, file);
    fputc('#', file);
    cabecalho->proxByteOffset = ftell(file);
}

void escrever_crime_por_cima(header_t* cabecalho, FILE *file, crime_t *crime) {
    crime_arquivo(crime, file);
}
/*
    Abre o arquivo binário para a leitura e deixa o cursor
    logo após o final do cabeçalho do arquivo

    Parâmetros:
        cabecalho: struct do cabeçalho do arquivo;
        nome_arquivo: string do nome do arquivo que será lido
*/
FILE *abrir_arquivo_binario_leitura(header_t *cabecalho, char *nome_arquivo) {
    FILE *binfile = fopen(nome_arquivo, "rb");
    
    //Arquivo não encontrado/ Não foi possível abrir
    if(binfile == NULL) return NULL;
    
    //Lendo o cabeçalho do arquivo
    fread(&(cabecalho->status), sizeof(char), 1, binfile);
    fread(&(cabecalho->proxByteOffset), sizeof(long int), 1, binfile);
    fread(&(cabecalho->nroRegArq), sizeof(int), 1, binfile);
    fread(&(cabecalho->nroRegRem), sizeof(int), 1, binfile);
    
    //Arquivo inconsistente
    if(cabecalho->status=='0') return NULL;
    

    return binfile;
}

void deleta_e_escreve(FILE* arq, header_t* cabecalho, crime_t* crime, long int *byteoffset) {
    fputc('1', arq);

    cabecalho->nroRegRem++;                
    *byteoffset = cabecalho->proxByteOffset;
    
    fseek(arq, cabecalho->proxByteOffset, SEEK_SET);

    escrever_crime(cabecalho, arq, crime);
}

/*
    Dealoca o cabeçalho criado.

    Parâmetros: 
        cabecalho: ponteiro duplo da struct do cabeçalho do arquivo.
*/
void destruir_cabecalho(header_t **cabecalho) {
    free(*cabecalho);
    *cabecalho = NULL; // O ponteiro duplo serve para mudar a variável para nulo.
}

/*
    Retorna o número de registros de um arquivo com base no
    registro de cabeçalho.
    Parâmetros:
        cabecalho: Ponteiro do cabeçalho do arquivo
*/
int retorna_nroRegArq_cabecalho(header_t *cabecalho){
    return cabecalho -> nroRegArq;
}

int retorna_nroRegRem_cabecalho(header_t *cabecalho){
    return cabecalho -> nroRegRem;
}

int retorna_proxByteOffset_cabecalho(header_t *cabecalho){
    return cabecalho -> proxByteOffset;
}

void aumenta_regRem(header_t *cabecalho){
    cabecalho->nroRegRem++;
}