#include <stdlib.h>
#include <string.h>

#include "table.h"

/* Função para criar e inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash).
 * Retorna a tabela ou NULL em caso de erro.
 */
struct table_t *table_create(int n){
    if (n<=0) return NULL;
    struct table_t *table;
    table = (struct table_t*) malloc(sizeof(struct table_t));
    table->size = n;
    table->lists = (struct list_t**) malloc(sizeof(struct list_t*)*n); 

    for (int i = 0; i < n; i++){
        table->lists[i] = list_create();
        if (!table->lists[i]){
            table_destroy(table);
            return NULL;
        }
    }

    return table;
}

/* Função que elimina uma tabela, libertando *toda* a memória utilizada
 * pela tabela.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_destroy(struct table_t *table){

    if(!table){
        return -1;
    }

    if(!table->lists[0]){

        free(table->lists);
        free(table);

        return 0;
    }

    for(int i=0; i<table->size; i++){
        list_destroy(table->lists[i]);
    }

    free(table->lists);
    free(table);

    return 0;
}

/* Função que calcula o índice da lista a partir da chave
 */
int hash_code(char *key, int n){
    //does something, e.g.
    int hashed = strlen(key);
    //we can workshop a better hash code to use

    return hashed%n;
}

/* Função para adicionar um par chave-valor à tabela. Os dados de entrada
 * desta função deverão ser copiados, ou seja, a função vai criar uma nova
 * entry com *CÓPIAS* da key (string) e dos dados. Se a key já existir na
 * tabela, a função tem de substituir a entry existente na tabela pela
 * nova, fazendo a necessária gestão da memória.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value){
    int index = hash_code(key, table->size);
    struct entry_t *entry = entry_create(strdup(key),data_dup(value));
    int result = list_add(table->lists[index], entry_dup(entry));
    if (result == 1) { //aka: substituted data
        return 0;
    }else{
        return result;
    }
}

/* Função que procura na tabela uma entry com a chave key. 
 * Retorna uma *CÓPIA* dos dados (estrutura data_t) nessa entry ou 
 * NULL se não encontrar a entry ou em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key){

    if(!table || !key) return NULL;

    int index = hash_code(key, table->size);
    char** keys = list_get_keys(table->lists[index]);

    for(int i=0;i<table->lists[index]->size;i++){
        
        if (strcmp(keys[i], key) == 0) {

            struct entry_t* entry = list_get(table->lists[index], key);
            return data_dup(entry->value);
        }
    }

    return NULL;
}

/* Função que remove da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int table_remove(struct table_t *table, char *key){
    int index = hash_code(key, table->size);
    return list_remove(table->lists[index],key);
}

/* Função que conta o número de entries na tabela passada como argumento.
 * Retorna o tamanho da tabela ou -1 em caso de erro.
 */
int table_size(struct table_t *table){
    if(!table->size || !table) return -1; //eh why not keep it

    if(table->lists[0] == NULL){
        return 0;
    }

    int entry_count = 0;
    for(int i=0; i<table->size; i++){
        int result = list_size(table->lists[i]);
        if (result == -1) return result;
        entry_count += result;
    }
    return entry_count;
}

/* Função que constrói um array de char* com a cópia de todas as keys na 
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char **table_get_keys(struct table_t *table){

    if (table == NULL) return NULL;

    //allocate this array
    int total_keys = 0;
    char **key_arr = (char **)malloc(sizeof(char*));

    for(int i=0; i<(table->size); i++){
    
        char **keys = list_get_keys(table->lists[i]);

        int list_size = 0;
        while(!keys[list_size]){
            list_size++;
        }

        key_arr = (char **)realloc(key_arr, (total_keys + list_size + 1) * sizeof(char *));

        for (int j = 0; j < list_size; j++){
            key_arr[total_keys++] = keys[j];
        }

        free(keys);
    }

    key_arr[total_keys] = NULL;

    return key_arr;
}

/* Função que liberta a memória ocupada pelo array de keys obtido pela 
 * função table_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_free_keys(char **keys){
    if(!keys) return -1;

    size_t i = 0;
    while(keys[i]){
        free(keys[i]);
        i++;
    // Q: what if an error occurs during the freeing of one of the keys?
    // wouldn't it just assume it'd reached the end and fail to free correctly?
    }
    free(keys);
    return 0;
}
