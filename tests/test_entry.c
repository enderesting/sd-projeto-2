#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "entry.h"
#include "data.h"

/**************************************************************/
void pee(const char *msg)
{
	perror(msg);
	exit(0);
}

/**************************************************************/
int testCreate() {
	int result;
	char *key = strdup("123abc");
	struct data_t *value = data_create(4, strdup("1234"));
	struct entry_t *entry;

	printf("Módulo entry -> testCreate: ");
	fflush(stdout);

	assert(entry_create(NULL, value) == NULL);
	result = (entry_create(NULL, value) == NULL);

    	assert(entry_create(key, NULL) == NULL);
	result = result && (entry_create(key, NULL) == NULL);

	if ((entry = entry_create(key, value)) == NULL)
		pee("  entry_create retornou NULL - O teste não pode prosseguir");

	result = result && (entry->key == key)
                            && (entry->value == value);

	free (entry->key);
	data_destroy(entry->value);
	free (entry);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testDestroy(){
	int result;
	struct entry_t *entry;

	printf("Módulo entry -> testDestroy: ");
	fflush(stdout);

	assert(entry_destroy(NULL) == -1);
	result = (entry_destroy(NULL) == -1);

	entry = (struct entry_t *) malloc(sizeof(struct entry_t));

	entry->key = NULL;
	entry->value = NULL;
	assert(entry_destroy(entry) == -1);
	result = result && (entry_destroy(entry) == -1);

	entry->key = strdup("abc");
	assert(entry_destroy(entry) == -1);
	result = result && (entry_destroy(entry) == -1);
	free (entry->key);
	entry->key = NULL;

	entry->value = data_create(4, strdup("1234"));
	assert(entry_destroy(entry) == -1);
	result = result && (entry_destroy(entry) == -1);
	data_destroy(entry->value);
	
	free(entry);

	if ((entry = entry_create(strdup("abc"), data_create(4, strdup("1234")))) == NULL)
		pee("  entry_create retornou NULL - O teste não pode prosseguir");
		
	result = result && (entry_destroy(entry) == 0);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testDup() {
	int result;
	char *key = strdup("abc");
	struct data_t *value = data_create(4, strdup("1234"));
	struct entry_t *entry;
	struct entry_t *entry2;

	printf("Módulo entry -> testDup: ");
	fflush(stdout);

	assert(entry_dup(NULL) == NULL);
	result = (entry_dup(NULL) == NULL);

	if ((entry = entry_create(key, value)) == NULL)
		pee("  entry_create retornou NULL - O teste não pode prosseguir");

	if ((entry2 = entry_dup(entry)) == NULL)
		pee("  entry_dup retornou NULL - O teste não pode prosseguir");

	result = result && entry2 != entry
                        && (entry->key != entry2->key)
                        && (strcmp(entry->key,entry2->key) == 0)
                        && (entry->value != entry2->value)
                        && (entry->value->datasize == entry2->value->datasize)
                        && (memcmp(entry->value->data, entry2->value->data, entry->value->datasize) == 0);

	entry_destroy(entry);
	entry_destroy(entry2);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testReplace() {
	int result;
	char *key = strdup("abc");
	char *key2 = strdup("abc2");
	struct data_t *value = data_create(4, strdup("1234"));
	struct data_t *value2 = data_create(5, strdup("56789"));
	struct entry_t *entry;

	printf("Módulo entry -> testReplace: ");
	fflush(stdout);

	assert(entry_replace(NULL, NULL, NULL) == -1);
	result = (entry_replace(NULL, NULL, NULL) == -1);

	assert(entry_replace(NULL, key2, value2) == -1);
	result = result && (entry_replace(NULL, key2, value2) == -1);

	if ((entry = entry_create(key, value)) == NULL)
		pee("  entry_create retornou NULL - O teste não pode prosseguir");

	assert(entry_replace(entry, NULL, NULL) == -1);
	result = result && (entry_replace(entry, NULL, NULL) == -1);

	assert(entry_replace(entry, key2, NULL) == -1);
	result = result && (entry_replace(entry, key2, NULL) == -1);

	assert(entry_replace(entry, NULL, value2) == -1);
	result = result && (entry_replace(entry, NULL, value2) == -1);

	if ((entry_replace(entry, key2, value2)) == -1)
		pee("  entry_replace retornou -1 - O teste não pode prosseguir");

	result = result && (entry->key == key2)
                        && (entry->value == value2);

	entry_destroy(entry);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testCompare() {
	int result;
	char *key = strdup("aaa");
	char *key2 = strdup("bbb");
	struct data_t *value = data_create(4, strdup("1234"));
	struct data_t *value2 = data_create(5, strdup("56789"));
	struct entry_t *entry;
	struct entry_t *entry2;

	printf("Módulo entry -> testCompare: ");
	fflush(stdout);

	assert(entry_compare(NULL, NULL) == -2);
	result = (entry_compare(NULL, NULL) == -2);

	if ((entry = entry_create(key, value)) == NULL)
		pee("  entry_create(key) retornou NULL - O teste não pode prosseguir");

	if ((entry2 = entry_create(key2, value2)) == NULL)
		pee("  entry_create(key2) retornou NULL - O teste não pode prosseguir");

	assert(entry_compare(entry, NULL) == -2);
	result = result && (entry_compare(entry, NULL) == -2);

	assert(entry_compare(NULL, entry) == -2);
	result = result && (entry_compare(NULL, entry) == -2);

	result = result && (entry_compare(entry, entry) == 0)
                        && (entry_compare(entry, entry2) == -1)
                        && (entry_compare(entry2, entry) == 1);

	entry_destroy(entry);
	entry_destroy(entry2);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo entry \n");

	score += testCreate();

	score += testDestroy();

	score += testDup();
	
	score += testReplace();

	score += testCompare();

	printf("teste entry (score): %d/5\n", score);

	if (score == 5)
        	return 0;
	else
        	return -1;
}
