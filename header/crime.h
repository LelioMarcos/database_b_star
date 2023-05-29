#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct crime crime_t;


crime_t* criar_crime();

void set_status(crime_t* crime, char status);
int tamanho_crime(crime_t* crime);
void atualizar_campo_int(crime_t *crime, char campo[], int num);
void atualizar_campo_str(crime_t *crime, char campo[], char* str);

void retorna_valor_campo(crime_t* crime, char campo[20], int *num, char **str);

/*
void atualizar_campo(crime_t *crime, char campo[]);
    Função para criar a estrutura Crime a partir de uma linha do csv.

    Parâmetros:
        linha: um array de strings com os campos de uma linha do arquivo csv.

    Retorno: A estrutura Crime, caso dê certo. Senão, retorna NULL.
*/
crime_t *criar_crime_de_csv(char **linha);

/*
    Função para colocar o limitador no final de uma string.

    Parâmetros:
        string: string que deseja colocar o limitador.

    Retorno: a string com o limitador.
*/
void print_crime(crime_t *crime);

/*
    Dealocar a struct crime.

    Parâmetros:
        crime: o ponteiro duplo do crime que deseja dealocar.    
*/
void destruir_crime(crime_t **crime);

/*
    Escreve os campos de Crime no arquivo binário.

    Parâmetros:
        crime: a struct do crime que deseja escrever no arquivo.
        arq: o ponteiro de escrita do arquivo binário.
*/
void crime_arquivo(crime_t* crime, FILE *arq);

/*
    Realiza a leitura de um crime escrito no arquivo
    binário.

    Parâmetro:
        arqbin: ponteiro para o arquivo binário
*/
//crime_t *leitura_crime_de_binario(FILE *arqbin);
crime_t *leitura_crime_de_binario(FILE *arqbin, int *tam);
/*
    Cria um ponteiro de registro crime. É utilizada nas funções
    relacionadas à leitura do binário.
    
    Parâmetro:
*/
crime_t *criar_crime_bin();

/*
    Após a leitura do binário, lista todos os campos de forma
    organizada.

    Parâmetros:
        crime: Ponteiro do registro crime que será listado
*/
void printa_crime_bin(crime_t *crime);


/*
    Retorna o valor do campo do registro fornecido como parâmetro, independente
    de ser int ou string

    Parâmetros:
        crime: Ponteiro do registro de crime
        campo: Campo do registro cujo valor será retornado
        num: Ponteiro da variável que será utilizada para retornar o valor
        caso seja do tipo int
        str: Ponteiro de ponteiro que será utilizado para retornar a string
        caso seja do tipo string

*/
void retorna_valor_campo(crime_t* crime, char campo[20], int *num, char **str);


/*
    Retorna 1 se o registro do crime tiver sido logicamente removido.
    Caso contrário, retorna 0.
    
    Parâmetros:
        *crime: Ponteiro do crime


*/
int crime_foi_removido(crime_t *crime);

/*
    Retorna o id do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna 0
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
int retorna_idCrime(crime_t *crime);


/*
    Retorna a data do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_dataCrime(crime_t *crime);
/*
    Retorna o numero do artigo do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna 0
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
int retorna_numeroArtigo(crime_t *crime);

/*
    Retorna a marca do celular do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_marcaCelular(crime_t *crime);

/*
    Retorna o lugar do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_lugarCrime(crime_t *crime);
/*
    Retorna a descrição do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_descricaoCrime(crime_t *crime);


/*
    Atualiza um registro do arquivo de dados caso o campo seja
    do tipo int

    Parâmetros:
        crime -> Ponteiro do registro de dados (crime)
        campo -> Campo que será atualizado (idCrime ou numeroArtigo)
        num -> Valor que substituirá o antigo

*/
void atualizar_campo_int(crime_t *crime, char campo[], int num);

/*
    Atualiza um registro do arquivo de dados caso o campo seja
    do tipo string

    Parâmetros:
        crime -> Ponteiro do registro de dados (crime)
        campo -> Campo que será atualizado (marcaCelular, dataCrime, lugarCrime
        ou descricaoCrime)
        str -> String que substituirá a antiga


*/
void atualizar_campo_str(crime_t *crime, char campo[], char* str);


/*
    'Deleta' o registro de crime no arquivo binário
    marcando o campo 'removido' como 1
    Parâmetros:
        arq_bin -> Ponteiro do arquivo binário
        byteoffset -> ByteOffset do registro no arquivo

*/
void deleta_crime_bin(FILE *arq_bin, long int byteoffset);