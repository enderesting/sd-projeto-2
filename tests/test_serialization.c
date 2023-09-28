#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>

#include "serialization.h"
#include "data.h"
#include "entry.h"
#include "list.h"

/**************************************************************/
void pee(const char *msg)
{
    perror(msg);
    exit(0);
}

/**************************************************************/
int testSerialize(){
	int result, size;
	char **keys;
	char *keys_buf;
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

	printf("Módulo serialize -> testSerialize: ");
	fflush(stdout);

	list_add(list, entry1);
	list_add(list, entry2);
	list_add(list, entry3);
    
	keys = list_get_keys(list);

	assert(keyArray_to_buffer(keys, NULL) == -1);
	result = (keyArray_to_buffer(keys, NULL) == -1);

	assert(keyArray_to_buffer(NULL, &keys_buf) == -1);
	result = result && (keyArray_to_buffer(NULL, &keys_buf) == -1);

	if ((size = keyArray_to_buffer(keys, &keys_buf)) == -1)
		pee("  keyArray_to_buffer retornou -1 - O teste não pode prosseguir");

	/*
	| int   | string | string | string |
        | nkeys | key1   | key2   | key3   |
	*/

	assert(size == (sizeof(int) + strlen(key1)+1 + strlen(key2)+1 + strlen(key3)+1));
	result = result && (size == (sizeof(int) + strlen(key1)+1 + strlen(key2)+1 + strlen(key3)+1));

	assert(ntohl(*((int*)keys_buf)) == 3);
    	result = result && (ntohl(*((int*)keys_buf)) == 3);

	assert(strcmp(key1, keys_buf+sizeof(int)) == 0);
    	result = result && (strcmp(key1, keys_buf+sizeof(int)) == 0);

	assert(strcmp(key2, keys_buf+sizeof(int)+strlen(key1)+1) == 0);
    	result = result && (strcmp(key2, keys_buf+sizeof(int)+strlen(key1)+1) == 0);

	assert(strcmp(key3, keys_buf+sizeof(int)+strlen(key1)+strlen(key2)+2) == 0);
    	result = result && (strcmp(key3, keys_buf+sizeof(int)+strlen(key1)+strlen(key2)+2) == 0);

	list_destroy(list);
	list_free_keys(keys);
	free(keys_buf);
    
	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testDeserialize(){
	int result, size;
	char **keys;
	char **keys2;
	char *keys_buf;
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

	printf("Módulo serialize -> testDeserialize: ");
	fflush(stdout);

	list_add(list, entry1);
	list_add(list, entry2);
	list_add(list, entry3);
    
	keys = list_get_keys(list);

	if ((size = keyArray_to_buffer(keys, &keys_buf)) == -1)
		pee("  keyArray_to_buffer retornou -1 - O teste não pode prosseguir");

	if ((keys2 = buffer_to_keyArray(keys_buf)) == NULL)
		pee("  buffer_to_keyArray retornou NULL - O teste não pode prosseguir");

	assert(strcmp(key1, keys2[0]) == 0);
	result = (strcmp(key1, keys2[0]) == 0) && (key1 != keys2[0]);

	assert(strcmp(key2, keys2[1]) == 0);
	result = result && (strcmp(key2, keys2[1]) == 0) && (key2 != keys2[1]);

	assert(strcmp(key3, keys2[2]) == 0);
	result = result && (strcmp(key3, keys2[2]) == 0) && (key3 != keys2[2]);

	assert(keys2[3] == NULL);
	result = result && (keys2[3] == NULL); 

	list_destroy(list);
	list_free_keys(keys);
	free(keys_buf);
	free(keys2[0]);
	free(keys2[1]);
	free(keys2[2]);
	free(keys2);
    
	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo serialize \n");

	score += testSerialize();

	score += testDeserialize();

	printf("teste serialize (score): %d/2\n", score);

	if (score == 2)
        	return 0;
	else
        	return -1;
}
