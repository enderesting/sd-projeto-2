#include <stdlib.h>
#ifndef _LIST_H
#define _LIST_H /* Módulo list */

#include "data.h"
#include "entry.h"

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
struct entry_t *entry_create(char *key, struct data_t *data){
    struct entry_t* entry;
    entry = (struct entry_t*) calloc(1,sizeof(struct entry_t));
    if (entry == NULL || key == NULL || data == NULL)
        return NULL;
    entry->key = key;
    entry->value = data;
    return entry;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_destroy(struct entry_t *entry){
    if(entry == NULL)
        return -1;
    free(entry);
    return 0;
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t *entry_dup(struct entry_t *entry){
    // TODO... Unsure about this
}

/* Função que substitui o conteúdo de uma entry, usando a nova chave e
 * o novo valor passados como argumentos, e eliminando a memória ocupada
 * pelos conteúdos antigos da mesma.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    if(entry == NULL || new_key == NULL || new_value == NULL)
        return -1;
    free(entry->value);
    free(entry->key);
    entry->value = new_value;
    entry->key = new_key;
    return 0;
    //Q: wouldn't it be the same as destroying an entry and recreating it?
    //   or would entry ptr has to be the same? therefore it can't be a new entry?
}

/* Função que compara duas entries e retorna a ordem das mesmas, sendo esta
 * ordem definida pela ordem das suas chaves.
 * Retorna 0 se as chaves forem iguais, -1 se entry1 < entry2,
 * 1 se entry1 > entry2 ou -2 em caso de erro.
 */
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    //TODO. im sleepy
}

#endif