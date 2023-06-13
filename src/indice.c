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

typedef struct valor valor_t;
struct valor {
    int chave;
    int byteoffset;
};

struct no {
    int rrn;
    int nivel;
    int n;
    int chaves[CONST_M - 1];
    long int byteOffset[CONST_M - 1];
    int descendentes[CONST_M];
};

void escreverHeader(FILE *arq_indice, header_indice_t* header_indice){
    fseek(arq_indice, 0, SEEK_SET);
    fwrite(&(header_indice->status), 1, sizeof(char), arq_indice);
    fwrite(&(header_indice->noRaiz), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->rrnProxNo), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->nroNiveis), 1, sizeof(int), arq_indice);
    fwrite(&(header_indice->nroChaves), 1, sizeof(int), arq_indice);
}

void print_header_indice(header_indice_t* header) {
    printf("%d\n", header->noRaiz);
    printf("%d\n", header->nroChaves);
    printf("%d\n", header->nroNiveis);
    printf("%d\n", header->rrnProxNo);
    printf("%d\n", header->status);
}

void leituraHeader(FILE *arq_indice, header_indice_t* header_indice){
    fread(&(header_indice->status), 1, sizeof(char), arq_indice);
    fread(&(header_indice->noRaiz), 1, sizeof(int), arq_indice);
    fread(&(header_indice->rrnProxNo), 1, sizeof(int), arq_indice);
    fread(&(header_indice->nroNiveis), 1, sizeof(int), arq_indice);
    fread(&(header_indice->nroChaves), 1, sizeof(int), arq_indice);
}

FILE *abrir_arquivo_indice(char *nome_arquivo, header_indice_t* header_indice, char tipo) {
    FILE *arq; 
    
    if (tipo == LEITURA) arq = fopen(nome_arquivo, "rb");
    else if (tipo == ESCRITA) arq = fopen(nome_arquivo, "rb+");
    else arq = fopen(nome_arquivo, "wb");

    if (arq == NULL) return NULL;

    if (tipo != 2) leituraHeader(arq, header_indice);
    
    if (tipo != 2 && header_indice->status == '0') {
        fclose(arq);
        return NULL;
    }

    if (tipo == 2) {
        header_indice->noRaiz = -1;
    }
    if (tipo == ESCRITA || tipo == 2) {
        header_indice->status = '0';
        header_indice->rrnProxNo = TAMANHO_PAGINA;
        escreverHeader(arq, header_indice);
    }

    return arq;
}

void fechar_arquivo_indice(FILE *arq_indice, header_indice_t *header_indice, char tipo) {
    if (tipo == ESCRITA) {
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
    return header;
}



no_t *ler_no(FILE *arq_indice, int rrn) {
    if(rrn == -1) return NULL;
    
    fseek(arq_indice, rrn*TAMANHO_PAGINA, SEEK_SET);

    no_t *novo_no = (no_t*)malloc(sizeof(no_t));

    fread(&(novo_no->nivel), sizeof(int), 1, arq_indice);
    fread(&(novo_no->n), sizeof(int), 1, arq_indice);
    
    int i = 0;

    for (i = 0; i < novo_no->n; i++) {
        fread(&(novo_no->descendentes[i]), sizeof(int), 1, arq_indice);
        fread(&(novo_no->chaves[i]), sizeof(int), 1, arq_indice);
        fread(&(novo_no->byteOffset[i]), sizeof(long int), 1, arq_indice);
    }

    fread(&(novo_no->descendentes[i]), sizeof(int), 1, arq_indice);
    return novo_no;
}

void escreve_no(FILE *arq_indice, no_t* no, int rrn) {
    fseek(arq_indice, rrn*TAMANHO_PAGINA, SEEK_SET);

    fwrite(&(no->nivel), sizeof(int), 1, arq_indice);
    fwrite(&(no->n), sizeof(int), 1, arq_indice);
    
    int i = 0;

    for (i = 0; i < no->n; i++) {
        fwrite(&(no->descendentes[i]), sizeof(int), 1, arq_indice);
        fwrite(&(no->chaves[i]), sizeof(int), 1, arq_indice);
        fwrite(&(no->byteOffset[i]), sizeof(int), 1, arq_indice);
    }

    fwrite(&(no->descendentes[i]), sizeof(int), 1, arq_indice);
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

void print_no(no_t* no) {
    printf("%d\n", no->nivel);
    printf("%d\n", no->n);
    for (int i = 0; i < no->n; i++) {
        printf("%d ", no->descendentes[i]);
        printf("%d ", no->chaves[i]);
    }
    printf("%d\n", no->descendentes[no->n]);
}

int buscar_arvoreB(int curr_rrn, FILE* arq_indice, int item) {
    printf("%d\n", curr_rrn);
    if (curr_rrn == -1) {
        return -1;
    }

    no_t *curr_no = ler_no(arq_indice, curr_rrn); 

    print_no(curr_no);

    int pos = busca_binaria(curr_no->chaves, curr_no->n, item);

    printf("%d %d\n\n", curr_no->chaves[pos], item);
    if (curr_no->chaves[pos] == item) 
        return curr_no->byteOffset[pos];
    else if (item > curr_no->chaves[pos]) {
        return buscar_arvoreB(curr_no->descendentes[pos + 1], arq_indice, item);
    } else {
        return buscar_arvoreB(curr_no->descendentes[pos], arq_indice, item);
    }
}

int busca_indice(FILE* arq_indice, header_indice_t *header_indice, int item) {
    return buscar_arvoreB(header_indice->noRaiz, arq_indice, item);
}

int e_folha(no_t* no) {
    for (int i = 0; i < 5; i++)
        if (no->descendentes[i] != -1) return 0;

    return 1;
}

no_t *buscar_folha(FILE* arq_indice, int curr_rrn, int item) {
    no_t *curr_no = ler_no(arq_indice, curr_rrn); 

    int pos = busca_binaria(curr_no->chaves, curr_no->n, item);

    if (curr_no->chaves[pos] == item) 
        return NULL;

    if (e_folha(curr_no)) return curr_no;
    
    if (item > curr_no->chaves[pos])
        return buscar_folha(arq_indice, curr_no->descendentes[pos + 1], item);
    else
        return buscar_folha(arq_indice, curr_no->descendentes[pos], item); 
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

int redistribuicao(header_indice_t *header_indice, FILE *arq_indice, no_t *pai, int pos_pai, no_t *no, no_t *no_irmao) {
    //se n de chaves for impar
    int tam = no->n + 1 + no->n;

    if (no_irmao->n == CONST_M - 1) {
        //redistribuicao nao é possivel
        return 1;
    }

    valor_t valores[tam];

    int tamanho1 = tam / 2;
    int tamanho2 = (tam - (tam / 2)) - 1;

    // adicionar chaves em valores
    for(int i = 0; i < no->n; i++){
        valores[i].chave = no->chaves[i];
        valores[i].byteoffset = no->byteOffset[i];
        no->chaves[i] = -1;
        no->byteOffset[i] = -1;
    }

    valores[no->n].chave = pai->chaves[pos_pai];
    valores[no->n].byteoffset = pai->byteOffset[pos_pai];

    for(int i = 0; i < no_irmao->n; i++){
        valores[i + no->n + 1].chave = no_irmao->chaves[i];
        valores[i + no->n + 1].byteoffset = no_irmao->byteOffset[i];
        no_irmao->chaves[i + no->n + 1] = -1;
        no_irmao->byteOffset[i + no->n + 1] = -1;
    }

    // redistribuir

    for (int i = 0; i < tamanho1; i++) {
        no->chaves[i] = valores[i].chave;
        no->byteOffset[i] = valores[i].byteoffset;
    }

    pai->chaves[pos_pai] = valores[tamanho1].chave; 
    pai->byteOffset[pos_pai] = valores[tamanho1].chave; 
    
    for (int i = 0; i < tamanho2; i++) {
        no_irmao->chaves[i] = valores[i + tamanho1 + 1].chave;
        no_irmao->byteOffset[i] = valores[i + tamanho1 + 1].byteoffset;
    } 

    no->n = tamanho1;
    no->n = tamanho2;

    escreve_no(arq_indice, no, no->rrn);
    escreve_no(arq_indice, pai, pai->rrn);
    escreve_no(arq_indice, no_irmao, no_irmao->rrn);

    //0 -> redistribuição foi possível
    return 0;
}


void split1_2(header_indice_t *header_indice, FILE* arq_indice, no_t *no, int idCrime, int byteOffset) {
    
    // armazenar valores em ordem, para o split
    valor_t valores[CONST_M];
    for (int i = 0; i < CONST_M - 1; i++) {
        valores[i].chave = no->chaves[i]; 
        valores[i].byteoffset = no->byteOffset[i];
        no->chaves[i] = -1;
        no->byteOffset[i] = -1;
    }
    
    int pos = -1;
    for (int i = 0; i < CONST_M - 1 && pos != i; i++) {
        if (valores[i].chave > idCrime) pos = i;
    }

    if (pos < CONST_M - 1) {
        for (int i = CONST_M; i < pos + 1; i--) {
            valores[i] = valores[i - 1];
        }
    }
    
    int size_split1 = CONST_M / 2;
    int size_split2 = (CONST_M - (CONST_M / 2)) - 1;
    valores[pos].chave = idCrime;
    valores[pos].byteoffset = byteOffset;

    // O split será, consideirando valores {A, B, C, D, E} no split:
    //  A B C D E
    // <--- . --->
    
    // Cria no da esquerda
    no_t *no_esquerda = criaNo(1);
    for (int i = 0; i < size_split1; i++) {
        no_esquerda->chaves[i] = valores[i].chave;
        no_esquerda->byteOffset[i] = valores[i].byteoffset;
    }

    // Cria no da direita
    no_t *no_direita = criaNo(1);
    for (int i = size_split1 + 1; i < size_split1 + 1 + size_split2; i++) {
        no_direita->chaves[i] = valores[i].chave;
        no_direita->byteOffset[i] = valores[i].byteoffset;
    }

    // Promove o nó do meio
    no->chaves[0] = valores[size_split1].chave;
    no->byteOffset[0] = valores[size_split1].byteoffset;

    // Adiciona os descendentes
    no->descendentes[0] = header_indice->rrnProxNo;
    no->descendentes[1] = header_indice->rrnProxNo + 1;

    no->nivel = 2;

    // Escreve os nós no arquivo.
    escreve_no(arq_indice, no, 0);
    escreve_no(arq_indice, no_esquerda, no->descendentes[0]);
    escreve_no(arq_indice, no_direita, no->descendentes[1]);

    header_indice->rrnProxNo += 2;
    header_indice->nroNiveis++;
    header_indice->nroChaves += 1;
}

void split2_3(header_indice_t *header_indice, FILE* arq_indice, no_t* no_pai, int pos_no_pai, no_t *no_esq, no_t *no_dir, int idCrime, int byteOffset){
    
    // add 5
    // 2 4 5 | 6 | 8 10 12 | 14 | 16 18 20 21
    //MODULARIZAR ESSA ATROCIDADE DEPOIS (NAO ESQUECER)
    valor_t valores[(CONST_M * 2) + 3];


    for (int i = 0; i < (CONST_M - 1); i++) {
        valores[i].chave = no_esq->chaves[i];
        valores[i].byteoffset = no_esq->byteOffset[i];
        
        no_esq->chaves[i] = -1;
        no_esq->byteOffset[i] = -1;
    }

    valores[CONST_M - 1].chave = no_pai->chaves[pos_no_pai];

    for (int i = CONST_M; i < (CONST_M * 2 - 1); i++) {
        valores[i].chave = no_dir->chaves[i];
        valores[i].byteoffset = no_dir->byteOffset[i];
        
        no_dir->chaves[i] = -1;
        no_dir->byteOffset[i] = -1;
    }

    int pos = -1;
    for (int i = 0; i < CONST_M *2 + 3 && pos != i; i++) {
        if (valores[i].chave > idCrime) pos = i;
    }
    if (pos < CONST_M *2 + 3) {
        for (int i = CONST_M *2 + 2; i < pos + 1; i--) {
            valores[i] = valores[i - 1];
        }
    }
    valores[pos].chave = idCrime;
    valores[pos].byteoffset = byteOffset;


    no_t *no_meio = criaNo(no_esq->nivel);

    for (int i = 0; i < 3; i++) {
        no_esq->chaves[i] = valores[i].chave;
        no_esq->byteOffset[i] = valores[i].byteoffset;
    }

    for (int i = 5; i < 5 + 3; i++) {
        no_meio->chaves[i - 5] = valores[i].chave;
        no_meio->byteOffset[i - 5] = valores[i].byteoffset;
    }
    

    for (int i = 9; i < 9 + 3; i++) {
        no_dir->chaves[i - 9] = valores[i].chave;
        no_dir->byteOffset[i - 9] = valores[i].byteoffset;
    }
    no_pai->chaves[pos_no_pai] = valores[4].chave; 
    no_pai->byteOffset[pos_no_pai] = valores[4].byteoffset;
    no_pai->descendentes[pos_no_pai] = no_esq->rrn;

    // Verifica se o nó pai está cheio e chama rotina de redistribuição;
    if (no_pai->n == CONST_M - 1) {
        rotina(header_indice, arq_indice, no_pai, valores[8].chave, valores[8].byteoffset);
    } else {
        //shifta(no_pai->byteOffset, CONST_M - 1, pos_no_pai+1);
        shifta(no_pai->chaves, CONST_M - 1, pos_no_pai+1);
        shifta(no_pai->descendentes, CONST_M - 1, pos_no_pai+1);
        no_pai->chaves[pos_no_pai + 1] = valores[8].chave; 
        no_pai->byteOffset[pos_no_pai + 1] = valores[8].byteoffset;
    }
    
    //Shift
    
    no_pai->descendentes[pos_no_pai + 1] = no_meio->rrn;
    no_pai->descendentes[pos_no_pai + 2] = no_dir->rrn;
    
     

}

void rotina(header_indice_t *header_indice, FILE *arq_indice, no_t* no, int idCrime, long int byteoffset){
    
    //Se o nó é raiz
    int pos_pai;
    no_t *no_pai = buscar_pai(arq_indice, header_indice->noRaiz, no, &pos_pai);

    if (no_pai->rrn == header_indice->noRaiz) {
        split1_2(header_indice, arq_indice, no_pai, idCrime, byteoffset);
    } 
    
    else {
        int foi = 1;
        no_t* no_irmao;
        
        
        
        if (pos_pai != 0) {
            no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai - 1]);
            foi = redistribuicao(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao);
        } 
        
        if (pos_pai != no_pai->n && foi == 1) {
            no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai + 1]);
            foi = redistribuicao(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao);
        } 
        
        //Redistribuição não foi possível
        if (foi == 1) {
            
            //verificar se rrn da direita != -1 (não existe página à direita)
            no_irmao = ler_no(arq_indice, no_pai->descendentes[ + 1]);
            
            if(no_irmao == NULL){
                no_irmao = ler_no(arq_indice, no_pai->descendentes[pos_pai - 1]);
            }
            
            split2_3(header_indice, arq_indice, no_pai, pos_pai, no, no_irmao, idCrime, byteoffset);
        }
    }
}



void inserir_indice(header_indice_t* header_indice, FILE* arq_indice, int idCrime, int byteOffset) {
    print_header_indice(header_indice);
    if (header_indice->noRaiz == -1) {
        no_t* novo_no = criaNo(1);
        novo_no->n = 1;
        novo_no->chaves[0] = idCrime;
        novo_no->byteOffset[0] = byteOffset;
        escreve_no(arq_indice, novo_no, 0);
        
        header_indice->noRaiz = 0;
        header_indice->nroNiveis = 1; 
        header_indice->nroChaves = 1; 
        header_indice->rrnProxNo = 2*TAMANHO_PAGINA; 
    } else {
        int curr_rrn = header_indice->noRaiz;
        no_t *folha = buscar_folha(arq_indice, curr_rrn, idCrime);

        // Verificar se exsite já
        if (folha == NULL) return;
        for (int i = 0; i < folha->n; i++) {
            if (folha->chaves[i] == idCrime) return;
        }


        // Se NÃO está cheio
        if (folha->n < CONST_M - 1) {
            int pos = -1;
            for (int i = 0; i < folha->n && pos != i; i++) {
                if (folha->chaves[i] > idCrime) pos = i;
            }

            if (pos < folha->n) {
                for (int i = folha->n; i < pos + 1; i--) {
                    folha->chaves[i] = folha->chaves[i - 1];
                    folha->byteOffset[i] = folha->byteOffset[i - 1];
                }
            }

            folha->chaves[pos] = idCrime;
            folha->byteOffset[pos] = byteOffset;
            folha->n++;

            header_indice->nroChaves += 1;
        } else {
            rotina(header_indice, arq_indice, folha, idCrime, byteOffset);
        }
    }
}
    
    
    // busca folha
    // se folha dá, insere
    // se não, busca pai e o outro filho desse pai
    // se tem espaço nesse filho, redistribui
    //      pega valores da folha, do pai e os do filho ordenados
    //      divide os valores o melhor possível
    //      promove o nó como o novo pai.
    // se não tiver espaço,
        // se nó raiz: split 1-2
        // senão: split 2-3
    // insere ordenado



//}



/* 
 
    Cria cabeçalho de arquivo de índice.
    Parâmetro:
header_indice_t *cria_cabecalhoIndice(){
    header_indice_t *header = (header_indice_t*)malloc(sizeof(header_indice_t));
    
    if (header == NULL){
        return NULL;

    }

    header -> status = '1'; //Inicia o status como consistente
    header -> qtdReg = 0;   //Quantidade de registros é inicialmente 0
    return header;
}

 
    Destrói o cabeçalho de arquivo de índice.
    Parâmetro:
        cabecalho_indicie -> Ponteiro de ponteiro do cabeçalho

void destruir_cabecalhoIndice(header_indice_t** cabecalho_indicie) {
    free(*cabecalho_indicie);
    *cabecalho_indicie = NULL;
}

 
    Atualiza o cabeçalho do arquivo de índices com seus valores modificados
    Parâmetro:
        arq -> ponteiro arquivo de índices
        cabecalho -> cabeçalho do arquivo de índices

void atualiza_cabecalhoIndice(FILE *arq, header_indice_t *cabecalho){
    //Volta para o início do arquivo de índice
    if (ftell(arq) != 0) fseek(arq, 0, SEEK_SET);

    //Sobrescreve o cabeçalho com as informações atualizadas
    fwrite(&(cabecalho->status), 1, sizeof(char), arq);
    fwrite(&(cabecalho->qtdReg), 1, sizeof(int), arq);


}

 
    Cria um registro de arquivo de índices (inteiro)
    Parâmetro:

indice_inteiro_t *cria_indice_inteiro(){
    indice_inteiro_t *index = (indice_inteiro_t*)malloc(sizeof(indice_inteiro_t));

    if(index == NULL){
        return NULL;

    }

    return index;

}

 
    Cria um registro de arquivo de índices (string)
    Parâmetro:

indice_string_t *cria_indice_string(){
    indice_string_t *index = (indice_string_t*)malloc(sizeof(indice_string_t));

    if(index == NULL){
        return NULL;

    }

    return index;
    
}

 
    Abre o arquivo de índices de forma que os registros de índice
    possam ser escritos.
    Parâmetros:
        nomeArquivo -> Nome do arquivo de índices
        cabecalho -> cabeçalho de índice de registros


FILE *abreArquivoIndice_escrita(char *nomeArquivo, header_indice_t *cabecalho){
    FILE *arq = fopen(nomeArquivo, "wb");
    
    if(arq == NULL){
        return NULL;
    }
    
    //Escreve o cabeçalho no arquivo de índice
    fwrite(&(cabecalho->status), sizeof(char), 1, arq);
    fwrite(&(cabecalho->qtdReg), sizeof(int), 1, arq);

    return arq;
    
}

 
    Fecha o arquivo de índice e atualiza seu cabeçalho.
    Parâmetros:
        file -> arquivo de índices
        cabecalho -> cabeçalho do arquivo de índices

  
void fechar_arquivoIndice(FILE *file, header_indice_t *cabecalho) {
    cabecalho->status = '1';
    atualiza_cabecalhoIndice(file, cabecalho);    // Escreve essa mudança no arquivo.
    
    fclose(file);
} 

 
    Cria um vetor de registros escolhidos de acordo com o campo indexado (int)
    Parâmetros:

indice_inteiro_t **cria_vetorIndices_inteiro(){
    indice_inteiro_t **vetor_indices = (indice_inteiro_t**)malloc(sizeof(indice_inteiro_t*));
    return vetor_indices;

}


 
    Cria um vetor de registros escolhidos de acordo com o campo indexado (string)
    Parâmetros:

indice_string_t **cria_vetorIndices_strings(){
    indice_string_t **vetor_indices = (indice_string_t**)malloc(sizeof(indice_string_t*));
    return vetor_indices;

}

 
    Compara dois registros escolhidos de acordo com o campo indexado (int)
    Parâmetros:
        registroA -> Primeiro registro a ser comparado
        registroB -> Segundo registro a ser comparado
  
int compararIndiceInteiros (const void* registroA, const void* registroB) {
    indice_inteiro_t* indiceA = *(indice_inteiro_t**)registroA;
    indice_inteiro_t* indiceB = *(indice_inteiro_t**)registroB;

    return (indiceA->chaveBusca - indiceB->chaveBusca);
}

 
    Compara dois registros escolhidos de acordo com o campo indexado (string)
    Parâmetros:
        registroA -> Primeiro registro a ser comparado
        registroB -> Segundo registro a ser comparado

  
int compararIndiceStrings(const void *registroA, const void *registroB) {
    indice_string_t* indiceA = *(indice_string_t**)registroA;
    indice_string_t* indiceB = *(indice_string_t**)registroB;

    return strcmp(indiceA->chaveBusca, indiceB->chaveBusca);
}

 
    Escreve no arquivo de índices a partir de um vetor já preenchido, seja ele do tipo inteiro
    ou de strings.
    Parâmetros:
        arq_index -> Ponteiro do arquivo de indíce
        vetor_int -> Vetor de registros de índice (inteiro)
        vetor_str -> Vetor de registros de índice (string)
        cabecalho_indicie -> Ponteiro do cabeçalho do arquivo de índice

  
void escreve_indice(FILE* arq_index, indice_inteiro_t **vetor_int, indice_string_t** vetor_str, header_indice_t* cabecalho_indice) { 
    indice_inteiro_t* index_int = NULL;
    indice_string_t* index_str = NULL;

    for(int i = 0; i < cabecalho_indice->qtdReg; i++){
        
        //Escrita a partir de um vetor de registros de índice (inteiro)
        if (vetor_int != NULL)  {
            index_int = vetor_int[i];
            fwrite(&(index_int->chaveBusca), sizeof(int), 1, arq_index);
            fwrite(&(index_int->byteOffset), sizeof(long int), 1, arq_index);
        
        //Escrita a partir de um vetor de registros de índice (string)
        } else {
            index_str = vetor_str[i];
            int len = strlen(index_str->chaveBusca);

            //Caso o tamanho da string seja menor do que 12, ela será preenchida
            //com $
            if (len < 12)
                preenche_string(index_str->chaveBusca);

            fwrite(&(index_str->chaveBusca), sizeof(char), 12, arq_index);
            fwrite(&(index_str->byteOffset), sizeof(long int), 1, arq_index);
        }
    }
}

 
    Lê o binário original, filtra de acordo com o
    campo indexado (tipo inteiro), calcula o byteoffset de cada registro selecionado,
    ordena e escreve no arquivo de índices.
    Parâmetro:
        header -> cabeçalho do arquivo de índice
        arq_binario -> Arquivo de onde serão lidos os registros
        arq_index -> Arquivo onde serão escritos os índices
        campo_indexado -> Campo que será utilizado para criar o arquivo de índice (apenas inteiros)

  
void escreveIndice_inteiros(header_indice_t *cabecalhoIndice, FILE *arq_binario, FILE *arq_index, char *campo_indexado){

    //Lendo o binario original
    //leitura do cabecalho
    header_t *cabecalhoBinario = criar_cabecalho();
    ler_cabecalho(arq_binario, cabecalhoBinario);


    //Guardando o número de registros do binário original
    int nroReg = retorna_nroRegArq_cabecalho(cabecalhoBinario);
    
    //Cria o ponteiro de crime que será utilizado para ler
    //registro por registro do binário
    crime_t *crime = criar_crime_bin();

    //Cria o vetor de indices que será útil na ordenação
    indice_inteiro_t **vetor = cria_vetorIndices_inteiro();
    
    //Controla a posicao do vetor
    int posicao_vetor = 0;

    long int pos_inicial = 0x11;
    for(int i = 0; i < nroReg; i++) {
        int tam;
        crime = leitura_crime_de_binario(arq_binario, &tam);

        indice_inteiro_t *index = cria_indice_inteiro();

        
        //Se o campo indexado for idCrime
        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "idCrime") == 0){
            int idCrime = retorna_idCrime(crime);
            index->chaveBusca = idCrime;

            index->byteOffset = pos_inicial;
            vetor[posicao_vetor] = index;
            posicao_vetor++;

        }


        //Se o campo indexado for numeroArtigo
        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "numeroArtigo") == 0){
            int numeroArtigo = retorna_numeroArtigo(crime);
            
            //Verifica se o campo é nulo
            if(numeroArtigo != -1){    
                index->chaveBusca = numeroArtigo;

                index->byteOffset = pos_inicial;
                vetor[posicao_vetor] = index;
                posicao_vetor++;
        
            }
        }

        //A cada leitura de registro do binário, realocamos o vetor
        //e adicionamos mais uma posição
        vetor = realloc(vetor, (posicao_vetor + 1) * sizeof(indice_inteiro_t*));

        pos_inicial += tam;
    }

    //Atualiza cabeçalho do registro de indice com o numero de registros que serão escritos 
    cabecalhoIndice->qtdReg = posicao_vetor;


    //Ordenação e escrita no arquivo
    qsort(vetor, posicao_vetor, sizeof(indice_inteiro_t*), compararIndiceInteiros);
    
    escreve_indice(arq_index, vetor, NULL, cabecalhoIndice); 
    
    //Fecha arquivo de índices e encerra indexação
    fechar_arquivoIndice(arq_index, cabecalhoIndice);       
    

}

 
    Lê o binário original, filtra de acordo com o
    campo indexado (tipo inteiro), calcula o byteoffset de cada registro selecionado,
    ordena e escreve no arquivo de índices.
    Parâmetro:
        header -> cabeçalho do arquivo de índice
        arq_binario -> Arquivo de onde serão lidos os registros
        arq_index -> Arquivo onde serão escritos os índices
        campo_indexado -> Campo que será utilizado para criar o arquivo de índice (apenas inteiros)

  
void escreveIndice_strings(header_indice_t *cabecalhoIndice, FILE *arq_binario, FILE *arq_index, char *campo_indexado){
    //Lendo o binario original
    //leitura do cabecalho
    header_t *cabecalho = criar_cabecalho();
    ler_cabecalho(arq_binario, cabecalho);

    //Guardando o número de registros do binário original
    int nroReg = retorna_nroRegArq_cabecalho(cabecalho);

    //Cria o ponteiro de crime que será utilizado para ler
    //registro por registro do binário
    crime_t *crime = criar_crime_bin();

    //Cria o vetor de indices que será útil na ordenação
    indice_string_t **vetor = cria_vetorIndices_strings();

    int posicao_vetor = 0;

    long int pos_inicial = 0x11;
    for(int i = 0; i < nroReg; i++){

        int tam;
        crime = leitura_crime_de_binario(arq_binario, &tam);
        indice_string_t *index = cria_indice_string();


        //Se o campo indexado for dataCrime
        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "dataCrime") == 0){
            char *dataCrime = retorna_dataCrime(crime);
            
            //Não é necessário verificar se dataCrime é nulo.
            //Copiaremos direto para a chaveBusca do registro de indice
            strcpy(index->chaveBusca, dataCrime);

            index->byteOffset = pos_inicial;
            vetor[posicao_vetor] = index;
            posicao_vetor++;

        }

        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "marcaCelular") == 0){
            char *marcaCelular = retorna_marcaCelular(crime);

            //Verifica se o campo não é nulo
            if(marcaCelular[0] != '\0'){
                
                //Se o campo em questão tiver espaços vazios, será preenchido
                if(strlen(marcaCelular) < 12){
                    preenche_string(marcaCelular);
                }

                strcpy(index->chaveBusca, marcaCelular);
                index->byteOffset = pos_inicial;
                vetor[posicao_vetor] = index;
                posicao_vetor++;
            }

            

        }

        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "lugarCrime") == 0){
            char *lugarCrime = retorna_lugarCrime(crime);

            //Verifica se o campo não é nulo
            if(lugarCrime[0] != '\0'){
                
                //Como lugarCrime tem tamanho variável, precisamos
                //determinar se o truncamento da string será necessário
                if(strlen(lugarCrime) > 12){
                    trunca_strings(lugarCrime);
                }

                //Se o campo em questão tiver espaços vazios, será preenchido
                if(strlen(lugarCrime) < 12){
                    preenche_string(lugarCrime);
                }
                
                strcpy(index->chaveBusca, lugarCrime);
                index->byteOffset = pos_inicial;
                vetor[posicao_vetor] = index;
                posicao_vetor++;
            }

        }

        if(crime_foi_removido(crime) != 1 && strcmp(campo_indexado, "descricaoCrime") == 0){
            char *descricaoCrime = retorna_descricaoCrime(crime);

            //Verifica se o campo não é nulo
            if(descricaoCrime[0] != '\0'){
                
                //Como descricaoCrime tem tamanho variável, precisamos
                //determinar se o truncamento da string será necessário
                if(strlen(descricaoCrime) > 12){
                    trunca_strings(descricaoCrime);
                }
            
                //Se o campo em questão tiver espaços vazios, será preenchido
                if(strlen(descricaoCrime) < 12){
                    preenche_string(descricaoCrime);
                }
                
                strcpy(index->chaveBusca, descricaoCrime);
                index->byteOffset = pos_inicial;
                vetor[posicao_vetor] = index;
                posicao_vetor++;
            }

        }

        vetor = realloc(vetor, (posicao_vetor + 1) * sizeof(indice_string_t*));
    
        pos_inicial += tam;
    }

    //Atualiza cabeçalho do registro de indice com o numero de registros que serão escritos 
    cabecalhoIndice->qtdReg = posicao_vetor;
    
    //Ordenação específica no caso em que o campo_indexado é data.
    //Aqui, os $ são adicionados APÓS a ordenação para facilitar
    //sua comparação anterior.
    if(strcmp(campo_indexado, "dataCrime") == 0){
        qsort(vetor, posicao_vetor, sizeof(indice_string_t*), compararIndiceStrings);

        //Depois de ordenados, podemos preencher os campos
        for(int i = 0; i < posicao_vetor; i++){
            preenche_string(vetor[i]->chaveBusca);
        }
    }

    //Em todos os outros campos que são strings, faremos uma ordenação
    //mais genérica.
    else{
        qsort(vetor, posicao_vetor, sizeof(indice_string_t*), compararIndiceStrings);
    } 
    
    
    escreve_indice(arq_index, NULL, vetor, cabecalhoIndice); 

    //Fecha arquivo e encerra indexação
    fechar_arquivoIndice(arq_index, cabecalhoIndice);   
      

}

 
    Lê todos os registros do arquivo de indice (int) e salva em um vetor.
    Parâmetro:
        arq_index->Ponteiro do arquivo de índices

  
indice_inteiro_t **leitura_indicesInteiros(FILE *arq_index, header_indice_t *cabecalho_indicie){
    fread(&(cabecalho_indicie->status), 1, sizeof(char), arq_index);

    //Verifica a consistência do arquivo
    if(cabecalho_indicie->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }
    
    //Leitura da quantidade de registros no arquivo
    fread(&(cabecalho_indicie->qtdReg), 1, sizeof(int), arq_index);
    int qtdReg = cabecalho_indicie->qtdReg;
    
    //Cria vetor de registro de indices de inteiro
    indice_inteiro_t **vetor = cria_vetorIndices_inteiro();

    //Lê registro por registro até o final do arquivo e armazena no vetor
    vetor = realloc(vetor, qtdReg * sizeof(indice_inteiro_t));
    for(int i = 0; i < qtdReg; i++){
        indice_inteiro_t *index = cria_indice_inteiro();
        
        fread(&(index->chaveBusca), 1, sizeof(int), arq_index);
        fread(&(index->byteOffset), 1, sizeof(long int), arq_index);

        vetor[i] = index;
    }

    return vetor;

}

 
    Lê todos os registros do arquivo de indice (string) e salva em um vetor.
    Parâmetro:
        arq_index->Ponteiro do arquivo de índices

  
indice_string_t **leitura_indicesStrings(FILE *arq_index, header_indice_t* cabecalho_indicie){
    fread(&(cabecalho_indicie->status), 1, sizeof(char), arq_index);
    
    //Verifica a consistência do arquivo
    if(cabecalho_indicie->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }
    
    //Leitura da quantidade de registros no arquivo
    fread(&(cabecalho_indicie->qtdReg), 1, sizeof(int), arq_index);
    int qtdReg = cabecalho_indicie->qtdReg;

    //Cria vetor de registro de indices de string
    indice_string_t **vetor = cria_vetorIndices_strings();

    //Lê registro por registro até o final do arquivo e armazena no vetor
    vetor = realloc(vetor, qtdReg * sizeof(indice_string_t));
    for(int i = 0; i < qtdReg; i++){
        indice_string_t *index = cria_indice_string();
        fread(&(index->chaveBusca), 12, sizeof(char), arq_index);
        int j = 0;
        while(index->chaveBusca[j] != '$' && index->chaveBusca[j] != '\0') j++;
        index->chaveBusca[j] = '\0';

        fread(&(index->byteOffset), 1, sizeof(long int), arq_index);
        vetor[i] = index;
    }
    
    return vetor;
}

 
    Compara uma chave fornecida com a chave de busca de um registro de índices (tipo inteiro).
    Retorna o byteoffset se forem iguais e -1 se não forem.
    Parâmetros:
        ind_int -> Registro de índice de inteiro
        num -> Chave a ser comparada

  
long int compara_indicie_inteiro(indice_inteiro_t* ind_int, int num) {
    if (ind_int->chaveBusca == num) 
        return ind_int->byteOffset;

    return -1;
}

 
    Compara uma chave fornecida com a chave de busca de um registro de índices (tipo string).
    Retorna o byteoffset se os 12 primeiros caracteres forem iguais e -1 se não forem.
    Parâmetros:
        ind_str -> Registro de índice de string
        num -> Chave a ser comparada

  
long int compara_indicie_string(indice_string_t* ind_str, char *str) {
    if (strncmp(ind_str->chaveBusca, str, 12) == 0) { 
        return ind_str->byteOffset;
    }
    return -1;
}

 
    Retorna a quantidade de índices salva em um cabeçalho de arquivo de índice
    Parâmetros:
        cabecalho_indicie -> Cabeçalho do arquivo de índice
  
int retorna_quant_indicies(header_indice_t* cabecalho_indicie) {
    return cabecalho_indicie->qtdReg;
}

 
    Insere um novo registro de índices em um vetor já preenchido
    Parâmetros:
        ind_int -> Vetor de registros de índice (inteiro)
        cabecalho_indice -> Cabeçalho do arquivo de índice
        num -> Valor da chave de busca do registro a ser inserido
        byteoffset -> Próximo byteoffset livre no arquivo de índice
  
void adicionar_indice_inteiro(indice_inteiro_t*** ind_int, header_indice_t* cabecalho_indicie, int num, long int byteoffset) {
    if (num == -1) return;
    
    int p;
    for (p = 0; p <= cabecalho_indicie->qtdReg - 1; p++) {
        if ((*ind_int)[p]->chaveBusca > num) {
            break;
        }
    }

    cabecalho_indicie->qtdReg++;
    *ind_int = realloc(*ind_int, (cabecalho_indicie->qtdReg) * sizeof(indice_inteiro_t*));
    
    (*ind_int)[cabecalho_indicie->qtdReg - 1] = cria_indice_inteiro();

    // Shift dos valores
    for (int i = cabecalho_indicie->qtdReg - 1; i > p; i--) {
        ((*ind_int)[i])->chaveBusca = ((*ind_int)[i - 1])->chaveBusca;
        ((*ind_int)[i])->byteOffset = ((*ind_int)[i - 1])->byteOffset;
    }

    ((*ind_int)[p])->chaveBusca = num;
    ((*ind_int)[p])->byteOffset = byteoffset;
}

 
    Insere um novo registro de índices em um vetor já preenchido
    Parâmetros:
        ind_int -> Vetor de registros de índice (string)
        cabecalho_indice -> Cabeçalho do arquivo de índice
        str -> Valor da chave de busca do registro a ser inserido
        byteoffset -> Próximo byteoffset livre no arquivo de índice
  
void adicionar_indice_string(indice_string_t*** ind_str, header_indice_t* cabecalho_indicie, char *str, long int byteoffset) {
    if (str == NULL || strlen(str) == 0) return;
    
    int p;
    for (p = 0; p <= cabecalho_indicie->qtdReg - 1; p++) {
        if (strncmp(((*ind_str)[p])->chaveBusca, str, 12) > 0) {
            break;
        }
    }

    cabecalho_indicie->qtdReg++;
    *ind_str = realloc(*ind_str, (cabecalho_indicie->qtdReg) * sizeof(indice_inteiro_t*));
    
    (*ind_str)[cabecalho_indicie->qtdReg - 1] = cria_indice_string();

    // Shift dos valores
    for (int i = cabecalho_indicie->qtdReg - 1; i > p; i--) {
        strcpy(((*ind_str)[i])->chaveBusca,((*ind_str)[i - 1])->chaveBusca);
        ((*ind_str)[i])->byteOffset = ((*ind_str)[i - 1])->byteOffset;
    }

    strncpy(((*ind_str)[p])->chaveBusca, str, 12);
    ((*ind_str)[p])->byteOffset = byteoffset;
}

int buscabin_int(indice_inteiro_t **vet_int, int tam_vetor, int chave){

    int inicio = 0, fim = tam_vetor-1;
    
    while(inicio <= fim){
        int meio = inicio + (fim - inicio) / 2;

        if(vet_int[meio]->chaveBusca == chave) return meio;

        if(vet_int[meio]->chaveBusca < chave) inicio = meio + 1;

        else fim = meio - 1;
    }

    return -1;

}

int buscabin_str(indice_string_t **vet_str, int tam_vetor, char *chave){

    int inicio = 0, fim = tam_vetor-1;
    
    while(inicio <= fim){
        int meio = inicio + (fim - inicio) / 2;

        if(strncmp(vet_str[meio]->chaveBusca, chave, 12) == 0) return meio;

        if(strncmp(vet_str[meio]->chaveBusca, chave, 12) < 0) inicio = meio + 1;

        else fim = meio - 1;
    }

    return -1;



}




 
    Dado um byteoffset, busca e retorna a posição de um registro em um vetor de registro
    de índice (inteiro ou string)
    Parâmetro:
        vet_int -> Vetor de registro de índice (inteiro)
        vet_str -> Vetor de registro de índice (string)
        cabecalho_indice -> Cabeçalho do arquivo de índice
        chave -> byteoffset do registro

  
int buscaPosicao_indice(indice_inteiro_t **vet_int, indice_string_t **vet_str, int tam_vetor, int chaveInt, char *chaveStr, int long byteoffset){
    
    int posicao;

    if(vet_int != NULL){
        posicao = buscabin_int(vet_int, tam_vetor, chaveInt);
        int posicao_copia = posicao;

        if(posicao != -1){
            while(posicao >= 0 && vet_int[posicao]->chaveBusca == chaveInt){
                if(vet_int[posicao]->byteOffset == byteoffset) return posicao;
                posicao--;
            }

            posicao = posicao_copia + 1;

            while(posicao <= (tam_vetor - 1) && vet_int[posicao]->chaveBusca == chaveInt){
                if(vet_int[posicao]->byteOffset == byteoffset) return posicao;
                posicao++;
            }
            

        }

    }

    else{
        posicao = buscabin_str(vet_str, tam_vetor, chaveStr);
        int posicao_copia = posicao;

        if(posicao != -1){
            while(posicao >= 0 && strncmp(vet_str[posicao]->chaveBusca, chaveStr, 12) == 0){
                if(vet_str[posicao]->byteOffset == byteoffset) return posicao;
                posicao--;

            }

            posicao = posicao_copia + 1;
            while(posicao <= (tam_vetor - 1) && strncmp(vet_str[posicao]->chaveBusca, chaveStr, 12) == 0){
                if(vet_str[posicao]->byteOffset == byteoffset) return posicao;
                posicao++;
            }
        }
    }

    return -1;
}

 
    Deleta o registro do arquivo de índice de forma definitiva
    Parâmetros:
        vetor_int -> Vetor de registros de índice (inteiros)
        vetor_str -> Vetor de registros de índice (strings)
        byteoffset -> ByteOffset do registro no arquivo de índice
        cabecalho -> Cabeçalho do arquivo de índice

  
void deleta_crimeIndice(indice_inteiro_t ***vetor_int, indice_string_t ***vetor_str, int chaveInt, char *chaveStr, long int byteoffset, char *campo_indexado, header_indice_t *cabecalho){
    

    //Se o vetor de inteiros não é nulo, as operações serão feitas nele
    if(*vetor_int != NULL){
        
        //Encontra posição no vetor do registro que será deletado
        int posicao = buscaPosicao_indice(*vetor_int, *vetor_str, cabecalho->qtdReg, chaveInt, chaveStr, byteoffset);
        
        //Caso a posição tenha sido encontrada
        if(posicao != -1){
            //Dentro do loop, iremos shiftar os registros, apagando o que ocupa
            //a posição encontrada do vetor e realocaremos o vetor com o novo tamanho
            for(int i = posicao; i < (cabecalho->qtdReg) - 1; i++){
                ((*vetor_int)[i])->chaveBusca = ((*vetor_int)[i+1])->chaveBusca;
                ((*vetor_int)[i])->byteOffset = ((*vetor_int)[i+1])->byteOffset;
            }

            cabecalho->qtdReg--; //Diminui o número de registros do cabeçalho
            *vetor_int = realloc(*vetor_int, (cabecalho->qtdReg) * sizeof(indice_inteiro_t*));
        }
    }

    //Se o vetor de strings não é nulo, as operações serão feitas nele
    else{
        
        //Encontra posição no vetor do registro que será deletado
        int posicao = buscaPosicao_indice(*vetor_int, *vetor_str, cabecalho->qtdReg, chaveInt, chaveStr, byteoffset);
    
        //Caso a posição tenha sido encontrada
        if(posicao != -1){

            //Dentro do loop, iremos shiftar os registros, apagando o que ocupa
            //a posição encontrada do vetor e realocaremos o vetor com o novo tamanho
            for(int i = posicao; i < (cabecalho->qtdReg) - 1; i++){
                ((*vetor_str)[i])->byteOffset = ((*vetor_str)[i+1])->byteOffset;
                strcpy(((*vetor_str)[i])->chaveBusca, ((*vetor_str)[i+1])->chaveBusca);
            }

            cabecalho->qtdReg--; //Diminui o número de registros do cabeçalho
            *vetor_str = realloc(*vetor_str, (cabecalho->qtdReg) * sizeof(indice_string_t*));
        }

    }

}

 
    Atualiza o arquivo de índice com os registros alterados
    Parâmetros:
        ind_int -> Vetor de registros de índice (inteiro)
        ind_str -> Vetor de registros de índice (string)
        cabecalho -> Cabeçalho do arquivo de índice
        num_antigo -> Conteúdo da chave de busca de antes da alteração do campo, caso o registro seja de inteiro
        str_antigo -> Conteúdo da chave de busca de antes da alteração do campo, caso o registro seja de string
        num -> Conteúdo da chave de busca do registro a ser alterado, caso o registro seja de inteiro
        str -> Conteúdo da chave de busca do registro a ser alterado, caso o registro seja de string
        byteoffset_antigo -> Byteoffset antigo do registro
        byteoffset_novo -> Byteoffset novo
  
void atualiza_indice(indice_inteiro_t*** ind_int, indice_string_t*** ind_str, header_indice_t* cabecalho_indice, int num_antigo, char *str_antigo, int num, char *str, long int byteoffset_antigo, long int byteoffset) {
    // Faz a busca pelo valor usando o a chave e o byteoffset antigo
    int pos = buscaPosicao_indice(*ind_int, *ind_str, cabecalho_indice->qtdReg, num_antigo, str_antigo, byteoffset_antigo);

    // Se achou no arquivo de índice
    if (pos != -1) {
        // Atualiza a chave busca
        if (*ind_int != NULL){
            ((*ind_int)[pos])->chaveBusca = num;
        } else { 
            strncpy((*ind_str)[pos]->chaveBusca, str, 13);
        }

        // Atualiza o byteoffset se for diferente. Senão, deixa o byteoffset como tá
        if (byteoffset_antigo != byteoffset) {
            if (*ind_int != NULL) (*ind_int)[pos]->byteOffset = byteoffset;
            else (*ind_str)[pos]->byteOffset = byteoffset;
        } else {
            if (*ind_int != NULL) (*ind_int)[pos]->byteOffset = byteoffset_antigo;
            else (*ind_str)[pos]->byteOffset = byteoffset_antigo;
        }
    } else { // Se não achou no indice, criar im indice novo.
        if (*ind_int != NULL)adicionar_indice_inteiro(ind_int, cabecalho_indice, num, byteoffset_antigo);
        else adicionar_indice_string(ind_str, cabecalho_indice, str, byteoffset_antigo); 
    }

    // Ordenar o vetor de indice
    if (*ind_int != NULL) qsort(*ind_int, cabecalho_indice->qtdReg, sizeof(indice_inteiro_t*), compararIndiceInteiros);
    else qsort(*ind_str, cabecalho_indice->qtdReg, sizeof(indice_string_t*), compararIndiceStrings);
}


 
    Desaloca o vetor de índices.
    Parâmetros:
        ind_int -> Vetor de registros de índice (inteiro)
        ind_str -> Vetor de registros de índice (inteiro)
        cabecalho -> Cabeçalho do arquivo de índice
        
  
void destruir_indices(indice_inteiro_t** ind_int, indice_string_t** ind_str, header_indice_t* cabecalho_indice) {
    for (int i = 0; i < cabecalho_indice->qtdReg; i++) {
        if (ind_int != NULL) free(ind_int[i]);
        else free(ind_str[i]);
    }

    if (ind_int != NULL) free(ind_int);
    else free(ind_str);
} */