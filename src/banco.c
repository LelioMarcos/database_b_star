#include "../header/banco.h"
#include "../header/csv.h"
#include "../header/indice.h"
#include "../header/funcoesFornecidas.h"
#include "../header/utilidades.h"
#include "../header/arquivo.h"
#include "../header/crime.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define INICIO 0x11
#define INICIOIndices  0x05 

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/

/*
    Função que insere registros no arquivo de dados e de índice

    Parâmetros:
        cabecalho: Cabeçalho do arquivo de dados
        arq: Ponteiro do arquivo binário de dados
        arq_index: Ponteiro do arquivo de índice
        ind_int: Vetor de registros de dados de índice (inteiros)
        ind_str: Vetor de registros de dados de índice (strings)
        cabecalho_indice: Cabeçalho do arquivo de índice
        campo_indexado: Campo que será utilizado nas buscas
        n: Número de buscas/inserções

*/
void inserir(header_t* cabecalho, FILE* arq, FILE* arq_index, header_indice_t* cabecalho_indice, char campo_indexado[20], int n);

/*
    Função que realiza busca sequencial ou indexada no arquivo binário, de acordo com os 
    parâmetros inseridos.

    Parâmetros:
        cabecalho: Cabeçalho do arquivo de dados
        arq: Ponteiro do arquivo binário de dados
        arq_index: Ponteiro do arquivo de índice
        ind_int: Vetor de registros de dados de índice (inteiros)
        ind_str: Vetor de registros de dados de índice (strings)
        cabecalho_indice: Cabeçalho do arquivo de índice
        campo_indexado: Campo que será utilizado nas buscas
        n: Número de buscas

*/
void busca(header_t* cabecalho, FILE* arq, FILE* arq_index, header_indice_t* cabecalho_indice, char campo_indexado[20], int n);

typedef struct criterio criterio_t;
typedef struct resultado resultado_t;

struct criterio {
    char campo[100];
    char tipo;
    union {
        int num;
        char *str;
    } valor;
};

struct resultado {
    crime_t* crime;
    long int byteoffset;
};

/*
    Função para criar um arquivo binário com as informações de um arquivo csv.

    Parâmetros:
        csv_file: nome do arquivo csv de entrada;
        binary_file: nome do arquivo binário de saída.

    Retorno: 0 se tudo ocorrer bem. Senão, retorna 1; 
*/
int create_table(char *csv_file, char *binary_file) {
    FILE *csv = fopen(csv_file, "r");

    if (csv == NULL) {
        return ERRO;
    }

    int colunas; // Quantidade de colunas que a linha do CSV terá.
    char **linha;

    //Ignorar primeira linha
    fscanf(csv, "%*[^\n]\n");

    linha = csv_ler_linha(csv, &colunas);

    header_t *cab = criar_cabecalho();

    FILE *bin = abrir_arquivo_escrita(cab, binary_file);

    if (bin == NULL) {
        fclose(csv);
        return ERRO;
    }

    while (linha != NULL) {
        crime_t *crime = criar_crime_de_csv(linha);

        if (crime == NULL) {
            fclose(bin);
            fclose(csv);
            return ERRO;
        }

        escrever_crime(cab, bin, crime);
        
        free(linha[0]);
        free(linha);

        destruir_crime(&crime);
        linha = csv_ler_linha(csv, &colunas);
    }

    fechar_arquivo_escrita(bin, cab);
    destruir_cabecalho(&cab);
    free(linha);
    fclose(csv);

    return OK;
}

/*
    Função que percorre sequencialmente um arquivo arquivo binário
    e lista todos os crimes registrados.
    
    Parâmetros: 
        binary_file: nome do arquivo binário

*/
void selecte(char *binary_file){
    
    //Um cabecalho é criado para obtermos o número de registros
    //presentes no arquivo que será lido.
    header_t *cabecalho = criar_cabecalho();
    crime_t *crime = criar_crime_bin();

    //Abertura do arquivo binário e obtenção do número de registros
    FILE *arq = abrir_arquivo_binario_leitura(cabecalho, binary_file);
    if(arq == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    int nroReg = retorna_nroRegArq_cabecalho(cabecalho);
    if(nroReg == 0){
        printf("Registro inexistente.\n");
        fclose(arq);
        return;
    }
    
    //Laço para percorrer o arquivo binário de forma sequencial, permitindo
    //a leitura de todos os registros
    for(int i = 0; i < nroReg; i++){
        int tam;
        crime = leitura_crime_de_binario(arq, &tam);
        
        //É necessário verificar se um determinado registro
        //foi logicamente removido
        if(crime_foi_removido(crime) == 0){
            printa_crime_bin(crime);
        }
    }
    
    fclose(arq);
}

/*
    Função que cria um arquivo de índices a partir de um arquivo binário original
    e do campo que será indexado.

    Parâmetro:
        binary_file: nome do arquivo binário 

*/
int indexa(char *binary_file, char campo_indexado[20], char tipo_dado[20], char arquivo_index[100]) {
    
    //Cria headers 
    header_indice_t *header_indice = criaHeaderIndice();
    header_t *header_dados = criar_cabecalho();

    //Abre arquivo de índice
    FILE *arq_indice = abrir_arquivo_indice(arquivo_index, header_indice, 2);
    if(arq_indice == NULL){
        printf("Falha no 1processamento do arquivo.\n");
        return ERRO;
    }
    
    //Abre arquivo de dados
    FILE *arq_binario = abrir_arquivo_binario_leitura(header_dados, binary_file);
    if(arq_binario == NULL){
        printf("Falha no processamento do arquivo.\n");
        return ERRO;
    }

    //Recebe número de registro do arquivo de dados, cria ponteiro de crime
    //e define variável tam
    int nroReg = retorna_nroRegArq_cabecalho(header_dados);
    crime_t *crime = criar_crime_bin();
    int tam = 0;

    //Leitura dos registros do arquivo de dados e inserção no arquivo de índices
    for(int i = 0; i < nroReg; i++){
        crime = leitura_crime_de_binario(arq_binario, &tam);

        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "idCrime") == 0){
            int idcrime = retorna_idCrime(crime);
            inserir_indice(header_indice, arq_indice, idcrime, tam);
        }
    }

    //arrumar esses fcloses depois
    
    fechar_arquivo_indice(arq_indice, header_indice, 1);
    fclose(arq_binario);

    binarioNaTela(arquivo_index);
    
    return OK;
}


/*
    Adiciona um resultado ao vetor de resultados utilizado na busca
    
    Parâmetros:
        result -> Vetor de resultados
        crime -> Ponteiro do registro de dados
        inicio -> ByteOffset da posição do registro
        quant -> Variável utilizada para controlar a quantidade de resultados
        presentes no vetor

*/
void adicionar_resultado(resultado_t ***result, crime_t* crime, long int inicio, int *quant) {
    (*quant)++;
    *result = realloc(*result, (*quant) * sizeof(resultado_t*));
    (*result)[*quant - 1] = (resultado_t*)malloc(sizeof(resultado_t));
    ((*result)[*quant - 1])->crime = crime;
    ((*result)[*quant - 1])->byteoffset = inicio;
}

/*
    Destrói e apaga um resultado 
    Parâmetros:
        result -> Resultado que será apagado
*/
void destruir_resultado(resultado_t **result) {
    destruir_crime(&((*result)->crime));
    free(*result);
    *result = NULL;
}

/*
    Função principal da funcionalidade de busca. Procura todos os registros de dados que
    satisfaçam os critérios estabelecidos no vetor de critérios. Retorna um vetor de resultados
   
    Parâmetros:
        arq: Ponteiro do arquivo binário de dados
        cabecalho: Cabeçalho do arquivo de dados
        criterios: Vetor de critérios de busca
        ind_int: Vetor de registros de índice
        ind_str: Vetor de registros de string
        campo_indexado: Campo indexado escolhido para a busca
        quant_ind: Tamanho do vetor de registros de índice 
        m: Quantidade de pares que pode ser utilizada como critério em uma busca
        quant: Variável utilizada para controlar a quantidade de resultados
        presentes no vetor de resultados
*/
resultado_t **buscar(FILE* arq, header_t *cabecalho, criterio_t** criterios, header_indice_t* header_indice, FILE* arq_indice, char campo_indexado[20], int m, int *quant) {
    resultado_t **result = NULL;
    *quant = 0;

    int nulls = 0;
    
    // Pesquisa em cada criterio
    for (int i = 0; i < m; i++) {
        // Se o campo do criteirio for indexado e for a primeiro criterio, usar o arquivo indexado
        if (strcmp(criterios[i]->campo, "idCrime") == 0 && i == 0) {
            int byteoffset = busca_indice(arq_indice, header_indice, criterios[i]->valor.num);
            if (byteoffset != -1) {
                fseek(arq, byteoffset, SEEK_SET);
                int tam;
                crime_t *crime = leitura_crime_de_binario(arq, &tam);
                adicionar_resultado(&result, crime, byteoffset, quant);
            }
        } else { // Se não é indexado
            if (i == 0) { // for primeira busca, fazer uma busca sequencial no arquivo de dados.
                long int byteoffset_agora = INICIO; 
                
                fseek(arq, INICIO, SEEK_SET);
                // A busca sequencial.
                for (int j = 0; j < retorna_nroRegArq_cabecalho(cabecalho); j++) {
                    int tam;
                    crime_t *crime = leitura_crime_de_binario(arq, &tam);
                    if (!crime_foi_removido(crime)) {
                        // Utiliza o valor do campo do criterio de busca
                        int num = -1;
                        char *str = NULL;
                        retorna_valor_campo(crime, criterios[i]->campo, &num, &str); 

                        // Se o crime comparado pertence ao criterio, adicionar no vetor de resultados.
                        if ((criterios[i]->tipo == 0 && num == criterios[i]->valor.num) ||
                            (criterios[i]->tipo == 1 && strcmp(str, criterios[i]->valor.str) == 0)) {
                            adicionar_resultado(&result, crime, byteoffset_agora, quant);
                        } else {
                            destruir_crime(&crime);
                        }

                        if (str != NULL) free(str);
                    } else {
                        destruir_crime(&crime);
                    }

                    // Atualiza a posição atual no arquivo de dados.
                    byteoffset_agora += tam;
                }
            } else { 
                // Para os próximos criterios, percorrer o vetor de resultados
                // inicial e remover os valores que não respeitam o criterio
                for (int j = 0; j < *quant; j++) {
                    if (result[j] != NULL) {
                        int num = -1;
                        char *str = NULL;
                        retorna_valor_campo(result[j]->crime, criterios[i]->campo, &num, &str);

                        // Se não atende ao criterio, remover resultado
                        if ((criterios[i]->tipo == 0 && num != criterios[i]->valor.num) ||
                            (criterios[i]->tipo == 1 && strcmp(str, criterios[i]->valor.str) != 0)) {
                            destruir_resultado(&result[j]);
                            nulls++; // Conta quantidade de valores removidos
                        }

                        if (str != NULL) free(str);
                    }
                }
            }
        }
    }


    // Se tiveram valores removidos, percorrer a lista de resultados e shiftar os
    // valores para que os valores nulos fiquem no final da lista.
    if (nulls != 0) {
        for (int i = 0; i < *quant; i++) {
            if (result[i] == NULL) {
                for (int j = i + 1; j < *quant; j++) {
                    if (result[j] != NULL) {
                        resultado_t *aux = result[i];
                        result[i] = result[j];
                        result[j] = aux;
                        break;
                    }
                }
            }
        }

        // Realocar vetor de resultados
        *quant -= nulls; 
        result = realloc(result, *quant * sizeof(resultado_t*));
    }

    return result;
}

/*
    Realiza a leitura de todos os critérios selecionados para uma busca e retorna
    em um vetor de critérios
    
    Parâmetros:
        m: Quantidade de pares que podem ser usados como critério na busca

*/
criterio_t **ler_criterios(int m) {
    criterio_t **criterios = (criterio_t**)malloc(m * sizeof(criterio_t*));

    for (int j = 0; j < m; j++) {
        criterio_t *crit = (criterio_t*)malloc(sizeof(criterio_t));

        scanf(" %s", crit->campo);

        if (strcmp(crit->campo, "idCrime") == 0 || strcmp(crit->campo, "numeroArtigo") == 0) {
            scanf(" %d", &(crit->valor.num));
            crit->tipo = 0;
        } else {
            crit->valor.str = (char *)malloc(100 * sizeof(char));
            scan_quote_string(crit->valor.str);
            crit->tipo = 1;
        }
        criterios[j] = crit;    
    }

    return criterios;
}
void desturir_criterio(criterio_t** criterio) {
    if ((*criterio)->tipo == 1) free((*criterio)->valor.str);
    free(*criterio);
}

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
int funcionalidades_index(char *binary_file, char campo_indexado[20], char tipo_dado[20], char arquivo_index[100], int n, int func) {
    header_t *cabecalho = criar_cabecalho();

    //Abertura do arquivo binário e obtenção do número de registros
    FILE *arq;
    
    if (func != 4) {
        arq = abrir_arq_leitura_escrita(cabecalho, binary_file);
    } else {
        arq = abrir_arquivo_binario_leitura(cabecalho, binary_file);
    }

    if (arq == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return ERRO;
    }

    header_indice_t* cabecalho_indice = criaHeaderIndice();
    FILE *arq_ind = abrir_arquivo_indice(arquivo_index, cabecalho_indice, func != 4);
    
    if (arq_ind == NULL) {
        printf("Falha no processamento do arquivo.\n");
        
        fechar_arquivo_escrita(arq, cabecalho);
        destruir_cabecalho(&cabecalho);
        
        return ERRO;
    }

    //Criando os dois vetores (apenas um será utilizado, o outro continuará como NULL)

    switch (func) {
        case 4:
            busca(cabecalho, arq, arq_ind, cabecalho_indice, campo_indexado, n);
            break;
        case 5:
            break;
        case 6:
            inserir(cabecalho, arq, arq_ind, cabecalho_indice, campo_indexado, n);
            break;
        case 7:
            break;  
    } 

    //O arquivo de índice é aberto para escrita caso a funcionalidade selecionada
    //não seja a de busca
    fechar_arquivo_indice(arq_ind, cabecalho_indice, func != 4);
    if (func != 4) {
        fechar_arquivo_escrita(arq, cabecalho);
        
        binarioNaTela(binary_file);
        binarioNaTela(arquivo_index);

    } else {
        fclose(arq);
    }    

    destruir_cabecalho(&cabecalho);
    //destruir_cabecalhoIndice(&cabecalho_indice);

    return OK;
}

/*
    Função que realiza busca sequencial ou indexada no arquivo binário, de acordo com os
    parâmetros inseridos.
    
    Parâmetros:
        cabecalho: Cabeçalho do arquivo de dados
        arq: Ponteiro do arquivo binário de dados
        arq_index: Ponteiro do arquivo de índice
        ind_int: Vetor de registros de dados de índice (inteiros)
        ind_str: Vetor de registros de dados de índice (strings)
        cabecalho_indice: Cabeçalho do arquivo de índice
        campo_indexado: Campo que será utilizado nas buscas

*/
void busca(header_t* cabecalho, FILE* arq, FILE* arq_index, header_indice_t* cabecalho_indice, char campo_indexado[20], int n) {
    for (int i = 0; i < n; i++) {
        int m;
        scanf(" %d", &m);
         
        criterio_t **criterios = ler_criterios(m);
        
        int quant;
        resultado_t **resultados = buscar(arq, cabecalho, criterios, cabecalho_indice, arq_index, campo_indexado, m, &quant);

        printf("Resposta para a busca %d\n", i + 1);
        if (resultados != NULL && quant != 0) {
            for (int j = 0; j < quant; j++) {
                printa_crime_bin(resultados[j]->crime);
                destruir_resultado(&(resultados[j]));
            }
        } else {
            printf("Registro inexistente.\n");
        }

        for (int j = 0; j < m; j++)
            desturir_criterio(&(criterios[j]));

        free(resultados);
        free(criterios);
    }
}



/*
    Função que insere registros no arquivo de dados e de índice

    Parâmetros:
        cabecalho: Cabeçalho do arquivo de dados
        arq: Ponteiro do arquivo binário de dados
        arq_index: Ponteiro do arquivo de índice
        ind_int: Vetor de registros de dados de índice (inteiros)
        ind_str: Vetor de registros de dados de índice (strings)
        cabecalho_indice: Cabeçalho do arquivo de índice
        campo_indexado: Campo que será utilizado nas buscas
*/

void inserir(header_t* cabecalho, FILE* arq, FILE* arq_index, header_indice_t* cabecalho_indice, char campo_indexado[20], int n) {
   for (int i = 0; i < n; i++) {
        crime_t *crime = criar_crime();
        
        long int byteoffset = retorna_proxByteOffset_cabecalho(cabecalho);

        if (i == 0) fseek(arq, byteoffset, SEEK_SET);
        escrever_crime(cabecalho, arq, crime);
    
        
        inserir_indice(cabecalho_indice, arq_index, retorna_idCrime(crime), byteoffset);
    }
}