/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "client_stub.h"
#include "stats.h"

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna a estrutura rtable preenchida, ou NULL em caso de erro.
 */
struct rtable_t *rtable_connect(char *address_port) {

    if(!address_port) return NULL;

    char* colon = strchr(address_port, ':');
    int hostname_len = colon - address_port;

    char* server_address = strndup(address_port, hostname_len);
    int server_port = atoi(colon + 1);

    struct rtable_t* rtable = (struct rtable_t*) malloc(sizeof(struct rtable_t));

    rtable->server_address = server_address;
    rtable->server_port = server_port;

    if(network_connect(rtable)<0){
        printf("Error in connecting to table.");
        free(server_address);
        free(rtable);
        return NULL;
    }

    return rtable;
}

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem, ou -1 em caso de erro.
 */
int rtable_disconnect(struct rtable_t *rtable) {
    if (!rtable) return -1;

    int ret = network_close(rtable);

    free(rtable->server_address);
    free(rtable);

    return ret;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Retorna 0 (OK, em adição/substituição), ou -1 (erro).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry) {

    if (!rtable) return -1;
    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    EntryT* ent = (EntryT*) calloc(1, sizeof(EntryT));
    entry_t__init(ent);
    ent->key = strdup(entry->key);
    ent->value.len = entry->value->datasize;
    ent->value.data = malloc(ent->value.len);
    memcpy(ent->value.data, entry->value->data, entry->value->datasize);
    msg->opcode = MESSAGE_T__OPCODE__OP_PUT;
    msg->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
    msg->entry = ent;

    MessageT* res = network_send_receive(rtable, msg);

    if (!res) return -1;

    if (res->opcode == MESSAGE_T__OPCODE__OP_BAD) {
        message_t__free_unpacked(res, NULL);
        printf("Your function call was given incorrect and/or missing parameters\n");
        return -1;
    }

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return -1;
    }


    message_t__free_unpacked(msg, NULL);
    message_t__free_unpacked(res, NULL);
    return 0;
}

/* Retorna o elemento da tabela com chave key, ou NULL caso não exista
 * ou se ocorrer algum erro.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key) {

    if(!rtable) return NULL;

    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_GET;
    msg->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    msg->key = strdup(key);

    MessageT* res = network_send_receive(rtable, msg);
    message_t__free_unpacked(msg, NULL);

    if (!res) return NULL;
  
    if (res->opcode == MESSAGE_T__OPCODE__OP_BAD) {
        message_t__free_unpacked(res, NULL);
        printf("Your function call was given incorrect and/or missing parameters\n");
        return NULL;
    }

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return NULL;
    }

    if(!res->value.data){
        message_t__free_unpacked(res, NULL);
        return NULL;
    }

    struct data_t* data = (struct data_t*) malloc(sizeof(struct data_t));
    data->datasize = res->value.len;
    data->data = malloc(data->datasize);
    memcpy(data->data, res->value.data, res->value.len);

    message_t__free_unpacked(res, NULL);

    return data;
}

/* Função para remover um elemento da tabela. Vai libertar
 * toda a memoria alocada na respetiva operação rtable_put().
 * Retorna 0 (OK), ou -1 (chave não encontrada ou erro).
 */
int rtable_del(struct rtable_t *rtable, char *key) {

    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_DEL;
    msg->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    msg->key = strdup(key);

    MessageT* res = network_send_receive(rtable, msg);
    message_t__free_unpacked(msg, NULL);

    if (!res) return -1;

    if (res->opcode == MESSAGE_T__OPCODE__OP_BAD) {
        message_t__free_unpacked(res, NULL);
        printf("Your function call was given incorrect and/or missing parameters\n");
        return -1;
    }

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return -1;
    }
    if (res->c_type == MESSAGE_T__C_TYPE__CT_BAD) {
        message_t__free_unpacked(res, NULL);
        return -1;
    }
    else {
        message_t__free_unpacked(res, NULL);
        return 0;
    }
}

/* Retorna o número de elementos contidos na tabela ou -1 em caso de erro.
 */
int rtable_size(struct rtable_t *rtable) {

    if(!rtable) return -1;

    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_SIZE;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT* res = network_send_receive(rtable, msg);

    if (!res) return -1;

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return -1;
    }

    int size = (int)res->result;
    message_t__free_unpacked(msg, NULL);
    message_t__free_unpacked(res, NULL);

    return size;
}

/* Retorna um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento do array a NULL.
 * Retorna NULL em caso de erro.
 */
char **rtable_get_keys(struct rtable_t *rtable) {

    if(!rtable) return NULL;

    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT* res = network_send_receive(rtable, msg);

    if (!res) return NULL;

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return NULL;
    }

    char **key_arr = (char**) calloc(res->n_keys + 1, sizeof(char*));
    for (int i = 0; i < res->n_keys; i++) {
        key_arr[i] = strdup(res->keys[i]);
    }
    key_arr[res->n_keys] = NULL;

    message_t__free_unpacked(msg, NULL);
    message_t__free_unpacked(res, NULL);
    return key_arr;
}

/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys) {

    int i = 0;
    char *key_ptr = keys[i];
    while (key_ptr != NULL) {
        free(key_ptr);
        i++;
        key_ptr = keys[i];
    }

    free(keys);
}

/* Retorna um array de entry_t* com todo o conteúdo da tabela, colocando
 * um último elemento do array a NULL. Retorna NULL em caso de erro.
 */
struct entry_t **rtable_get_table(struct rtable_t *rtable) {

    if(!rtable) return NULL;

    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_GETTABLE;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT* res = network_send_receive(rtable, msg);

    if (!res) return NULL;

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return NULL;
    }

    struct entry_t** entry_arr = (struct entry_t**) calloc(res->n_entries + 1, sizeof(struct entry_t*));
    for (int i = 0; i < res->n_entries; i++) {
        entry_arr[i] = (struct entry_t*) malloc(sizeof(struct entry_t));
        entry_arr[i]->value = (struct data_t*) malloc(sizeof(struct data_t));
        entry_arr[i]->key = strdup(res->entries[i]->key);
        entry_arr[i]->value->datasize = res->entries[i]->value.len;
        entry_arr[i]->value->data = malloc(res->entries[i]->value.len);
        memcpy((entry_arr[i]->value->data), res->entries[i]->value.data, res->entries[i]->value.len);
    }
    entry_arr[res->n_entries] = NULL;

    message_t__free_unpacked(msg, NULL);
    message_t__free_unpacked(res, NULL);
    return entry_arr;
}

/* Liberta a memória alocada por rtable_get_table().
 */
void rtable_free_entries(struct entry_t **entries) {

    int i = 0;
    struct entry_t *entry_ptr = entries[i];
    while (entry_ptr != NULL) {
        free(entry_ptr->value->data);
        free(entry_ptr->value);
        free(entry_ptr->key);
        free(entry_ptr);
        i++;
        entry_ptr = entries[i];
    }

    free(entries);
}

/* Obtém as estatísticas do servidor. 
 */
struct statistics_t *rtable_stats(struct rtable_t *rtable) {

    if(!rtable) return NULL;

    MessageT* msg = (MessageT*) calloc(1, sizeof(MessageT));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_STATS;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT* res = network_send_receive(rtable, msg);

    if (!res) return NULL;

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        message_t__free_unpacked(res, NULL);
        return NULL;
    }

    struct statistics_t* stat = (struct statistics_t*) calloc(1, sizeof(struct statistics_t));
    stat->n_clientes = res->stats->n_clientes;
    stat->n_operacoes = res->stats->n_operacoes;
    stat->total_time = res->stats->total_time;

    message_t__free_unpacked(msg, NULL);
    message_t__free_unpacked(res, NULL);
    return stat;
}
