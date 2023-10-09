/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdlib.h>
#include <string.h>

#include "entry.h"


struct entry_t *entry_create(char *key, struct data_t *data){
    if (key == NULL || data == NULL) return NULL;

    struct entry_t* entry;
    entry = (struct entry_t*) calloc(1,sizeof(struct entry_t));

    if (entry == NULL) return NULL;

    entry->key = key;
    entry->value = data;
    return entry;
}


int entry_destroy(struct entry_t *entry){
    if(entry == NULL || entry->key == NULL || entry->value == NULL)
        return -1;

    free(entry->key);
    data_destroy(entry->value);
    free(entry);
    return 0;
}


struct entry_t *entry_dup(struct entry_t *entry){
    if(entry == NULL || entry->key == NULL || entry->value == NULL)
        return NULL;

    char *key_copy = (char*) calloc(1,strlen(entry->key)+1);
    strcpy(key_copy,entry->key);

    struct data_t *value_copy = data_dup(entry->value);

    return entry_create(key_copy,value_copy);
}

int entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    if(entry == NULL || new_key == NULL || new_value == NULL)
        return -1;

    data_destroy(entry->value);
    free(entry->key);
    entry->value = new_value;
    entry->key = new_key;
    return 0;
}


int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    
    if (!entry1 || !entry2 || !entry1->key || !entry2->key)
        return -2;
    else{
        int result = strcmp(entry1->key,entry2->key);
        if (result > 0)
            return 1;
        else if (result < 0)
            return -1;
        else
            return 0;
    }
}
