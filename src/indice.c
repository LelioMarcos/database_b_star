#include "../header/indice.h"
#include "../header/funcoesFornecidas.h"
#include "../header/crime.h"
#include "../header/arquivo.h"
#include "../header/utilidades.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
    Trabalho 2 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148
*/

#define CONST_M 5

#define TAMANHO_PAGINA 76

struct header_indice {
    char status;
    int noRaiz;
    int rrnProxNo;
    int nroNiveis;
    int nroChaves;
};

struct valor {
    int filho_esq;
    int chave;
    long int byteoffset;
    int filho_dir;
};

struct no {
    int rrn;
    int nivel;
    int n;
    int chaves[CONST_M - 1];
    long int byteOffset[CONST_M - 1];
    int descendentes[CONST_M];
};

/*
    Função que escreve o header do arquivo de indexação.
    
    Parâmetros:
        *arq_indice: ponteiro para o arquivo de index.
        header_indice: ponteiro para a struct do header.

*/
void escreverHeader(FILE *arq_indice, header_indice_t* header_indice){
    fseek(arq_indice, 0, SEEK_SET);
    fwrite(&(header_indice->status), 1, sizeof(char), arq_indice);
    fwrite(&(header_indice->noRaiz), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->rrnProxNo), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->nroNiveis), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->nroChaves), 1, sizeof(int), arq_indice);
    // Preencher lixo
    for (int i = 0; i < 59; i++) {
        fputc('$', arq_indice);
    }
}

/*
    Função que lê o header do arquivo de indice.
    
    Parâmetros:
        *arq_indice: ponteiro para o arquivo de indice.
        header_indice: ponteiro para a struct do header.

*/
void leituraHeader(FILE *arq_indice, header_indice_t* header_indice){
    fseek(arq_indice, 0, SEEK_SET);
    fread(&(header_indice->status), 1, sizeof(char), arq_indice);
    fread(&(header_indice->noRaiz), 1, sizeof(int), arq_indice);
    fread(&(header_indice->rrnProxNo), 1, sizeof(int), arq_indice);
    fread(&(header_indice->nroNiveis), 1, sizeof(int), arq_indice);
    fread(&(header_indice->nroChaves), 1, sizeof(int), arq_indice);
}

/*
    Função para abrir o arquivo de indice.

    Parâmetros:
        *nome_arquivo: string do nome do arquivo.
        *header_indice: ponteiro para a header do arquivo.
        tipo: indicação do tipo de operação será feita com o arquivo, dentre eles:
            INDICE_LEITURA: abre para somente leitura
            INDICE_ESCRITA: abre para leitura e escrita sem apagar o arquivo
            INDICE_CRIACAO: abre para leitura e escrita, criando um novo arquivo.

    Retorno: Ponteiro do arquivo de indice.

*/
FILE *abrir_arquivo_indice(char *nome_arquivo, header_indice_t* header_indice, char tipo) {
    FILE *arq; 
    
    if (tipo == INDICE_LEITURA) arq = fopen(nome_arquivo, "rb");
    else if (tipo == INDICE_ESCRITA) arq = fopen(nome_arquivo, "rb+");
    else arq = fopen(nome_arquivo, "wb+");
    
    // Não deu certo, fechar arquivo.
    if (arq == NULL) return NULL;

    // Se não tiver criando o arquivo.
    if (tipo != INDICE_CRIACAO) {
        leituraHeader(arq, header_indice);
        if (header_indice->status == '0') { // Inconsistente.
            fclose(arq);
            return NULL;
        }
    }
    
    // Se tiver escrevendo no arquivo, mudar status.
    if (tipo == INDICE_ESCRITA || tipo == INDICE_CRIACAO) {
        if (tipo == INDICE_CRIACAO) {
            header_indice->noRaiz = -1;
        }
        header_indice->status = '0';
        escreverHeader(arq, header_indice);
    }

    return arq;
}

/*
    Função para fechar o arquivo de indice.

    Parâmetros:
        *arq_indice: ponteiro do arquivo de indice.
        *header_indice: ponteiro para a header do arquivo.
        tipo: indicação do tipo de operação que foi feita com o arquivo, dentre eles:
            INDICE_LEITURA: abre para somente leitura
            INDICE_ESCRITA: abre para leitura e escrita sem apagar o arquivo
            INDICE_CRIACAO: abre para leitura e escrita, criando um novo arquivo.
*/
void fechar_arquivo_indice(FILE *arq_indice, header_indice_t *header_indice, char tipo) {
    // Se abriu para escrever, mudar status.
    if (tipo == INDICE_ESCRITA || tipo == INDICE_CRIACAO) {
        header_indice->status = '1';
        escreverHeader(arq_indice, header_indice);
    }

    free(header_indice);

    fclose(arq_indice);
}

/*
    Função para criar um nó novo.
    
    Retorno: ponteiro para a struct do nó.
*/
no_t *criaNo(){
    no_t *novoNo = (no_t*)malloc(sizeof(no_t));
    
    if(novoNo == NULL) return NULL;

    novoNo->nivel = 1;
    novoNo->n = 0;
    int i;
    for(i = 0; i < CONST_M - 1; i++){
        novoNo->chaves[i] = -1;
        novoNo->byteOffset[i] = -1; 
        novoNo->descendentes[i] = -1;
    }
    novoNo->descendentes[i] = -1;

    return novoNo;
}

/*
    Função para criar um cabeçalho.
    
    Retorno: ponteiro para a struct do cabeçalho.
*/
header_indice_t *criaHeaderIndice(){
    header_indice_t *header = (header_indice_t*)malloc(sizeof(header_indice_t));
    
    if(header == NULL) return NULL;

    header->status = '1';
    header->noRaiz = -1;
    header->nroChaves = 0;
    header->nroNiveis = 0;
    header->rrnProxNo = 0;
    return header;
}

/*
    Função para ler um nó na arvore B* do arquivo de indice.
    
    Parâmetros:
        *arq_indice: ponteiro para o arquivo de indice
        rrn: rrn do nó que deseja ler.

    Retorno: ponteiro para a struct do nó lido.
*/
no_t *ler_no(FILE *arq_indice, int rrn) {
    // Se rrn não existe, não retorna nó.
    if (rrn == -1) {
        return NULL;
    }
    
    fseek(arq_indice, rrn*TAMANHO_PAGINA + TAMANHO_PAGINA, SEEK_SET);

    no_t *novo_no = (no_t*)malloc(sizeof(no_t));
    
    novo_no->rrn = rrn;
    
    fread(&(novo_no->nivel), sizeof(int), 1, arq_indice);
    fread(&(novo_no->n), sizeof(int), 1, arq_indice);
    
    for (int i = 0; i < CONST_M - 1; i++) {
        fread(&(novo_no->descendentes[i]), sizeof(int), 1, arq_indice);
        fread(&(novo_no->chaves[i]), sizeof(int), 1, arq_indice);
        fread(&(novo_no->byteOffset[i]), sizeof(long int), 1, arq_indice);
    }

    fread(&(novo_no->descendentes[CONST_M - 1]), sizeof(int), 1, arq_indice);
    return novo_no;
}

/*
    Função para escrever um nó na arvore B* do arquivo de indice.
    
    Parâmetros:
        *arq_indice: ponteiro para o arquivo de indice
        *no: ponteiro para a struct do nó que será escrito 
        rrn: rrn do nó que deseja escrever.
*/
void escreve_no(FILE *arq_indice, no_t* no, int rrn) {
    fseek(arq_indice, rrn*TAMANHO_PAGINA + TAMANHO_PAGINA, SEEK_SET);
    
    fwrite(&(no->nivel), sizeof(int), 1, arq_indice);
    fwrite(&(no->n), sizeof(int), 1, arq_indice);

    for (int i = 0; i < CONST_M - 1; i++) {
        fwrite(&(no->descendentes[i]), sizeof(int), 1, arq_indice);
        fwrite(&(no->chaves[i]), sizeof(int), 1, arq_indice);
        fwrite(&(no->byteOffset[i]), sizeof(long int), 1, arq_indice);
    }

    fwrite(&(no->descendentes[CONST_M - 1]), sizeof(int), 1, arq_indice);
}

/*
    Função que faz uma busca binária em um vetor.
    
    Parâmetros:
        vet: vetor em que deseja buscar
        len: tamanho do vetor vet
        item: item que esteja procurando

    Retorno: valor encontrado dento do arquivos, que pode se igual ao
    item procurado ou o valor imediatamente maior ou menor a ele.
*/
int busca_binaria(int vet[], int len, int item) {
    int ini = 0;
    int fim = len - 1;
    int mid = 0;

    while (ini <= fim) {
        mid = (ini + fim) / 2;
        if (vet[mid] == item) return mid;

        if (item > vet[mid]) ini = mid + 1;
        else fim = mid - 1;
    }

    return mid;
}

/*
    Função para comparar se o valor da chave do nó na posição é igual 
    ao item. Será usado para a lógica de buscar um valor na árvore.
    
    Parâmetros:
        **no: ponteiro duplo para a struct do nó.
        item: item que está comparando
        *pos: posição no vetor de chaves do nó.

    Retorno: 1 se forem iguais, 0 se não.
*/
int no_igual_item(no_t **no, int item, int *pos) {
    return (*no)->chaves[*pos] == item;
}

/*
    Função para verificar se o nó é uma folha.
    
    Parâmetros:
        *no: ponteiro para a struct do nó.

    Retorno: 1 se é, 0 se não.
*/
int e_folha(no_t* no) {
    for (int i = 0; i < 5; i++)
        if (no->descendentes[i] != -1) return 0;

    return 1;
}

/*
    Função para verificar se o item pertence a ao vetor de chaves e se é uma folha.
    Sejá usado para a lógica de inserir um valor na árvore.
    
    Parâmetros:
        **no: ponteiro duplo para a struct do nó.
        item: item que está comparando
        *pos: posição no vetor de chaves do nó.

    Retorno: 1 se é, 0 se não. Caso o item esteja no nó, retornaremos NULO, pois
    não queremos que um valor repetido seja inserido. 
*/
int no_nao_pertence_e_folha(no_t **no, int item, int *pos) {
    if ((*no)->chaves[*pos] == item) {
        free(*no);
        *no = NULL;
        return 1;
    }

    return e_folha(*no);
}

/*
    Função que realiza uma busca na árvore B* no arquivo e realizará alguma ação em cada nó.
    
    Parâmetros:
        curr_rrn: rrn atual do nó.
        *arq_indice: ponteiro para o arquivo de indice.
        item: item que está comparando.
        *pos: posição no vetor de chaves do nó.
        (*condicao_de_retorno): ponteiro de função para a ação que será realizada a cada nó.

    Retorno: o nó que atende a condicao_de_retorno.
*/
no_t* buscar_arvoreB(int curr_rrn, FILE* arq_indice, int item, int *pos, 
    int (*condicao_de_retorno)(no_t**, int item, int *pos)) {
    
    if (curr_rrn == -1) {
        return NULL;
    }

    no_t *curr_no = ler_no(arq_indice, curr_rrn); 

    *pos = busca_binaria(curr_no->chaves, curr_no->n, item);

    if (condicao_de_retorno(&curr_no, item, pos)) {
        return curr_no;
    }
    
    int desce_para;
    if (item > curr_no->chaves[*pos]) {
        desce_para = curr_no->descendentes[*pos + 1];
    } else {
        desce_para = curr_no->descendentes[*pos];
    }
    
    free(curr_no);
    return buscar_arvoreB(desce_para, arq_indice, item, pos, condicao_de_retorno);
}


/*
    Função para saber se certo valor está na árvore B* do arquivo de indice.

    Parâmetros:
        *arq_indice: ponteiro para o arquivo de indice.
        *header_indice: ponteiro para o cabeçalho do arquivo
        item: item que está buscando

    Retorno: byteoffset no arquivo de dados em que a chave está.
*/
long int busca_indice(FILE* arq_indice, header_indice_t *header_indice, int item) {
    int pos;
    no_t *no = buscar_arvoreB(header_indice->noRaiz, arq_indice, item, &pos, no_igual_item);
    
    if (no == NULL) return -1;

    long int byteoffset = no->byteOffset[pos];

    free(no);

    return byteoffset;
}

/*
    Função para buscar por um nó folha ideal para inserir o item

    Parâmetros:
        *arq_indice: ponteiro para o arquivo de indice.
        *header_indice: ponteiro para o cabeçalho do arquivo
        item: item que deseja inserir

    Retorno: ponteiro para a struct do nó em que deseja inserir o item.
*/
no_t *buscar_folha(FILE* arq_indice, header_indice_t *header_indice, int item) {
    int n;
    return buscar_arvoreB(header_indice->noRaiz, arq_indice, item, &n, no_nao_pertence_e_folha);
}

/*
    Função para buscar pelo pai de um nó.

    Parâmetros:
        *arq_indice: ponteiro para o arquivo de indice.
        curr_rrn: rrn atual no arquivo de indice
        *no: ponteiro para a struct do nó.
        *pos: posição do nó no vetor de descendentes do pai.
*/
no_t *buscar_pai(FILE* arq_indice, int curr_rrn, no_t *no, int *pos){
    if (curr_rrn == -1) return NULL;    
    
    no_t *curr_no = ler_no(arq_indice, curr_rrn);

    // Se for folha, não terá filhos.
    if (e_folha(curr_no)) {
        free(curr_no);
        return NULL;
    }

    // Pessquisa se o rrn dos descendentes é igual ao rrn do no.
    int i;
    for (i = 0; i < curr_no->n + 1; i++) {
        if (curr_no->descendentes[i] == no->rrn) {
            *pos = i;
            return curr_no;
        }
    }

    // Busca pelos descendentes.
    for (i = 0; i < curr_no->n + 1; i++) { 
        no_t* busca;
        busca = buscar_pai(arq_indice, curr_no->descendentes[i], no, pos);
        if (busca != NULL) {
            free(curr_no);
            return busca;
        }
        free(busca);
    }

    free(curr_no);

    return NULL;
}

/*
    Função para deslocar o vetor para a direita a partir de uma posição

    Parâmetros:
        *array: vetor que deseja deslocar.
        tamanho: tamanho do vetor.
        posicao: até que posição deseja deslocar.
*/
void shifta(int *array, int tamanho, int posicao){
    for(int i = tamanho-1; i > posicao; i--){
        array[i] = array[i-1];
    }
}

/*
    Função que ompara dois valores tendo a chave como critério
    
    Parâmetros:
        a -> Ponteiro do primeiro valor a ser comparado
        b -> Ponteiro do segundo valor a ser comparado

*/
int compara_valores(const void *a, const void *b) {
    return (((valor_t*)a)->chave - ((valor_t*)b)->chave);
}

/*
    Função que preenche um determinado nó a partir de um vetor de valores
    
    Parâmetros:
        no -> Ponteiro do nó que será preenchido
        valores -> Vetor de valor
        tam -> Tamanho do vetor de valores
        inicio -> Indica a posição a partir da qual o vetor
        é acessado

*/
void preencher_no_valores(no_t *no, valor_t valores[], int tam, int inicio) {
    for (int i = 0; i < tam; i++) {
        no->chaves[i] = valores[inicio + i].chave;
        no->byteOffset[i] = valores[inicio + i].byteoffset;
        no->descendentes[i] = valores[inicio + i].filho_esq;
        no->descendentes[i + 1] = valores[inicio + i].filho_dir;
    }
}


/*
    Função que efetua a rotina de redistribuição em nós

    Parâmetros:
        header_indice -> Cabeçalho do arquivo de índice
        arq_indice -> Arquivo de índice
        pai -> Ponteiro do nó pai
        pos_pai -> Posição ocupada pelo nó pai
        no -> Nó a partir de onde será feita a redistribuição
        no_irmao -> Nó irmão/vizinho do no
        idCrime ->
        byteOffset -> Byteoffset da chave no arquivo de dados
        rrn_filho_esq -> Posição do filho esquerdo de no
        rrn_filho_dir -> Posição do filho direito de no

*/
int redistribuicao(header_indice_t *header_indice, FILE *arq_indice, 
    no_t *pai, int pos_pai, 
    no_t *no, no_t *no_irmao, 
    int idCrime, int byteOffset, int rrn_filho_esq, int rrn_filho_dir) {
    //se n de chaves for impar

    if (no_irmao->n == CONST_M - 1) {
        //redistribuicao nao é possivel
        return 1;
    }

    header_indice->nroChaves++;
    
    int sentido_pai; 
    if (pos_pai - 1 >= 0 && pai->descendentes[pos_pai - 1] == no_irmao->rrn) {
        sentido_pai = 1;
    } else {
        sentido_pai = 0;
    }
    
    valor_t valor_novo = {rrn_filho_esq, idCrime, byteOffset, rrn_filho_dir};
    valor_t *valores = preenche_valores(no, pai, no_irmao, valor_novo, pos_pai, sentido_pai);

    int tam = no->n + no_irmao->n + 2;
    int tamanho1 = (tam) / 2;
    int tamanho2 = (tam - (tam / 2)) - 1;

    if (sentido_pai == 1) {
        preencher_no_valores(no_irmao, valores, tamanho1, 0);
        preencher_no_valores(no, valores, tamanho2, tamanho1 + 1);
    } else {
        preencher_no_valores(no, valores, tamanho1, 0);
        preencher_no_valores(no_irmao, valores, tamanho2, tamanho1 + 1);
    }

    pai->chaves[pos_pai - sentido_pai] = valores[tamanho1].chave; 
    pai->byteOffset[pos_pai - sentido_pai] = valores[tamanho1].byteoffset; 

    if (pos_pai - 1 >= 0 && pai->descendentes[pos_pai - 1] == no_irmao->rrn) {
        no_irmao->n = tamanho1;
        no->n = tamanho2;
    } else {
        no->n = tamanho1;
        no_irmao->n = tamanho2;
    }

    free(valores);

    escreve_no(arq_indice, no, no->rrn);
    escreve_no(arq_indice, pai, pai->rrn);
    escreve_no(arq_indice, no_irmao, no_irmao->rrn);

    //0 -> redistribuição foi possível
    return 0;
}

/*
    Função que realiza o split 1-2 a partir de um nó

    Parâmetros:
        header_indice -> Cabeçalho do arquivo de índice
        arq_indice -> Arquivo de índice
        no -> Nó onde será realizado o split
        idCrime -> idCrime
        byteOffset ->
        rrn_filho_esq -> Posição do filho esquerdo do nó no arquivo de índice
        rrn_filho_dir -> Posição do filho direito do nó no arquivo de índice

*/
void split1_2(header_indice_t *header_indice, FILE* arq_indice, no_t *no,
    int idCrime, int byteOffset, int rrn_filho_esq, int rrn_filho_dir) {
    

    valor_t valor_novo = {rrn_filho_esq, idCrime, byteOffset, rrn_filho_dir};
    valor_t *valores = preenche_valores(no, NULL, NULL, valor_novo, 0, 0);

    int size_split1 = CONST_M / 2;
    int size_split2 = (CONST_M - (CONST_M / 2)) - 1;


    // O split será, consideirando valores {A, B, C, D, E} no split:
    //  A B C D E
    // <--- . --->
    
    // Cria no da esquerda
    no->n = size_split1;

    preencher_no_valores(no, valores, size_split1, 0);
    
    // Cria no da direita
    no_t *no_irmao = criaNo();
    no_irmao->n = size_split2;
    no_irmao->rrn = header_indice->rrnProxNo;
    no_irmao->nivel = no->nivel;
    preencher_no_valores(no_irmao, valores, size_split2, size_split1 + 1);
    
    // Promove o nó do meio
    no_t *novo_raiz = criaNo();
    novo_raiz->chaves[0] = valores[size_split1].chave;
    novo_raiz->byteOffset[0] = valores[size_split1].byteoffset;
    novo_raiz->rrn = header_indice->rrnProxNo + 1;

    // Adiciona os descendentes
    novo_raiz->descendentes[0] = no->rrn;
    novo_raiz->descendentes[1] = no_irmao->rrn;

    novo_raiz->nivel = no->nivel + 1;
    novo_raiz->n = 1;

    free(valores);
    // Escreve os nós no arquivo.
    escreve_no(arq_indice, novo_raiz, novo_raiz->rrn);
    escreve_no(arq_indice, no, novo_raiz->descendentes[0]);
    escreve_no(arq_indice, no_irmao, novo_raiz->descendentes[1]);


    header_indice->rrnProxNo += 2;
    header_indice->noRaiz = novo_raiz->rrn;
    header_indice->nroNiveis++;
    header_indice->nroChaves += 1;
    free(no_irmao);
    free(novo_raiz);
}

/*
    Função que efetua o split2-3 a partir de dois nós
    
    Parâmetros:
        header_indice -> Cabeçalho do arquivo de índice
        arq_indice -> Arquivo de índice
        pai -> Pai do no
        no -> Um dos nós no qual o split será efetuado
        no_irmao -> O segundo nó no qual o split será efetuado
        idCrime ->
        byteOffset ->
        rrn_no_esq -> Posição do nó esquerdo
        rrn_no_dir -> Posição do nó direito

*/
void split2_3(header_indice_t *header_indice, FILE* arq_indice, no_t* pai, 
    int pos_pai, no_t *no, no_t *no_irmao, 
    int idCrime, int byteOffset, int rrn_no_esq, int rrn_no_dir) {

    header_indice->nroChaves++;
    
    //Determina se o split será realizado à direita ou à esquerda do pai
    int sentido_pai;
    if (pos_pai - 1 >= 0 && pai->descendentes[pos_pai - 1] == no_irmao->rrn) {
        sentido_pai = 1; // direita do pai
    } else {
        sentido_pai = 0; // esquerda do pai
    }

    //Cria uma struct com os campos que serão utilizados
    valor_t valor_novo = {rrn_no_esq, idCrime, byteOffset, rrn_no_dir};

    //Cria e preenche o vetor de valores
    valor_t *valores = preenche_valores(no, pai, no_irmao, valor_novo, pos_pai, sentido_pai);
    
    no_t *novo_no = criaNo();
    novo_no->rrn = header_indice->rrnProxNo;
    novo_no->nivel = no->nivel;
    header_indice->rrnProxNo++; 

    if (sentido_pai == 1) {
        preencher_no_valores(no_irmao, valores, 3, 0);
        preencher_no_valores(no, valores, 3, 4);
    } else {
        preencher_no_valores(no, valores, 3, 0);
        preencher_no_valores(no_irmao, valores, 3, 4);
    }
   
    preencher_no_valores(novo_no, valores, 2, 8);

    pai->chaves[pos_pai - sentido_pai] = valores[3].chave; 
    pai->byteOffset[pos_pai - sentido_pai] = valores[3].byteoffset;
 
    if (sentido_pai == 1) {
        pai->descendentes[pos_pai - sentido_pai] = no_irmao->rrn;
    } else {
        pai->descendentes[pos_pai - sentido_pai] = no->rrn;
    }

    // Verifica se o nó pai está cheio e chama rotina de redistribuição;
    if (pai->n == CONST_M - 1) {
        rotina(header_indice, arq_indice, pai, valores[7].chave, valores[7].byteoffset, sentido_pai == 1 ? no->rrn : no_irmao->rrn, novo_no->rrn);
        header_indice->nroChaves--;
    } else {
        for (int i = 3; i > pos_pai - sentido_pai + 1; i--) {
            pai->byteOffset[i] = pai->byteOffset[i - 1];
        }
        shifta(pai->chaves, 4, pos_pai - sentido_pai + 1);
        shifta(pai->descendentes, 5, pos_pai - sentido_pai + 1);
        
        pai->chaves[pos_pai - sentido_pai + 1] = valores[7].chave; 
        pai->byteOffset[pos_pai - sentido_pai + 1] = valores[7].byteoffset;

        pai->descendentes[pos_pai - sentido_pai + 2] = novo_no->rrn;
        
        if (sentido_pai == 1) {
            pai->descendentes[pos_pai - sentido_pai + 1] = no->rrn;
        } else {
            pai->descendentes[pos_pai - sentido_pai + 1] = no_irmao->rrn;
        }
        pai->n += 1;
        escreve_no(arq_indice, pai, pai->rrn); 
    }
    no_irmao->n = 3;
    no->n = 3;
    novo_no->n = 2;

    free(valores);

    escreve_no(arq_indice, no, no->rrn); 
    escreve_no(arq_indice, no_irmao, no_irmao->rrn); 
    escreve_no(arq_indice, novo_no, novo_no->rrn);

    free(novo_no);
}

/*
    Função que realiza a rotina de redistribuição/split1-2/split2-3 dependendo
    das condições da árvore
    
    Parâmetros:
        header_indice -> Cabeçalho do arquivo de índice
        arq_indice -> Arquivo de índice
        no -> Nó atual
        idCrime -> idCrime da indexação
        byteoffset -> Byteoffset do registro do arquivo de dados
        rrn_filho_esq -> Posição do filho esquerdo do nó
        rrn_filho_dir -> Posição do filho direito do nó
    
*/
void rotina(header_indice_t *header_indice, FILE *arq_indice, 
                    no_t* no, int idCrime, long int byteoffset, int rrn_filho_esq, int rrn_filho_dir){
    
    
    int pos_pai;

    //Se o nó é raiz
    if (no->rrn == header_indice->noRaiz) {
        split1_2(header_indice, arq_indice, no, idCrime, byteoffset, rrn_filho_esq, rrn_filho_dir);
    } 
    
    else {
        no_t *no_pai = buscar_pai(arq_indice, header_indice->noRaiz, no, &pos_pai);
        int foi = 1;
        no_t* no_irmao = NULL;
        
        if (pos_pai - 1 >= 0) {
            no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai - 1]);
            foi = redistribuicao(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao, idCrime, byteoffset, rrn_filho_esq, rrn_filho_dir);
        } 
        
        if (pos_pai != no_pai->n && foi == 1) {
            if (no_irmao != NULL) free(no_irmao);
            no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai + 1]);
            foi = redistribuicao(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao, idCrime, byteoffset, rrn_filho_esq, rrn_filho_dir);
        } 
        
        //Redistribuição não foi possível
        if (foi == 1) {
            free(no_irmao);
            no_irmao = NULL;
            //verificar se rrn da direita != -1 (não existe página à direita)
            if (pos_pai < 4) {
                no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai + 1]);
            }

            if (no_irmao == NULL) {
                no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai - 1]);
            }
            
            split2_3(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao, idCrime, byteoffset, rrn_filho_esq, rrn_filho_dir);
        }

        free(no_pai);
        free(no_irmao);
    }
}

/*
    Função que efetua a inserção em um registro de índice

    Parâmetros:
        header_indice -> Cabeçalho do arquivo de índice
        arq_indice -> Arquivo de índice
        idCrime -> idCrime do registro do arquivo de dados
        byteOffset -> Byteoffset do registro do arquivo de dados
*/
void inserir_indice(header_indice_t* header_indice, FILE* arq_indice, 
                                        int idCrime, long int byteOffset) {
    
    //Caso a árvore 'ainda não tenha sido criada' (raiz nula)
    if (header_indice->noRaiz == -1) {
        no_t* novo_no = criaNo();
        novo_no->n = 1;
        novo_no->chaves[0] = idCrime;
        novo_no->byteOffset[0] = byteOffset;
        escreve_no(arq_indice, novo_no, 0);
        
        free(novo_no);
        header_indice->noRaiz = 0;
        header_indice->nroNiveis = 1; 
        header_indice->nroChaves = 1; 
        header_indice->rrnProxNo = 1;
    }
    
    //Caso a raiz já exista e tenha sido preenchida
    else {
        no_t *folha = buscar_folha(arq_indice, header_indice, idCrime);
       
        // Verifica se a chave já foi inserida anteriormente
        if (folha == NULL) return;

        // Se o nó NÃO está cheio
        if (folha->n < CONST_M - 1) {
            int pos = -1;
            
            //Laço que determina a posição de inserção no nó
            for (int i = 0; i < folha->n && pos == -1; i++) {
                if (folha->chaves[i] > idCrime) pos = i;
            }

            //Caso a posição seja válida
            if (pos != -1 && pos < folha->n) {
                for (int i = folha->n; i >= pos + 1; i--) {
                    folha->chaves[i] = folha->chaves[i - 1];
                    folha->byteOffset[i] = folha->byteOffset[i - 1];
                }
            } else {
                pos = folha->n;
            }

            folha->chaves[pos] = idCrime;
            folha->byteOffset[pos] = byteOffset;
            folha->n++;

            header_indice->nroChaves++;

            //Escreve o 'novo' nó no arquivo de índices
            escreve_no(arq_indice, folha, folha->rrn);
        } 
        
        //Caso o nó esteja cheio, a rotina de redistribuição é chamada
        else { 
            rotina(header_indice, arq_indice, folha, idCrime, byteOffset, -1, -1);
        }
        free(folha);
    }
}

/*
    Função que preenche o vetor de valores a partir de nós

    Parâmetros:
        no -> Nó que terá seus campos copiados para o vetor de valores
        no_pai -> Pai do nó
        no_irmao -> Irmão do nó
        valor_novo -> Struct com os campos que serão copiados
        pos_pai -> Posição do pai
        sentido_pai -> 'Posição do pai' em relação à direita ou esquerda  
        

*/
valor_t *preenche_valores(no_t *no, no_t *no_pai, no_t *no_irmao, 
    valor_t valor_novo, int pos_pai, int sentido_pai){
    
    int tam;

    if(no_pai == NULL && no_irmao == NULL) tam = CONST_M;
    else tam = no->n + no_irmao->n + 2;

    valor_t *valores = malloc(tam * sizeof(valor_t));
    for (int i = 0; i < no->n; i++) {
        valores[i].filho_esq = no->descendentes[i];
        valores[i].chave = no->chaves[i]; 
        valores[i].byteoffset = no->byteOffset[i];
        valores[i].filho_dir = no->descendentes[i + 1];
        no->chaves[i] = -1;
        no->byteOffset[i] = -1;
        no->descendentes[i] = -1;
    }
    no->descendentes[no->n] = -1;
    
    //Split 1-2: Sem pai e sem irmão
    if (no_pai != NULL && no_irmao != NULL){
        for (int i = 0; i < no_irmao->n; i++) {
            valores[i + no->n + 1].chave = no_irmao->chaves[i];
            valores[i + no->n + 1].byteoffset = no_irmao->byteOffset[i];
            valores[i + no->n + 1].filho_esq = no_irmao->descendentes[i];
            valores[i + no->n + 1].filho_dir = no_irmao->descendentes[i + 1];

            no_irmao->chaves[i] = -1;
            no_irmao->byteOffset[i] = -1;
            no_irmao->descendentes[i] = -1;
        }
        no_irmao->descendentes[no_irmao->n] = -1;

        valores[no->n].chave = no_pai->chaves[pos_pai - sentido_pai];
        valores[no->n].byteoffset = no_pai->byteOffset[pos_pai - sentido_pai];
        
        valores[no->n].filho_esq = sentido_pai == 1 ? valores[no->n + no_irmao->n].filho_dir : valores[no->n - 1].filho_dir;
        valores[no->n].filho_dir = sentido_pai != 1 ? valores[no->n + 1].filho_esq : valores[0].filho_esq;
    }

    valores[tam - 1].chave = valor_novo.chave;
    valores[tam - 1].byteoffset = valor_novo.byteoffset;
    valores[tam - 1].filho_dir = valor_novo.filho_dir;
    valores[tam - 1].filho_esq = valor_novo.filho_esq;

    qsort(valores, tam, sizeof(valor_t), compara_valores);

    for (int i = 1; i < tam; i++) {
        valores[i].filho_esq = valores[i - 1].filho_dir;
    }
   
    return valores;
}