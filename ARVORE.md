# Árvore-B*

## O que é?
Uma Árvore-B* é um tipo de [Árvore-B](https://pt.wikipedia.org/wiki/%C3%81rvore_B) na qual algumas regras são postas para os seus nós. Considerando um número inteiro _m > 1_ qualquer:

1. Cada página do disco apresenta no máximo m páginas filhas
2. O número de chaves em uma página deve ser entre _⌊(2m-1)/3⌋_ e _m-1_
3. Todas as páginas folha estão no mesmo nível
4. Toda página, exceto a raiz e as folhas possuem no máximo _(2m-1)/3_ descendentes
5. Uma página não folha com k descendentes possui k-1 chaves

Por exemplo, se escolhermos m = 5 (como feito nesse trabalho), cada nó folha deverá ter no mínimo 3  chaves (_⌊(2*5-1)/3⌋ = 3_) e no máximo 4 chaves. Também terá no máximo 5 páginas filhas.

OBS.: a quantidade mínima de chaves só não será respeitada nas 5 primeiras inserções na árvore.

## Algoritmos
Segue uma base para os algoritmos de busca e insersão.

### Busca
```
busca(A, rrn, item)
    se rrn == -1:
        retorna -1
    
    no <- ler_no_arquivo(A, rrn)

    pos <- busca_binaria(no, item)

    se (no.chaves[pos] == item)
        retorna item
    senão se (no.chaves[pos] > item)
        retorna busca(A, A.descendentes[pos + 1], item)
    senão
        retorna busca(A, descendentes[pos], item)

```

## Referências
- [https://pt.wikipedia.org/wiki/%C3%81rvore_B*](https://pt.wikipedia.org/wiki/%C3%81rvore_B*)
- FOLK, Michael; ZOELLICK, Bill; File Structures
