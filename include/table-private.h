#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t {
	struct list_t **lists;
	int size;
};

/* Função que calcula o índice da lista a partir da chave
 */
int hash_code(char *key, int n);

/* Função que constrói um array de entry_t* com a cópia de todas as entries na 
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
struct entry_t **table_get_entries(struct table_t *table);
#endif