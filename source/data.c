/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdlib.h>
#include <string.h>

#include "data.h"

/* Função que cria um novo elemento de dados data_t e que inicializa 
 * os dados de acordo com os argumentos recebidos, sem necessidade de
 * reservar memória para os dados.	
 * Retorna a nova estrutura ou NULL em caso de erro.
 */
struct data_t *data_create(int size, void *data){
    
    if (size <= 0 || data == NULL)
        return NULL;

    struct data_t* d = malloc (sizeof(struct data_t));

    d->data = data;
    d->datasize = size;

    return d;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int data_destroy(struct data_t *data){

    if(data == NULL)
        return -1;

    free(data->data);
    free(data);    

    return 0;
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 * Retorna a nova estrutura ou NULL em caso de erro.
 */
struct data_t *data_dup(struct data_t *data){

    if(data == NULL || data->data == NULL || data->datasize <= 0)
        return NULL;

    struct data_t* dup = malloc (sizeof(struct data_t));
    dup->data = (void *) malloc (data->datasize);
    dup->datasize = data->datasize;
    memcpy(dup->data, data->data, dup->datasize);

    return dup;
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
 * Deve assegurar que liberta o espaço ocupado pelo conteúdo antigo.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int data_replace(struct data_t *data, int new_size, void *new_data){

    if(data == NULL || data->data == NULL || data->datasize <= 0 || new_size <= 0 || new_data == NULL)
        return -1;

    free(data->data);
    data->data = new_data;
    data->datasize = new_size;

    return 0;
}
