#include <stdlib.h>
#include <string.h>


/* Módulo list */
#include "data.h"
#include "entry.h"


struct entry_t *entry_create(char *key, struct data_t *data){
    struct entry_t* entry;
    entry = (struct entry_t*) calloc(1,sizeof(struct entry_t));
    if (entry == NULL || key == NULL || data == NULL)
        return NULL;
    entry->key = key;
    entry->value = data;
    return entry;
}


int entry_destroy(struct entry_t *entry){
    if(entry == NULL)
        return -1;
    free(entry);
    return 0;
}


struct entry_t *entry_dup(struct entry_t *entry){
    return entry_create(entry->key,entry->value);
}

int entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    if(entry == NULL || new_key == NULL || new_value == NULL)
        return -1;
    free(entry->value);
    free(entry->key);
    entry->value = new_value;
    entry->key = new_key;
    return 0;
}


int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    int result = strcmp(entry1->key,entry2->key);
    if (result == NULL)
        return -2;
    else{
        if (result > 0)
            return 1;
        else if (result < 0)
            return -1;
        else
            return 0;
    }
}
