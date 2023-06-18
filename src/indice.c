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
    Monitores: aff prefiro nem falar
        MODULARIZAR BUSCA!!!!!!!!!!
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

void print_no(no_t* no) {
    printf("Nível %d\n", no->nivel);
    printf("N: %d\n", no->n);
    printf("{ ");
    for (int i = 0; i < no->n; i++) {
        printf("%d <-- ", no->descendentes[i]);
        printf("%d,%ld --> ", no->chaves[i], no->byteOffset[i]);
    }
    printf("%d ", no->descendentes[no->n]);
    printf("}\n\n");
}
void escreverHeader(FILE *arq_indice, header_indice_t* header_indice){
    fseek(arq_indice, 0, SEEK_SET);
    fwrite(&(header_indice->status), 1, sizeof(char), arq_indice);
    fwrite(&(header_indice->noRaiz), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->rrnProxNo), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->nroNiveis), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->nroChaves), 1, sizeof(int), arq_indice);
    for (int i = 0; i < 59; i++) {
        fputc('$', arq_indice);
    }
}

void print_header_indice(header_indice_t* header) {
    printf("%d\n", header->noRaiz);
    printf("%d\n", header->nroChaves);
    printf("%d\n", header->nroNiveis);
    printf("%d\n", header->rrnProxNo);
    printf("%d\n", header->status);
}

void leituraHeader(FILE *arq_indice, header_indice_t* header_indice){
    fseek(arq_indice, 0, SEEK_SET);
    fread(&(header_indice->status), 1, sizeof(char), arq_indice);
    fread(&(header_indice->noRaiz), 1, sizeof(int), arq_indice);
    fread(&(header_indice->rrnProxNo), 1, sizeof(int), arq_indice);
    fread(&(header_indice->nroNiveis), 1, sizeof(int), arq_indice);
    fread(&(header_indice->nroChaves), 1, sizeof(int), arq_indice);
}

FILE *abrir_arquivo_indice(char *nome_arquivo, header_indice_t* header_indice, char tipo) {
    FILE *arq; 
    
    if (tipo == INDICE_LEITURA) arq = fopen(nome_arquivo, "rb");
    else if (tipo == INDICE_ESCRITA) arq = fopen(nome_arquivo, "rb+");
    else arq = fopen(nome_arquivo, "wb+");
    
    if (arq == NULL) return NULL;

    if (tipo != INDICE_CRIACAO) {
        leituraHeader(arq, header_indice);
        if (header_indice->status == '0') {
            fclose(arq);
            return NULL;
        }
    }

    if (tipo == INDICE_ESCRITA || tipo == INDICE_CRIACAO) {
        if (tipo == INDICE_CRIACAO) {
            header_indice->noRaiz = -1;
        }
        header_indice->status = '0';
        //header_indice->rrnProxNo = TAMANHO_PAGINA;
        escreverHeader(arq, header_indice);
    }

    return arq;
}

void fechar_arquivo_indice(FILE *arq_indice, header_indice_t *header_indice, char tipo) {
    if (tipo == INDICE_ESCRITA || tipo == INDICE_CRIACAO) {
        header_indice->status = '1';
        escreverHeader(arq_indice, header_indice);
    } 

    fclose(arq_indice);
}

no_t *criaNo(int nivel){
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



no_t *ler_no(FILE *arq_indice, int rrn) {
    if (rrn == -1) {
        return NULL;
    }
    
    fseek(arq_indice, rrn*TAMANHO_PAGINA + TAMANHO_PAGINA, SEEK_SET);

    no_t *novo_no = (no_t*)malloc(sizeof(no_t));
    
    novo_no->rrn = rrn;
    
    fread(&(novo_no->nivel), sizeof(int), 1, arq_indice);
    fread(&(novo_no->n), sizeof(int), 1, arq_indice);
    
    int i = 0;

    for (i = 0; i < CONST_M - 1; i++) {
        fread(&(novo_no->descendentes[i]), sizeof(int), 1, arq_indice);
        fread(&(novo_no->chaves[i]), sizeof(int), 1, arq_indice);
        fread(&(novo_no->byteOffset[i]), sizeof(long int), 1, arq_indice);
    }

    fread(&(novo_no->descendentes[i]), sizeof(int), 1, arq_indice);
    return novo_no;
}

void escreve_no(FILE *arq_indice, no_t* no, int rrn) {
    fseek(arq_indice, rrn*TAMANHO_PAGINA + TAMANHO_PAGINA, SEEK_SET);
    
    fwrite(&(no->nivel), sizeof(int), 1, arq_indice);
    fwrite(&(no->n), sizeof(int), 1, arq_indice);
    
    int i = 0;
    for (i = 0; i < CONST_M - 1; i++) {
        fwrite(&(no->descendentes[i]), sizeof(int), 1, arq_indice);
        fwrite(&(no->chaves[i]), sizeof(int), 1, arq_indice);
        fwrite(&(no->byteOffset[i]), sizeof(long int), 1, arq_indice);
    }

    fwrite(&(no->descendentes[CONST_M - 1]), sizeof(int), 1, arq_indice);
}

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

int no_igual_item(no_t **no, int item, int *pos) {
    return (*no)->chaves[*pos] == item;
}

int e_folha(no_t* no) {
    for (int i = 0; i < 5; i++)
        if (no->descendentes[i] != -1) return 0;

    return 1;
}
int no_nao_pertence_e_folha(no_t **no, int item, int *pos) {
    if ((*no)->chaves[*pos] == item) {
        free(*no);
        *no = NULL;
        return 1;
    }

    return e_folha(*no);
}

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
    
    if (item > curr_no->chaves[*pos]) {
        return buscar_arvoreB(curr_no->descendentes[*pos + 1], arq_indice, item, pos, condicao_de_retorno);
    } else {
        return buscar_arvoreB(curr_no->descendentes[*pos], arq_indice, item, pos, condicao_de_retorno);
    }
}

int busca_indice(FILE* arq_indice, header_indice_t *header_indice, int item) {
    int pos;
    no_t *no = buscar_arvoreB(header_indice->noRaiz, arq_indice, item, &pos, no_igual_item);
    
    if (no == NULL) return -1;

    return no->byteOffset[pos];
}


no_t *buscar_folha(FILE* arq_indice, int curr_rrn, int item) {
    int n;
    return buscar_arvoreB(curr_rrn, arq_indice, item, &n, no_nao_pertence_e_folha);
}

no_t *buscar_pai(FILE* arq_indice, int curr_rrn, no_t *no1, int *pos){
    if (curr_rrn == -1) return NULL;    
    
    no_t *curr_no = ler_no(arq_indice, curr_rrn);

    if (e_folha(curr_no)) return NULL;

    int i;
    for (i = 0; i < curr_no->n + 1; i++) {
        if (curr_no->descendentes[i] == no1->rrn) {
            *pos = i;
            return curr_no;
        }
    }

    for (i = 0; i < curr_no->n + 1; i++) { 
        no_t* busca;
        busca = buscar_pai(arq_indice, curr_no->descendentes[i], no1, pos);
        if (busca != NULL) return busca;
    }

    return NULL;
}

void shifta(int *array, int tamanho, int posicao){
    for(int i = tamanho-1; i > posicao; i--){
        array[i] = array[i-1];
    }
}

int compara_valores(const void *a, const void *b) {
    return (((valor_t*)a)->chave - ((valor_t*)b)->chave);
}

void preencher_no_valores(no_t *no, valor_t valores[], int tam, int inicio) {
    for (int i = 0; i < tam; i++) {
        no->chaves[i] = valores[inicio + i].chave;
        no->byteOffset[i] = valores[inicio + i].byteoffset;
        no->descendentes[i] = valores[inicio + i].filho_esq;
        no->descendentes[i + 1] = valores[inicio + i].filho_dir;
    }
}

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
    
    int p; 
    if (pos_pai - 1 >= 0 && pai->descendentes[pos_pai - 1] == no_irmao->rrn) {
        p = 1;
    } else {
        p = 0;
    }
    
    valor_t valor_novo = {rrn_filho_esq, idCrime, byteOffset, rrn_filho_dir};
    valor_t *valores = preenche_valores(no, pai, no_irmao, valor_novo, pos_pai, p);

    int tam = no->n + no_irmao->n + 2;
    int tamanho1 = (tam) / 2;
    int tamanho2 = (tam - (tam / 2)) - 1;

    if (p == 1) {
        preencher_no_valores(no_irmao, valores, tamanho1, 0);
        preencher_no_valores(no, valores, tamanho2, tamanho1 + 1);
    } else {
        preencher_no_valores(no, valores, tamanho1, 0);
        preencher_no_valores(no_irmao, valores, tamanho2, tamanho1 + 1);
    }

    pai->chaves[pos_pai - p] = valores[tamanho1].chave; 
    pai->byteOffset[pos_pai - p] = valores[tamanho1].byteoffset; 

    if (pos_pai - 1 >= 0 && pai->descendentes[pos_pai - 1] == no_irmao->rrn) {
        no_irmao->n = tamanho1;
        no->n = tamanho2;
    } else {
        no->n = tamanho1;
        no_irmao->n = tamanho2;
    }

    escreve_no(arq_indice, no, no->rrn);
    escreve_no(arq_indice, pai, pai->rrn);
    escreve_no(arq_indice, no_irmao, no_irmao->rrn);


    //0 -> redistribuição foi possível
    return 0;
}

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
    no_t *no_irmao = criaNo(1);
    no_irmao->n = size_split2;
    no_irmao->rrn = header_indice->rrnProxNo;
    no_irmao->nivel = no->nivel;
    preencher_no_valores(no_irmao, valores, size_split2, size_split1 + 1);
    
    // Promove o nó do meio
    no_t *novo_raiz = criaNo(1);
    novo_raiz->chaves[0] = valores[size_split1].chave;
    novo_raiz->byteOffset[0] = valores[size_split1].byteoffset;
    novo_raiz->rrn = header_indice->rrnProxNo + 1;

    // Adiciona os descendentes
    novo_raiz->descendentes[0] = no->rrn;
    novo_raiz->descendentes[1] = no_irmao->rrn;

    novo_raiz->nivel = no->nivel + 1;
    novo_raiz->n = 1;

    // Escreve os nós no arquivo.
    escreve_no(arq_indice, novo_raiz, novo_raiz->rrn);
    escreve_no(arq_indice, no, novo_raiz->descendentes[0]);
    escreve_no(arq_indice, no_irmao, novo_raiz->descendentes[1]);

    header_indice->rrnProxNo += 2;
    header_indice->noRaiz = novo_raiz->rrn;
    header_indice->nroNiveis++;
    header_indice->nroChaves += 1;
}


void split2_3(header_indice_t *header_indice, FILE* arq_indice, no_t* pai, 
    int pos_pai, no_t *no, no_t *no_irmao, 
    int idCrime, int byteOffset, int rrn_no_esq, int rrn_no_dir) {

    header_indice->nroChaves++;
    
    int p;
    if (pos_pai - 1 >= 0 && pai->descendentes[pos_pai - 1] == no_irmao->rrn) {
        p = 1; // direita do pai
    } else {
        p = 0; // esquerda do pai
    }

    valor_t valor_novo = {rrn_no_esq, idCrime, byteOffset, rrn_no_dir};
    valor_t *valores = preenche_valores(no, pai, no_irmao, valor_novo, pos_pai, p);
    
    no_t *no_meio = criaNo(1);
    no_meio->rrn = header_indice->rrnProxNo;
    no_meio->nivel = no->nivel;
    header_indice->rrnProxNo++; 

    if (p == 1) {
        preencher_no_valores(no_irmao, valores, 3, 0);
        preencher_no_valores(no, valores, 3, 4);
    } else {
        preencher_no_valores(no, valores, 3, 0);
        preencher_no_valores(no_irmao, valores, 3, 4);
    }
   
    preencher_no_valores(no_meio, valores, 2, 8);

    pai->chaves[pos_pai - p] = valores[3].chave; 
    pai->byteOffset[pos_pai - p] = valores[3].byteoffset;
 
    if (p == 1) {
        pai->descendentes[pos_pai - p] = no_irmao->rrn;
    } else {
        pai->descendentes[pos_pai - p] = no->rrn;
    }

    // Verifica se o nó pai está cheio e chama rotina de redistribuição;
    if (pai->n == CONST_M - 1) {
        rotina(header_indice, arq_indice, pai, valores[7].chave, valores[7].byteoffset, p == 1 ? no->rrn : no_irmao->rrn, no_meio->rrn);
        header_indice->nroChaves--;
    } else {
        for (int i = 3; i > pos_pai - p + 1; i--) {
            pai->byteOffset[i] = pai->byteOffset[i - 1];
        }
        shifta(pai->chaves, 4, pos_pai - p + 1);
        shifta(pai->descendentes, 5, pos_pai - p + 1);
        
        pai->chaves[pos_pai - p + 1] = valores[7].chave; 
        pai->byteOffset[pos_pai - p + 1] = valores[7].byteoffset;

        pai->descendentes[pos_pai - p + 2] = no_meio->rrn;
        
        if (p == 1) {
            pai->descendentes[pos_pai - p + 1] = no->rrn;
        } else {
            pai->descendentes[pos_pai - p + 1] = no_irmao->rrn;
        }
        pai->n += 1;
        escreve_no(arq_indice, pai, pai->rrn); 
    }
    no_irmao->n = 3;
    no->n = 3;
    no_meio->n = 2;

    escreve_no(arq_indice, no, no->rrn); 
    escreve_no(arq_indice, no_irmao, no_irmao->rrn); 
    escreve_no(arq_indice, no_meio, no_meio->rrn);
}

void rotina(header_indice_t *header_indice, FILE *arq_indice, 
                    no_t* no, int idCrime, long int byteoffset, int rrn_filho_esq, int rrn_filho_dir){
    
    //Se o nó é raiz
    int pos_pai;

    if (no->rrn == header_indice->noRaiz) {
        split1_2(header_indice, arq_indice, no, idCrime, byteoffset, rrn_filho_esq, rrn_filho_dir);
    } 
    
    else {
        no_t *no_pai = buscar_pai(arq_indice, header_indice->noRaiz, no, &pos_pai);
        int foi = 1;
        no_t* no_irmao;
        
        if (pos_pai - 1 >= 0) {
            no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai - 1]);
            foi = redistribuicao(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao, idCrime, byteoffset, rrn_filho_esq, rrn_filho_dir);
        } 
        
        if (pos_pai != no_pai->n && foi == 1) {
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
    }
}

void printar_nos(header_indice_t* header_indice, FILE *arq_indice) {
    for (int i = 0; i < header_indice->rrnProxNo; i++) {
        printf("RRN: %d\n", i);
        no_t *no = ler_no(arq_indice, i);
        print_no(no);
        free(no);
    }
}

void inserir_indice(header_indice_t* header_indice, FILE* arq_indice, 
                                        int idCrime, int byteOffset) {
    
    if (header_indice->noRaiz == -1) {
        no_t* novo_no = criaNo(1);
        novo_no->n = 1;
        novo_no->chaves[0] = idCrime;
        novo_no->byteOffset[0] = byteOffset;
        escreve_no(arq_indice, novo_no, 0);
        
        header_indice->noRaiz = 0;
        header_indice->nroNiveis = 1; 
        header_indice->nroChaves = 1; 
        header_indice->rrnProxNo = 1;
    } else {
        int curr_rrn = header_indice->noRaiz;
        
        no_t *folha = buscar_folha(arq_indice, curr_rrn, idCrime);
        // Verificar se exsite já
        if (folha == NULL) return;

        // Se NÃO está cheio
        if (folha->n < CONST_M - 1) {
            int pos = -1;
            for (int i = 0; i < folha->n && pos == -1; i++) {
                if (folha->chaves[i] > idCrime) pos = i;
            }

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

            escreve_no(arq_indice, folha, folha->rrn);
            free(folha);
        } else { 
            rotina(header_indice, arq_indice, folha, idCrime, byteOffset, -1, -1);
        }
    }
}

valor_t *preenche_valores(no_t *no, no_t *no_pai, no_t *no_irmao, valor_t valor_novo, int pos_pai, int p){
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
    
    //Split 2-3
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

        valores[no->n].chave = no_pai->chaves[pos_pai - p];
        valores[no->n].byteoffset = no_pai->byteOffset[pos_pai - p];
        
        valores[no->n].filho_esq = p == 1 ? valores[no->n + no_irmao->n].filho_dir : valores[no->n - 1].filho_dir;
        valores[no->n].filho_dir = p != 1 ? valores[no->n + 1].filho_esq : valores[0].filho_esq;
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