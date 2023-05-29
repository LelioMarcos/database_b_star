#include "../header/readline.h"
#include <stdio.h>
#include <stdlib.h>

/*
    Trabalho 1 de Organização de Arquivos
    Alunos: 
        Rafael C. G. Conrado - 13671806
        Lélio Marcos Rangel Cunha - 13673148

*/

char *readline(FILE *stream) {
    char *str = NULL;
    int n_char = 0;

    while(1) {
        char c = fgetc(stream);
        if (c == '\r') continue;

        str = realloc(str, sizeof(char) * (n_char + 1));

        if (c == '\n' || c == EOF) {
            str[n_char] = '\0';
            break;
        }

        str[n_char++] = c;
    }

    return str;
}
