/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdio.h>
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
    table = (struct table_t*) calloc(1,sizeof(struct table_t));
    table->size = n;
    table->lists = (struct list_t**) calloc(1,sizeof(struct list_t*)*n); 

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

    int status = 0;
    for(int i=0; i<table->size; i++){
        status = (status || list_destroy(table->lists[i])); 
        if (status != 0) return status;
    }

    free(table->lists);
    free(table);

    return 0;
}

/* Função que calcula o índice da lista a partir da chave
 */
int hash_code(char *key, int n){
   /*  https://stackoverflow.com/questions/2624192/good-hash-function-for-strings
    *  31 was chosen bc of a section from Joshua Bloch's Effective Java book.
    *  It's also used in JAVA's string hash func.
    */

    // FIXME Implement this function without the use of abs to patch
    unsigned long long result = 7;
    for (int i = 0; i < strlen(key); i++){
        result = result * 31 + key[i];
    }

    result = (int) result % n;
    return abs((int) result);
}

/* Função para adicionar um par chave-valor à tabela. Os dados de entrada
 * desta função deverão ser copiados, ou seja, a função vai criar uma nova
 * entry com *CÓPIAS* da key (string) e dos dados. Se a key já existir na
 * tabela, a função tem de substituir a entry existente na tabela pela
 * nova, fazendo a necessária gestão da memória.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value){

    if(!table || !key || !value) return -1;
    
    int index = hash_code(key, table->size);
    char* dup_key = strdup(key);
    struct data_t* dup_value = data_dup(value);
    struct entry_t *entry = entry_create(dup_key, dup_value);

    int result = list_add(table->lists[index], entry);
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
    struct entry_t *entry = list_get(table->lists[index],key);
    if(!entry) return NULL;
    else return data_dup(entry->value);
}

/* Função que remove da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int table_remove(struct table_t *table, char *key){

    if(!table || !key) return -1;
    
    int index = hash_code(key, table->size);
    return list_remove(table->lists[index],key);
}

/* Função que conta o número de entries na tabela passada como argumento.
 * Retorna o tamanho da tabela ou -1 em caso de erro.
 */
int table_size(struct table_t *table){
    if(!table->size || !table) return -1; 
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
    if(!table) return NULL;
    char **key_arr = (char**) calloc(table_size(table) + 1, sizeof(char*));

    int write_index = 0;
    for (int i = 0; i < table->size; i++) {
        char** list_keys = list_get_keys(table->lists[i]);
        if (!list_keys) continue;

        for (int j = 0; j < list_size(table->lists[i]); j++) {
            key_arr[write_index] = strdup(list_keys[j]);
            write_index++;
        }

        list_free_keys(list_keys);
    }

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
    }
    free(keys);
    return 0;
}
