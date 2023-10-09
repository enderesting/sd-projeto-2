/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

/* Função que cria e inicializa uma nova lista (estrutura list_t a
 * ser definida pelo grupo no ficheiro list-private.h).
 * Retorna a lista ou NULL em caso de erro.
 */
struct list_t *list_create() {
    struct list_t *list = (struct list_t *) calloc(1, sizeof(struct list_t));
    return list;
}

/* Função que elimina uma lista, libertando *toda* a memória utilizada
 * pela lista.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int list_destroy(struct list_t *list) {
    if (list == NULL) return -1;

    struct node_t* node = list->head;
    struct node_t* next_node;
    int result = 0;

    if (list == NULL) result = -1;
    while (node) {
        next_node = node->next;
        node_destroy(node);
        node = next_node;
    }

    free(list);
    return result;
}

/* Creates a new node with given entry and next node.
 *
 * Returns the created node's pointer, or the null pointer in case the creation's
 * failed.
 */
struct node_t* node_create(struct entry_t* entry, struct node_t* next) {
    struct node_t* node = (struct node_t *)calloc(1, sizeof(struct node_t));
    node->entry = entry;
    node->next = next;
    return node;
}

/*
** Frees up all the memory occupied by a given node
**
** Returns 0 if the operation was successful or -1 in case of error
*/
int node_destroy(struct node_t* node) {
   if (!node) return -1;

   entry_destroy(node->entry);
   free(node);

   return 0;
}

/* Função que adiciona à lista a entry passada como argumento.
 * A entry é inserida de forma ordenada, tendo por base a comparação
 * de entries feita pela função entry_compare do módulo entry e
 * considerando que a entry menor deve ficar na cabeça da lista.
 * Se já existir uma entry igual (com a mesma chave), a entry
 * já existente na lista será substituída pela nova entry,
 * sendo libertada a memória ocupada pela entry antiga.
 * Retorna 0 se a entry ainda não existia, 1 se já existia e foi
 * substituída, ou -1 em caso de erro.
 */

int list_add(struct list_t *list, struct entry_t *entry) {
    if (!list || !entry) return -1;

    if (!list->head) {
        struct node_t* new_node = node_create(entry, NULL);
        if (!new_node) return -1;
        list->head = new_node;
        list->size = list->size + 1;
        return 0;
    }

    struct node_t* node = list->head;
    int cmp_ret = entry_compare(entry, node->entry);

    if (cmp_ret == ENTRY_CMP_ERROR) return -1;

    if (cmp_ret == ENTRY_EQUAL) {
        entry_destroy(node->entry);
        node->entry = entry;
        return 1;
    }

    if (cmp_ret == ENTRY_LESSER) {
        struct node_t* new_node = node_create(entry, node);
        if (!new_node) return -1;
        list->head = new_node;
        list->size = list->size + 1;
        return 0;
    }

    struct node_t* previous_node = node;
    node = node->next;
    while (node) {
        int cmp_ret = entry_compare(entry, node->entry);

        if (cmp_ret == ENTRY_CMP_ERROR) return -1;

        if (cmp_ret == ENTRY_EQUAL) {
            entry_destroy(node->entry);
            node->entry = entry;
            return 1;
        }

        if (cmp_ret == ENTRY_LESSER) {
            struct node_t* new_node = node_create(entry, node);
            if (!new_node) return -1;
            previous_node->next = new_node;
            list->size = list->size + 1;
            return 0;
        }

        previous_node = node;
        node = node->next;
    }

    struct node_t *new_node = node_create(entry, NULL);
    if (!new_node) return -1;
    previous_node->next = new_node;
    list->size = list->size + 1;
    return 0;
}

/* Função que elimina da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int list_remove(struct list_t *list, char *key) {

    if (!list || !key) return -1;

    if (!list->head) return 1;

    int cmp_ret = strcmp(list->head->entry->key, key);
    if (!cmp_ret) {
        struct node_t* old_node = list->head;
        list->head = list->head->next;

        int ret = entry_destroy(old_node->entry);
        if (ret) return -1;

        free(old_node);
        list->size = list->size - 1;
        return ret;
    }

    struct node_t* previous_node = list->head;
    struct node_t* node = previous_node->next;

    while (node) {
        cmp_ret = strcmp(node->entry->key, key);
        if (!cmp_ret) {
            struct node_t *old_node = node;
            previous_node->next = old_node->next;

            int ret = entry_destroy(old_node->entry);
            if (ret) return -1;

            free(old_node);
            list->size = list->size - 1;
            return ret;
        }

        previous_node = node;
        node = node->next;
    }

    return 1;
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL se não encontrar a
 * entry ou em caso de erro.
 */
struct entry_t *list_get(struct list_t *list, char *key) {
    if (!list || !key) return NULL;

    struct node_t* node = list->head;
    while (node) {
        if (strcmp(node->entry->key, key) == 0) {
            return node->entry;
        }

        node = node->next;
    }

    return NULL;
}

/* Função que conta o número de entries na lista passada como argumento.
 * Retorna o tamanho da lista ou -1 em caso de erro.
 */
int list_size(struct list_t *list) {
    if (!list) return -1;

    int count = 0;
    struct node_t* node = list->head;
    while (node) {
        count++;
        node = node->next;
    }

    return count;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
 * lista, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char **list_get_keys(struct list_t *list) {
    if (!list || !list->head) return NULL;

    char** keys_array = (char**) calloc(list->size + 1, sizeof(char*));

    int i = 0;
    char* key_ptr;
    struct node_t* node = list->head;
    while (node) {
        key_ptr = (char*) calloc(1, sizeof(char) * strlen(node->entry->key) + 1);

        keys_array[i] = key_ptr;
        strcpy(key_ptr, node->entry->key);

        node = node->next;
        i++;
    }
    keys_array[i] = NULL;

    return keys_array;
}

/* Função que liberta a memória ocupada pelo array de keys obtido pela
 * função list_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int list_free_keys(char **keys) {
    if (!keys) return -1;

    int i = 0;
    char *key_ptr = keys[i];
    while (key_ptr != NULL) {
        free(key_ptr);
        i++;
        key_ptr = keys[i];
    }

    free(keys);
    return 0;
}
