#include <stdlib.h>
#include <string.h>

#include "client_stub-private.h"
#include "data.h"
#include "entry.h"
#include "network_client.h"
#include "sdmessage.pb-c.h"

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna a estrutura rtable preenchida, ou NULL em caso de erro.
 */
struct rtable_t *rtable_connect(char *address_port) {

    //error checking
    if(!address_port) return NULL;

    char* colon = strchr(address_port, ':');
    int hostname_len = colon - address_port;

    char* server_address = strndup(address_port, hostname_len);
    int server_port = atoi(colon + 1);

    struct rtable_t* rtable = malloc(sizeof(struct rtable_t*));

    rtable->server_address = server_address;
    rtable->server_port = server_port;

    //FIXME Error checking
    network_connect(rtable);

    return rtable;
}

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem, ou -1 em caso de erro.
 */
int rtable_disconnect(struct rtable_t *rtable) {
    if (!rtable) return -1;

    //FIXME Error checking
    network_close(rtable);

    free(rtable->server_address);
    free(rtable);

    return 0;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Retorna 0 (OK, em adição/substituição), ou -1 (erro).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry) {

    if (!rtable) return -1;
    MessageT* msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(msg);
    EntryT* ent = (EntryT*) calloc(1, sizeof(struct EntryT*));
    entry_t__init(ent);
    ent->key = entry->key;
    ent->value.data = entry->value->data;
    ent->value.len = entry->value->datasize;
    msg->opcode = MESSAGE_T__OPCODE__OP_PUT;
    msg->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
    msg->entry = ent;

    MessageT* res = network_send_receive(rtable, msg);
    free(ent);
    free(msg);

    if (res->opcode == MESSAGE_T__OPCODE__OP_BAD) {
        printf("Your function call was given incorrect and/or missing parameters");
        return -1;
    }

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        return -1;
    }

    return 0;
}

/* Retorna o elemento da tabela com chave key, ou NULL caso não exista
 * ou se ocorrer algum erro.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key) {

    if(!rtable) return NULL;

    MessageT* msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_GET;
    msg->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    msg->key = key;

    MessageT* res = network_send_receive(rtable, msg);

    free(msg);

    if (res->opcode == MESSAGE_T__OPCODE__OP_BAD) {
        printf("Your function call was given incorrect and/or missing parameters");
        return -1;
    }

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        return NULL;
    }

    struct data_t* data = (struct data_t*) malloc(sizeof(struct data_t*));
    data->datasize = res->value.len;
    data->data = strndup((char*) res->value.data, data->datasize);

    message_t__free_unpacked(res, NULL);

    return data;
}

/* Função para remover um elemento da tabela. Vai libertar
 * toda a memoria alocada na respetiva operação rtable_put().
 * Retorna 0 (OK), ou -1 (chave não encontrada ou erro).
 */
int rtable_del(struct rtable_t *rtable, char *key) {

    if(!rtable_get(rtable, key)) return -1;

    MessageT* msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_DEL;
    msg->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    msg->key = key;

    MessageT* res = network_send_receive(rtable, msg);
    free(msg);

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        return -1;
    }

    else {
        free(rtable_get(rtable, key));
        return 0;
    }
}

/* Retorna o número de elementos contidos na tabela ou -1 em caso de erro.
 */
int rtable_size(struct rtable_t *rtable) {

    if(!rtable) return -1;

    MessageT* msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_SIZE;
    msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;

    MessageT* res = network_send_receive(rtable, msg);
    free(msg);

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        return -1;
    }

    return (int)res->result;
}

/* Retorna um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento do array a NULL.
 * Retorna NULL em caso de erro.
 */
char **rtable_get_keys(struct rtable_t *rtable) {

    if(!rtable) return NULL;

    MessageT* msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT* res = network_send_receive(rtable, msg);
    free(msg);

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        return NULL;
    }

    char **key_arr = (char**) calloc(res->n_keys + 1, sizeof(char*));
    for (int i = 0; i < res->n_keys; i++) {
        key_arr[i] = res->keys[i];
    }
    key_arr[res->n_keys] = NULL;
    
    return key_arr;
}

/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys) {

    int i = 0
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

    MessageT* msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(msg);
    msg->opcode = MESSAGE_T__OPCODE__OP_GETTABLE;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT* res = network_send_receive(rtable, msg);
    free(msg);

    if (res->opcode == MESSAGE_T__OPCODE__OP_ERROR &&
        res->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
        return NULL;
    }

    entry_t** entry_arr = (entry_t**) calloc(res->n_entries + 1, sizeof(entry_t*));
    for (int i = 0; i < res->n_entries; i++) {
        entry_arr[i] = res->entries[i];
    }
    entry_arr[res->n_entries] = NULL;

    return entry_arr;
}

/* Liberta a memória alocada por rtable_get_table().
 */
void rtable_free_entries(struct entry_t **entries) {

    int i = 0
    entry_t *entry_ptr = entries[i];
    while (entry_ptr != NULL) {
        free(entry_ptr);
        i++;
        entry_ptr = entries[i];
    }

    free(entries);
}
