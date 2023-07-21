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

## Inserções
Uma inserção em uma Árvore-B* sempre acontecerá em um nó folha. Deve-se procurar um nó folha ideal para inserir esssa chave, isto é, descer a árvore de acordo com o valor que deseja inserir e encontrar o nó folha em que esse valor deve estar. Antes de inserir o valor, é preciso verificar se o nó está cheio e, se estiver, é preciso verificar o seguinte:
- Se for um nó raiz, realizar um [Split1-2](#split1-2);
- Senão, verificar se o filho da esquerda e o da direta, nesta ordem, não estão cheios. Se não estiverem, realizar uma [Redistribuição](#redistribuicao) com esse filho;
- Senão, realizar um [Split2-3](#split2-3) com o filho da direita (verificando se esse filho existe) ou da esquerda (caso o anterior não exista).  

## <a id="split1-2"></a>Split 1-2
Nesse split, deve-se criar um vetor em que estará, em ordem, os valores que estão no nó e o novo valor que deseja inserir. Com esse vetor, é preciso separa o vetor em 3 partes de forma que exista apenas 1 valor no meio desse vetor. Com cada uma dessas partes, adiciona ao nó os valores a esquerda, e cria-se 2 novos nós que serão, respectivamente:
- A nova raíz, onde o valor no meio do vetor estará; e
- Um irmão a direita, onde os valores a direita do vetor estarão.

Após isso, define na chave que está nó raiz os seus filhos (o nó e o seu novo irmão).

## <a id="redistribuicao"></a>Redistribuição

## <a id="split2-3"></a>Split 2-3


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
