#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "list.h"
#include "data.h"
#include "entry.h"

/**************************************************************/
void pee(const char *msg)
{
	perror(msg);
	exit(0);
}

/**************************************************************/
int testEmptyList() {
	struct list_t *list = list_create();

	printf("Módulo list -> testEmptyList: ");
	fflush(stdout);

	int result = (list != NULL) && (list_size(list) == 0);

	list_destroy(list);
	
	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testAddFirst() {
	int result;
	char *key = strdup("abc");
	struct data_t *value = data_create(4, strdup("1234"));
	struct entry_t *entry = entry_create(key, value);
	struct list_t *list = list_create();

	printf("Módulo list -> testAddFirst: ");
	fflush(stdout);

	assert(list_add(NULL, entry) == -1);
	result = (list_add(NULL, entry) == -1);

	assert(list_add(list, NULL) == -1);
	result = result && (list_add(list, NULL) == -1);
	result = result && (list_add(list, entry) == 0);
	result = result && (list_get(list,"abc") == entry);
	result = result && (list_size(list) == 1);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testAdd123RemoveHead() {
	int result;
	char *key1 = strdup("abc1");
	char *key2 = strdup("abc2");
	char *key3 = strdup("abc3");
	struct data_t *value1 = data_create(3, strdup("123"));
	struct data_t *value2 = data_create(3, strdup("456"));
	struct data_t *value3 = data_create(3, strdup("789"));
	struct entry_t *entry1 = entry_create(key1, value1);
	struct entry_t *entry2 = entry_create(key2, value2);
	struct entry_t *entry3 = entry_create(key3, value3);
	struct list_t *list = list_create();

	printf("Módulo list -> testAdd123RemoveHead: ");
	fflush(stdout);

	result = (list_add(list, entry1) == 0);
	result = result && (list_add(list, entry2) == 0);
	result = result && (list_add(list, entry3) == 0);

	result = result && (list_remove(list, "abc1") == 0);
	result = result && (list_get(list, "abc2") == entry2);
	result = result && (list_get(list, "abc3") == entry3);
	result = result && (list_size(list) == 2);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testAdd321RemoveMiddle() {
	int result;
	char *key1 = strdup("abc1");
	char *key2 = strdup("abc2");
	char *key3 = strdup("abc3");
	struct data_t *value1 = data_create(3, strdup("123"));
	struct data_t *value2 = data_create(3, strdup("456"));
	struct data_t *value3 = data_create(3, strdup("789"));
	struct entry_t *entry1 = entry_create(key1, value1);
	struct entry_t *entry2 = entry_create(key2, value2);
	struct entry_t *entry3 = entry_create(key3, value3);
	struct list_t *list = list_create();

	printf("Módulo list -> testAdd321RemoveMiddle: ");
	fflush(stdout);

	result = (list_add(list, entry3) == 0);
	result = result && (list_add(list, entry2) == 0);
	result = result && (list_add(list, entry1) == 0);

	result = result && (list_remove(list, "abc2") == 0);
	result = result && (list_get(list, "abc1") == entry1);
	result = result && (list_get(list, "abc3") == entry3);
	result = result && (list_size(list) == 2);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testAdd132RemoveTail() {
	int result;
	char *key1 = strdup("abc1");
	char *key2 = strdup("abc2");
	char *key3 = strdup("abc3");
	struct data_t *value1 = data_create(3, strdup("123"));
	struct data_t *value2 = data_create(3, strdup("456"));
	struct data_t *value3 = data_create(3, strdup("789"));
	struct entry_t *entry1 = entry_create(key1, value1);
	struct entry_t *entry2 = entry_create(key2, value2);
	struct entry_t *entry3 = entry_create(key3, value3);
	struct list_t *list = list_create();

	printf("Módulo list -> testAdd132RemoveTail: ");
	fflush(stdout);

	result = (list_add(list, entry1) == 0);
	result = result && (list_add(list, entry3) == 0);
	result = result && (list_add(list, entry2) == 0);

	result = result && (list_remove(list, "abc2") == 0);
	result = result && (list_get(list, "abc1") == entry1);
	result = result && (list_get(list, "abc3") == entry3);
	result = result && (list_size(list) == 2);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testAdd123RemoveOther() {
	int result;
	char *key1 = strdup("abc1");
	char *key2 = strdup("abc2");
	char *key3 = strdup("abc3");
	struct data_t *value1 = data_create(3, strdup("123"));
	struct data_t *value2 = data_create(3, strdup("456"));
	struct data_t *value3 = data_create(3, strdup("789"));
	struct entry_t *entry1 = entry_create(key1, value1);
	struct entry_t *entry2 = entry_create(key2, value2);
	struct entry_t *entry3 = entry_create(key3, value3);
	struct list_t *list = list_create();

	printf("Módulo list -> testAdd123RemoveOther: ");
	fflush(stdout);

	result = (list_add(list, entry1) == 0);
	result = result && (list_add(list, entry2) == 0);
	result = result && (list_add(list, entry3) == 0);

	result = result && (list_remove(list, "other") == 1);
	result = result && (list_size(list) == 3);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testInsertDupKey() {
	int result;
	char *key1 = strdup("abc1");
	char *key2 = strdup("abc2");
	char *key3 = strdup("abc3");
	struct data_t *value1 = data_create(3, strdup("123"));
	struct data_t *value2 = data_create(3, strdup("456"));
	struct data_t *value3 = data_create(3, strdup("789"));
	struct entry_t *entry1 = entry_create(key1, value1);
	struct entry_t *entry2 = entry_create(key2, value2);
	struct entry_t *entry3 = entry_create(key3, value3);
	struct entry_t *entry4 = entry_dup(entry1);
	struct entry_t *entry5 = entry_dup(entry1);
	struct entry_t *entry6 = entry_dup(entry3);
	struct list_t *list = list_create();

	printf("Módulo list -> testInsertDupKey: ");
	fflush(stdout);

	result = (list_add(list, entry1) == 0);
	result = result && (list_add(list, entry2) == 0);
	result = result && (list_add(list, entry3) == 0);
	result = result && (list_get(list, "abc1") == entry1);
	result = result && (list_get(list, "abc2") == entry2);
	result = result && (list_add(list, entry4) == 1);
	result = result && (list_get(list, "abc1") == entry4);
	result = result && (list_add(list, entry5) == 1);
	result = result && (list_get(list, "abc1") == entry5);
	result = result && (list_add(list, entry6) == 1);
	result = result && (list_get(list, "abc2") == entry2);
	result = result && (list_get(list, "abc3") == entry6);
	result = result && (list_size(list) == 3);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testGetKeys() {
	int result;
	char *key1 = strdup("abc1");
	char *key2 = strdup("abc2");
	char *key3 = strdup("abc3");
	char **keys;
	struct data_t *value1 = data_create(3, strdup("123"));
	struct data_t *value2 = data_create(3, strdup("456"));
	struct data_t *value3 = data_create(3, strdup("789"));
	struct entry_t *entry1 = entry_create(key1, value1);
	struct entry_t *entry2 = entry_create(key2, value2);
	struct entry_t *entry3 = entry_create(key3, value3);
	struct list_t *list = list_create();

	printf("Módulo list -> testGetKeys: ");
	fflush(stdout);

	assert(list_get_keys(list) == NULL);
	result = (list_get_keys(list) == NULL);

	list_add(list, entry1);
	list_add(list, entry2);
	list_add(list, entry3);

	assert(list_get_keys(NULL) == NULL);
	result = result && (list_get_keys(NULL) == NULL);

	keys = list_get_keys(list);

	result = result && (strcmp(keys[0], entry1->key) == 0) && (keys[0] != entry1->key)
                        && (strcmp(keys[1], entry2->key) == 0) && (keys[1] != entry2->key) 
                        && (strcmp(keys[2], entry3->key) == 0) && (keys[2] != entry3->key)
                        && keys[3] == NULL;

	assert(list_free_keys(NULL) == -1);
	result = result && (list_free_keys(NULL) == -1);

	result = result && (list_free_keys(keys) == 0);

	list_destroy(list);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int main() {
	int score = 0;

	printf("\nIniciando teste do módulo list \n");

	score += testEmptyList();

	score += testAddFirst();
	
	score += testAdd123RemoveHead();

	score += testAdd321RemoveMiddle();

	score += testAdd132RemoveTail();

	score += testAdd123RemoveOther();

	score += testInsertDupKey();

	score += testGetKeys();

	printf("teste list (score): %d/8\n", score);

	if (score == 8)
        	return 0;
	else
        	return -1;
}
