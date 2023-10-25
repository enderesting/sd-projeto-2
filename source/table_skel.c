#include "table_skel.h"
#include "data.h"
#include "sdmessage.pb-c.h"

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
    int res;
    switch (msg->opcode){
        case MESSAGE_T__OPCODE__OP_BAD:
            printf("Invalid arguments!\n");
            res = 0;
            msg->c_type = MESSAGE_T__C_TYPE__CT_BAD;
            break;
        case MESSAGE_T__OPCODE__OP_PUT:
            int entry_size = entry_t__get_packed_size(msg->entry);
            struct data_t *value = data_create(entry_size, msg->entry->value.data); // CHECKTHIS: is this the correct way of doing it
            res = table_put(table,msg->entry->key,value);
            if (res == 0){
                msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            break;
        case MESSAGE_T__OPCODE__OP_GET:
            struct data_t* gotten_value = table_get(table,msg->key);
            if (gotten_value){
                //NO idea if this is being done correctly tbh
                struct ProtobufCBinaryData v;
                v.len = gotten_value->datasize;
                v.data = gotten_value->data;
                msg->value = v;
                msg->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
                res = 0;
            }else{ //if get success
                res = -1;
                printf("Error in rtable_get or key not found!\n");
            }
            break;
        case MESSAGE_T__OPCODE__OP_DEL:
            res = table_remove(table,msg->key);
            if (res == 0){
                msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            //CHECKTHIS: is there any difference in handling missing entry vs error deletion?
            break;
        case MESSAGE_T__OPCODE__OP_SIZE:
            res = table_size(table);
            if (res>=0){
                msg->result = res;
                msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            }
            break;
        case MESSAGE_T__OPCODE__OP_GETKEYS:
            char** keys = table_get_keys(table);
            if (keys){
                msg->n_keys = table_size(table);
                msg->keys = keys;
                msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
                res = 0;
            }else{
                res = -1;
            }
            break;
        case MESSAGE_T__OPCODE__OP_GETTABLE:
            int tab_size = table_size(table);
            struct entry_t** old_entries = table_get_entries(table);
            if (old_entries){
                struct _EntryT** entries = (struct _EntryT*) malloc(tab_size+1,sizeof(struct _EntryT*));
                for (int i=0; i<tab_size; i++){
                    entries[i]->key = old_entries[i]->key;
                    entries[i]->value.len = old_entries[i]->value->datasize;
                    entries[i]->value.data = old_entries[i]->value->data;
                }
                msg->n_entries = tab_size;
                msg->entries = entries;
                msg->c_type = MESSAGE_T__C_TYPE__CT_TABLE;
                res = 0;
            }
            break;
        case MESSAGE_T__OPCODE__OP_ERROR:
            //CHECKTHIS: how to handle error?
            break;
    }

    if (res<0) {
        msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
        msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    }else{
        msg->opcode = msg->opcode+1;
    }

    return res;
}