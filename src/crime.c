#include "../header/crime.h"
#include "../header/funcoesFornecidas.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/

struct crime {
    char removido;
    int idCrime;
    char dataCrime[11];
    int numeroArtigo;
    char marcaCelular[13];
    char *lugarCrime;
    char *descricaoCrime;
};

crime_t* criar_crime() {
    crime_t *crime = (crime_t*)malloc(sizeof(crime_t));

    if (crime == NULL) {
        return NULL;
    }

    char numero_artigo[5];

    crime->removido = '0';

    scanf(" %d", &(crime->idCrime)); 
    scan_quote_string(crime->dataCrime);
    scanf(" %s", numero_artigo);

    if (strcmp(numero_artigo, "NULO") == 0) {
        crime->numeroArtigo = -1;
    } else {
        crime->numeroArtigo = atoi(numero_artigo);
    }

    crime->lugarCrime = (char*)malloc(100 * sizeof(char));
    crime->descricaoCrime = (char*)malloc(100 * sizeof(char));
    scan_quote_string(crime->lugarCrime);
    scan_quote_string(crime->descricaoCrime);
    scan_quote_string(crime->marcaCelular);

    return crime;
}

void set_status(crime_t* crime, char status) {
    crime->removido = status;
}

int tamanho_crime(crime_t* crime) {
    long int ftell = 33 + strlen(crime->lugarCrime) + strlen(crime->descricaoCrime);
    return ftell;
}

/*
    Função para criar a estrutura Crime a partir de uma linha do csv.

    Parâmetros:
        linha: um array de strings com os campos de uma linha do arquivo csv.

    Retorno: A estrutura Crime, caso dê certo. Senão, retorna NULL.
*/
crime_t *criar_crime_de_csv(char **linha) {
    crime_t *crime = (crime_t*)malloc(sizeof(crime_t));
    
    if (crime == NULL) {
        return NULL;
    }

    crime->removido = '0';
    crime->idCrime = atoi(linha[0]); 
    strcpy(crime->dataCrime,linha[1]);
    crime->numeroArtigo = linha[2][0] == 0 ? -1 : atoi(linha[2]);
    strcpy(crime->marcaCelular,linha[5]);

    // Como lugarCrime e dscricaoCrime são campos de tamanhos variados, devemos
    // primeiro alocar uma string com o tamanho do campo para, então, atribuir
    // o valor desse campo na struct.
    crime->lugarCrime = (char*)malloc(strlen(linha[3]) * sizeof(char) + 1);
    strcpy(crime->lugarCrime,linha[3]);
    
    crime->descricaoCrime = (char*)malloc(strlen(linha[4]) * sizeof(char) + 1);
    strcpy(crime->descricaoCrime,linha[4]);

    return crime;
}

void print_crime(crime_t *crime) {
    printf("%d\n", crime->removido);
    printf("%d\n", crime->idCrime);
    printf("%s\n", crime->dataCrime);
    printf("%d\n", crime->numeroArtigo);
    printf("%s\n", crime->marcaCelular);
    printf("%s\n", crime->lugarCrime);
    printf("%s\n", crime->descricaoCrime);
    printf("\n");
}

/*
    Função para colocar o limitador no final de uma string.

    Parâmetros:
        string: string que deseja colocar o limitador.

    Retorno: a string com o limitador.
*/
char *colocar_limitador(char *string) {
    char *string_limitada = (char*)malloc(sizeof(char) * strlen(string) + 1);
    strcpy(string_limitada, string);
    
    string_limitada[strlen(string_limitada)] = '|';
    
    return string_limitada;
}

/*
    Escreve os campos de Crime no arquivo binário.

    Parâmetros:
        crime: a struct do crime que deseja escrever no arquivo.
        arq: o ponteiro de escrita do arquivo binário.
*/
void crime_arquivo(crime_t* crime, FILE *arq) {
    // Escrever os campos fixos
    fwrite(&(crime->removido), sizeof(char), 1, arq);
    fwrite(&(crime->idCrime), sizeof(int), 1, arq);
    fwrite(&(crime->dataCrime), sizeof(char), 10, arq);

    fwrite(&(crime->numeroArtigo), sizeof(int), 1, arq);


    // Preencher os bytes de lixo com o caracter '$'
    char marca_celular[12];
    int len = strlen(crime->marcaCelular);
    for (int i = 0; i < len; i++) {
        marca_celular[i] = crime->marcaCelular[i];
    }
    for (int i = 0; i < 12 - len; i++) {
        marca_celular[i + len] = '$';
    }

    fwrite(&(marca_celular), sizeof(char), 12, arq);

    // Escrever os campos de tamanho variado.    
    char *lugar_crime = colocar_limitador(crime->lugarCrime);
    fwrite(lugar_crime, sizeof(char), strlen(crime->lugarCrime) + 1, arq);
    free(lugar_crime);

    char *descricao = colocar_limitador(crime->descricaoCrime);
    fwrite(descricao, sizeof(char), strlen(crime->descricaoCrime) + 1, arq);
    free(descricao);

    // Terminador do registro.
}

/*
    Dealocar a struct crime.

    Parâmetros:
        crime: o ponteiro duplo do crime que deseja dealocar.    
*/
void destruir_crime(crime_t **crime) {
    free((*crime)->lugarCrime);
    free((*crime)->descricaoCrime);
    free(*crime);
    *crime = NULL;
}

/*
    Realiza a leitura de um crime escrito no arquivo
    binário.
    
    Parâmetro:
        arqbin: ponteiro para o arquivo binário

*/
crime_t *leitura_crime_de_binario(FILE *arqbin, int *tam) {
    crime_t *crime = (crime_t*)malloc(sizeof(crime_t));
    //*tam = 0;

    //Lendo os campos de tamanho fixo
    fread(&(crime->removido), sizeof(char), 1, arqbin);
    fread(&(crime->idCrime), sizeof(int), 1, arqbin);
    fread(&(crime->dataCrime), sizeof(char), 10, arqbin);
    crime->dataCrime[10] = '\0';

    fread(&(crime->numeroArtigo), sizeof(int), 1, arqbin);
    
    //Lendo o campo marcaCelular (é de tamanho fixo, mas fez-se
    //necessário um tratamento durante a leitura devido a presença de $)
    char *marca_celular = NULL;
    char c = fgetc(arqbin);
    int n = 0;
    while(c != '$'){
        marca_celular = realloc(marca_celular, (n+1) * sizeof(char));
        marca_celular[n] = c;
        n++;
        c = fgetc(arqbin);
    }
    marca_celular = realloc(marca_celular, (n+1) * sizeof(char));
    marca_celular[n] = '\0'; //Adicionamos um \0 no final da string
    strcpy(crime->marcaCelular, marca_celular);//para facilitar o print deste campo futuramente

    free(marca_celular);
    *tam = 31; // quantidade de bytes dos campos fixos

    //É feito um fseek para "pularmos" para o próximo campo (depois dos $)
    //O 12 é proveniente do tamanho fixo do campo marcaCelular
    int distancia_fseek = 12 - n;
    fseek(arqbin, distancia_fseek - 1, SEEK_CUR);
    
    //Lendo o campo lugarCrime
    char *lugar_crime = NULL;
    c = fgetc(arqbin);
    n = 0;
    while(c != '|'){
        lugar_crime = realloc(lugar_crime, (n+1) * sizeof(char));
        lugar_crime[n] = c;
        n++;
        c = fgetc(arqbin);
    }
    lugar_crime = realloc(lugar_crime, (n+1) * sizeof(char));
    lugar_crime[n] = '\0'; //O \0 é adicionado aqui com o mesmo propósito
    crime -> lugarCrime = lugar_crime; //presente na leitura de marcaCelular.
    *tam += n + 1;                            
    
    //Lendo o campo descricaoCrime
    char *descricao_crime = NULL;
    c = fgetc(arqbin);    
    n = 0;
    while(c != '|'){
        if(c != '$'){
            descricao_crime = realloc(descricao_crime, (n+1) * sizeof(char));
            descricao_crime[n] = c;
            n++;
        }
        c = fgetc(arqbin);
        
    }
    descricao_crime = realloc(descricao_crime, (n+1) * sizeof(char));
    descricao_crime[n] = '\0'; //O \0 é adicionado aqui com o mesmo propósito
                                //presente na leitura de marcaCelular.
    crime -> descricaoCrime = descricao_crime;
    *tam += n + 1;                             
    
    do {
        c = fgetc(arqbin);
        (*tam)++;
    } while (c != '#');

    return crime;
}

/*
    Cria um ponteiro de registro crime. É utilizada nas funções 
    relacionadas à leitura do binário.
    Parâmetro:

*/
crime_t *criar_crime_bin(){

    crime_t *crime = (crime_t*)malloc(sizeof(crime_t));
    return crime;

}

/*
    Após a leitura do binário, lista todos os campos de forma
    organizada.
    
    Parâmetros:
        crime: Ponteiro do registro crime que será listado
*/
void printa_crime_bin(crime_t *crime){

    //idCrime
    printf("%d, ", crime->idCrime);
    
    //dataCrime
    printf("%s, ", crime->dataCrime);
    
    //numeroArtigo
    if(crime->numeroArtigo != -1)printf("%d, ", crime->numeroArtigo);
    else printf("NULO, ");

    //lugarCrime
    //O \0 adicionado durante a leitura permite uma verificação para determinar
    //se o campo é vazio. O mesmo se repete na leitura de descricaoCrime e marcaCelular.
    if(crime -> lugarCrime[0] != '\0'){
        printf("%s, ", crime->lugarCrime);
    }
    else printf("NULO, ");
    

    //descricaoCrime
    if(crime -> descricaoCrime[0] != '\0'){
        printf("%s, ", crime->descricaoCrime);
    }
    else printf("NULO, ");
    
    //marcaCelular
    if(crime->marcaCelular[0] != '\0'){
        printf("%s\n", crime->marcaCelular);
    }
    else printf("NULO\n");

}

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
void retorna_valor_campo(crime_t* crime, char campo[20], int *num, char **str) {
    if (strcmp(campo, "idCrime") == 0) {
        *num = crime->idCrime;
    } else if (strcmp(campo, "dataCrime") == 0) {
        *str = (char*)malloc(11 * sizeof(char));
        strcpy(*str, crime->dataCrime);
    } else if (strcmp(campo, "numeroArtigo") == 0) {
        *num = crime->numeroArtigo;
    } else if (strcmp(campo, "marcaCelular") == 0) {
        *str = (char*)malloc(13 * sizeof(char));
        strcpy(*str, crime->marcaCelular);
    } else if (strcmp(campo, "lugarCrime") == 0) {
        *str = (char*)malloc((strlen(crime->lugarCrime) + 1) * sizeof(char));
        strcpy(*str, crime->lugarCrime);
    } else if (strcmp(campo, "descricaoCrime") == 0) {
        *str = (char*)malloc((strlen(crime->descricaoCrime) + 1) * sizeof(char));
        strcpy(*str, crime->descricaoCrime);
    }
}

/*
    Retorna 1 se o registro do crime tiver sido logicamente removido.
    Caso contrário, retorna 0.
    
    Parâmetros:
        *crime: Ponteiro do crime


*/
int crime_foi_removido(crime_t *crime){
    if(crime->removido == '1') return 1;
    else return 0;

}

/*
    Retorna o id do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna 0
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
int retorna_idCrime(crime_t *crime){
    if(crime_foi_removido(crime) == 0)return crime->idCrime;
    else return 0;
}


/*
    Retorna a data do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_dataCrime(crime_t *crime){
    if(crime_foi_removido(crime) == 0)return crime -> dataCrime;
    else return NULL;
}

/*
    Retorna o numero do artigo do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna 0
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
int retorna_numeroArtigo(crime_t *crime){
    if(crime_foi_removido(crime) == 0){
        return crime->numeroArtigo;
    }


    else return 0;
}

/*
    Retorna a marca do celular do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_marcaCelular(crime_t *crime){
    if(crime_foi_removido(crime) == 0)return crime -> marcaCelular;
    else return NULL;
}

/*
    Retorna o lugar do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_lugarCrime(crime_t *crime){
    return crime -> lugarCrime;
}

/*
    Retorna a descrição do crime em questão se o registro não tiver
    sido removido. Caso contrário, retorna NULL
    
    Parâmetros: 
        *crime: Ponteiro de crime

*/
char *retorna_descricaoCrime(crime_t *crime){
    return crime -> descricaoCrime;
}

/*
    Atualiza um registro do arquivo de dados caso o campo seja
    do tipo int

    Parâmetros:
        crime -> Ponteiro do registro de dados (crime)
        campo -> Campo que será atualizado (idCrime ou numeroArtigo)
        num -> Valor que substituirá o antigo

*/
void atualizar_campo_int(crime_t *crime, char campo[], int num) {
    if (strcmp(campo, "idCrime") == 0) {
        crime->idCrime = num;
    } else if (strcmp(campo, "numeroArtigo") == 0) {
        crime->numeroArtigo = num;
    } 
}

/*
    Atualiza um registro do arquivo de dados caso o campo seja
    do tipo string

    Parâmetros:
        crime -> Ponteiro do registro de dados (crime)
        campo -> Campo que será atualizado (marcaCelular, dataCrime, lugarCrime
        ou descricaoCrime)
        str -> String que substituirá a antiga

*/
void atualizar_campo_str(crime_t *crime, char campo[], char* str) {
    if (strcmp(campo, "dataCrime") == 0) {
        strcpy(crime->dataCrime, str);
    } else if (strcmp(campo, "marcaCelular") == 0) {
        strcpy(crime->marcaCelular, str);
    } else if (strcmp(campo, "lugarCrime") == 0) {
        crime->lugarCrime = realloc(crime->lugarCrime, (strlen(str) + 1) * sizeof(char));
        strcpy(crime->lugarCrime, str);
    } else if (strcmp(campo, "descricaoCrime") == 0) {
        crime->descricaoCrime = realloc(crime->descricaoCrime, (strlen(str) + 1) * sizeof(char));
        strcpy(crime->descricaoCrime, str);
    }
}


/*
    'Deleta' o registro de crime no arquivo binário
    marcando o campo 'removido' como 1
    Parâmetros:
        arq_bin -> Ponteiro do arquivo binário
        byteoffset -> ByteOffset do registro no arquivo

*/
void deleta_crime_bin(FILE *arq_bin, long int byteoffset){
    crime_t *crime = criar_crime_bin();
    //Encontrando o registro no arquivo binario
    fseek(arq_bin, byteoffset, SEEK_SET);


    //A função de busca já retorna uma mensagem de erro caso
    //o registro tenha sido logicamente removido. Prosseguiremos:
    crime->removido = '1';
    fwrite(&(crime->removido), sizeof(char), 1, arq_bin);


}

