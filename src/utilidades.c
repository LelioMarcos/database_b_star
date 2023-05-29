#include "../header/indice.h"
#include "../header/utilidades.h"
#include "../header/funcoesFornecidas.h"
#include "../header/crime.h"
#include "../header/arquivo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/


void preenche_string(char *str){
    int tamanho = strlen(str);
    
    //Preenche os espaços vazios da string com $
    //e adiciona \0 no final
    for(int i = tamanho; i < 12; i++){
        str[i] = '$';
    }
    str[12] = '\0';


}

char *trunca_strings(char *str){
    str[12] = '\0';
    return str;

}

