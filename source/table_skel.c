#include "table_skel.h"
#include "data.h"
#include "sdmessage.pb-c.h"

#include <stdlib.h>
#include <string.h>

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna a tabela criada ou NULL em caso de erro.
 */
struct table_t *table_skel_init(int n_lists){
    return table_create(n_lists);
}

/* Liberta toda a memória ocupada pela tabela e todos os recursos 
 * e outros recursos usados pelo skeleton.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_skel_destroy(struct table_t *table){
    return table_destroy(table);
}

/* Executa na tabela table a operação indicada pelo opcode contido em msg 
 * e utiliza a mesma estrutura MessageT para devolver o resultado.
 * Retorna 0 (OK) ou -1 em caso de erro.
*/
int invoke(MessageT *msg, struct table_t *table){
    int res = -1;
    MessageT* new_msg = (MessageT*) calloc(1, sizeof(struct MessageT*));
    message_t__init(new_msg);
    switch (msg->opcode){
        case MESSAGE_T__OPCODE__OP_PUT:{
            int entry_size = msg->entry->value.len;
            // CHECKTHIS: is this the correct way of doing it
            struct data_t *value = data_create(entry_size, msg->entry->value.data);
            if (!entry_size || !value){
                new_msg = respond_bad_op(new_msg);
                break;
            }
            res = table_put(table,msg->entry->key,value);
            if (res == 0){
                new_msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            } else new_msg = respond_err_in_exec(new_msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_GET:{
            char* key = msg->key;
            if (!key){
                new_msg = respond_bad_op(new_msg);
                break;
            }
            struct data_t* gotten_value = table_get(table,msg->key);
            if (gotten_value){
                new_msg->value.len = gotten_value->datasize;
                new_msg->value.data =  gotten_value->data;
                new_msg->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
                res = 0;
            }
            else new_msg = respond_err_in_exec(new_msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_DEL:{
            char* key = msg->key;
            if (!key){
                new_msg = respond_bad_op(new_msg);
                break;
            }
            res = table_remove(table,msg->key);
            if (res == 0){
                new_msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            else new_msg = respond_err_in_exec(new_msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_SIZE:{
            res = table_size(table);
            if (res>=0){
                new_msg->result = res;
                new_msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            }
            else new_msg = respond_err_in_exec(new_msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_GETKEYS:{
            char** keys = table_get_keys(table);
            if (keys){
                new_msg->n_keys = table_size(table);
                new_msg->keys = keys;
                new_msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
                res = 0;
            }
            else new_msg = respond_err_in_exec(new_msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_GETTABLE:{
            int tab_size = table_size(table);
            struct entry_t** old_entries = table_get_entries(table);
            if (old_entries){
                EntryT** entries = (EntryT**) calloc(tab_size+1,
                    sizeof(struct _EntryT*));
                for (int i=0; i<tab_size; i++){
                    entries[i]->key = strdup(old_entries[i]->key);
                    entries[i]->value.len = old_entries[i]->value->datasize;
                    memcpy(entries[i]->value.data, old_entries[i]->value->data,
                           old_entries[i]->value->datasize);
                }
                new_msg->n_entries = tab_size;
                new_msg->entries = entries;
                new_msg->c_type = MESSAGE_T__C_TYPE__CT_TABLE;
                res = 0;
            }
            else new_msg = respond_err_in_exec(new_msg);
            free(old_entries);
            break;
        }

        default:
            break;
    }

    if(res>=0){
        new_msg->opcode = msg->opcode+1;
    }

    message_t__free_unpacked(msg,NULL); 
    *msg = *new_msg; //TODO: is this the correct parsing?

    return res;
}

MessageT* respond_bad_op(MessageT* msg){
    msg->opcode = MESSAGE_T__OPCODE__OP_BAD;
    msg->c_type = MESSAGE_T__C_TYPE__CT_BAD;
    return msg;
}

MessageT* respond_err_in_exec(MessageT* msg){
    msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    return msg;
}
