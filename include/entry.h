#ifndef _ENTRY_H
#define _ENTRY_H /* Módulo entry */

#include "data.h"

/* Esta estrutura define o par {chave, valor} para a tabela
 */
struct entry_t {
	char *key;	/* string, cadeia de caracteres terminada por '\0' */
	struct data_t *value; /* Bloco de dados */
};

/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados de entrada.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t *entry_create(char *key, struct data_t *data);

/* Função que elimina uma entry, libertando a memória por ela ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_destroy(struct entry_t *entry);

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t *entry_dup(struct entry_t *entry);

/* Função que substitui o conteúdo de uma entry, usando a nova chave e
 * o novo valor passados como argumentos, e eliminando a memória ocupada
 * pelos conteúdos antigos da mesma.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value);

/* Função que compara duas entries e retorna a ordem das mesmas, sendo esta
 * ordem definida pela ordem das suas chaves.
 * Retorna 0 se as chaves forem iguais, -1 se entry1 < entry2,
 * 1 se entry1 > entry2 ou -2 em caso de erro.
 */
int entry_compare(struct entry_t *entry1, struct entry_t *entry2);

#endif