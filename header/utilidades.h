#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Preenche strings com menos de 12 caracteres com $
    Parâmetro:
        str -> string a ser preenchida
*/
void preenche_string(char *str);


/*
    Trunca strings que ultrapassam 12 caracteres.
    Parâmetro: 
        str -> string a ser truncada

*/
char *trunca_strings(char *str);


