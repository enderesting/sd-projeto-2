#include <stdlib.h>
#include <string.h>


#include "entry.h"
#include "list-private.h"
#include "list.h"

struct list_t *list_create() {
    struct list_t *list = (struct list_t *) calloc(1, sizeof(struct list_t));
    return list;
}

int list_destroy(struct list_t *list) {
    struct node_t* node = list->head;
    struct node_t* next_node;

    while (node != 0) {
        next_node = node->next;
        entry_destroy(node->entry);
        free(node);
        node = next_node;
    }

    free(list);
    return 0;
}

int list_add(struct list_t *list, struct entry_t *entry) {
    // TODO extract new_node creation to a separate function
    // TODO refactor this function
    // TODO update list size if a node was added

    if (!list->head) {
        struct node_t* new_node =
            (struct node_t*) calloc(1, sizeof(struct node_t));
        new_node->entry = entry;
        new_node->next = NULL;
        list->head = new_node;
        return 0;
    }

    struct node_t* node = list->head;
    int cmp_ret = entry_compare(entry, node->entry);

    if (cmp_ret == 0) {
        if (entry_replace(node->entry, entry->key, entry->value) == -1) {
            return -1;
        } else {
            return 1;
        }
    } else if (cmp_ret == -1) {
        struct node_t *new_node =
            (struct node_t *)calloc(1, sizeof(struct node_t));
        new_node->entry = entry;
        new_node->next = node;
        list->head = new_node;
        return 0;
    }

    struct node_t *previous_node;
    while (node->next != 0) {
        previous_node = node;
        node = node->next;
        cmp_ret = entry_compare(entry, node->entry);

        if (cmp_ret == 0) {
            if (entry_replace(node->entry, entry->key, entry->value) == -1) {
                return -1;
            } else {
                return 1;
            }
        } else if (cmp_ret == -1) {
            struct node_t *new_node =
                (struct node_t *) calloc(1, sizeof(struct node_t));
            new_node->entry = entry;
            new_node->next = node;
            previous_node->next = new_node;
            return 0;
        }
    }

    struct node_t *new_node = (struct node_t *) calloc(1, sizeof(struct node_t));
    new_node->entry = entry;
    new_node->next = node;
    previous_node->next = new_node;
    return 0;
}

int list_remove(struct list_t *list, char *key) {
    // TODO update list size if element was removed
    if (!list->head) {
        return 1;
    }

    int cmp_ret = strcmp(list->head->entry->key, key);
    if (!cmp_ret) {
        list->head = list->head->next;
        return entry_destroy(list->head->entry);
    }

    struct node_t* previous_node = list->head;
    struct node_t* node = previous_node->next;

    while (node != 0) {
        int cmp_ret = strcmp(node->entry->key, key);
        if (!cmp_ret) {
            previous_node->next = node->next;
            return entry_destroy(list->head->entry);
        }

        previous_node = node;
        node = node->next;
    }

    return 1;
}

struct entry_t *list_get(struct list_t *list, char *key) {
    struct node_t* node = list->head;
    while (node != 0) {
        if (strcmp(node->entry->key, key) == 0) {
            return node->entry;
        }

        node = node->next;
    }

    return NULL;
}

int list_size(struct list_t *list) {
    int count = 0;
    struct node_t* node = list->head;
    while (node != 0) {
        count++;
        node = node->next;
    }

    return count;
}

char **list_get_keys(struct list_t *list) {
    // TODO implement a way to enforce that strings are terminated
}
