#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "data.h"

/**************************************************************/
void pee(const char *msg)
{
	perror(msg);
	exit(0);
}

/**************************************************************/
int testCreate() {
	int result, data_size;
	struct data_t *data;
	char *data_s = strdup("1234567890abc");

	/* Ignore \0, because we consider arbitrary contents */
	data_size = strlen(data_s); 

	printf("Módulo data -> testCreate: ");
	fflush(stdout);

	assert(data_create(-1, data_s) == NULL);
	result = (data_create(-1, data_s) == NULL);

    	assert(data_create(0, data_s) == NULL);
	result = result && (data_create(0, data_s) == NULL);

	assert(data_create(data_size, NULL) == NULL);
	result = result && (data_create(data_size, NULL) == NULL);

	if ((data = data_create(data_size, data_s)) == NULL)
		pee("  data_create retornou NULL - O teste não pode prosseguir");

	result = result && (data->data == data_s)
                        && (data->datasize == data_size)
		        && (memcmp(data->data, data_s, data_size) == 0);
	data_destroy(data);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testDestroy(){
	char *data_s = strdup("1234567890abc");
	int result, data_size = strlen(data_s);
	struct data_t *data;

	printf("Módulo data -> testDestroy: ");
	fflush(stdout);

	assert(data_destroy(NULL) == -1);
	result = (data_destroy(NULL) == -1);

	if ((data = data_create(data_size, data_s)) == NULL)
		pee("  data_create retornou NULL - O teste não pode prosseguir");
		
	result = result && (data_destroy(data) == 0);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testDup() {
	char *data_s = strdup("1234567890abc");
	int result, data_size = strlen(data_s);
	struct data_t *data;
	struct data_t *data2;

	printf("Módulo data -> testDup: ");
	fflush(stdout);

	assert(data_dup(NULL) == NULL);
	result = (data_dup(NULL) == NULL);

	data = (struct data_t *) malloc(sizeof(struct data_t));

	data->data = malloc(1);

	data->datasize = -1;
	assert(data_dup(data) == NULL);
	result = result && (data_dup(data) == NULL);

	data->datasize = 0;
	assert(data_dup(data) == NULL);
	result = result && (data_dup(data) == NULL);

	data->datasize = 1;
	free(data->data);
	data->data = NULL;
	assert(data_dup(data) == NULL);
	result = result && (data_dup(data) == NULL);
	
	free(data);

	if ((data = data_create(data_size, data_s)) == NULL)
		pee("  data_create retornou NULL - O teste não pode prosseguir");
	
	data2 = data_dup(data);
	
	result = result && (data2 != data)
                        && (data->data != data2->data)
                        && (data->datasize == data2->datasize)
                        && (memcmp(data->data, data2->data, data->datasize) == 0);

	data_destroy(data);
	data_destroy(data2);

	printf("%s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo data \n");

	score += testCreate();

	score += testDestroy();

	score += testDup();
	
	printf("teste data (score): %d/3\n", score);

	if (score == 3)
        	return 0;
	else
        	return -1;
}
