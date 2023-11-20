/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdlib.h>
#include <string.h>
#include "table_skel.h"

struct timeval tv;
/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna a tabela criada ou NULL em caso de erro.
 */
struct table_t *table_skel_init(int n_lists){
    // struct timezone tz;
    return table_create(n_lists);
}

/* Liberta toda a memória ocupada pela tabela e todos os recursos 
 * e outros recursos usados pelo skeleton.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_skel_destroy(struct table_t *table){
    return table_destroy(table);
}

int return_time(struct timeval* tv){
    gettimeofday(tv,NULL);
    return tv->tv_usec;
}

/* Executa na tabela table a operação indicada pelo opcode contido em msg 
 * e utiliza a mesma estrutura MessageT para devolver o resultado.
 * Retorna 0 (OK) ou -1 em caso de erro.
*/
int invoke(MessageT *msg, struct table_t *table){
    int res = -1;
    switch (msg->opcode){
        case MESSAGE_T__OPCODE__OP_PUT:{
            int entry_size = msg->entry->value.len;
            void* v = malloc(entry_size);
            memcpy(v,msg->entry->value.data,entry_size);
            struct data_t* value = data_create(entry_size, v);
            if (!entry_size || !value){
                msg = respond_bad_op(msg);
                break;
            }
            enter_write(resources.table_locks);
            increase_operations(resources.global_stats,resources.stats_locks);
            int timestart = return_time(&tv);
            res = table_put(table,msg->entry->key,value);
            int timeend = return_time(&tv);
            increase_time(resources.global_stats,resources.stats_locks,(timeend-timestart));
            exit_write(resources.table_locks);
            if (res == 0){
                msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            } else msg = respond_err_in_exec(msg);
            data_destroy(value);
            break;
        }

        case MESSAGE_T__OPCODE__OP_GET:{
            char* key = msg->key;
            if (!key){
                msg = respond_bad_op(msg);
                break;
            }
            enter_read(resources.table_locks);
            increase_operations(resources.global_stats,resources.stats_locks);
            int timestart = return_time(&tv);
            struct data_t* gotten_value = table_get(table,(msg->key));
            int timeend = return_time(&tv);
            increase_time(resources.global_stats,resources.stats_locks,(timeend-timestart));
            exit_read(resources.table_locks);
            if (gotten_value){
                msg->value.len = gotten_value->datasize;
                msg->value.data = malloc(msg->value.len);
                memcpy(msg->value.data,gotten_value->data,msg->value.len);
                msg->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
                res = 0;
            }
            else if(!gotten_value) {
                msg->value.data = NULL;
                res = 0;
            }
            data_destroy(gotten_value);
            break;
        }

        case MESSAGE_T__OPCODE__OP_DEL:{
            char* key = msg->key;
            if (!key){
                msg = respond_bad_op(msg);
                break;
            }
            enter_write(resources.table_locks);
            increase_operations(resources.global_stats,resources.stats_locks);
            int timestart = return_time(&tv);
            res = table_remove(table,msg->key);
            int timeend = return_time(&tv);
            increase_time(resources.global_stats,resources.stats_locks,(timeend-timestart));
            exit_write(resources.table_locks);
            if (res == 0){
                msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            else if(res == 1) {
                msg->c_type = MESSAGE_T__C_TYPE__CT_BAD;
            }
            else msg = respond_err_in_exec(msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_SIZE:{
            enter_read(resources.table_locks);
            increase_operations(resources.global_stats,resources.stats_locks);
            int timestart = return_time(&tv);
            res = table_size(table);
            int timeend = return_time(&tv);
            increase_time(resources.global_stats,resources.stats_locks,(timeend-timestart));
            exit_read(resources.table_locks);
            if (res>=0){
                msg->result = res;
                msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            }
            else msg = respond_err_in_exec(msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_GETKEYS:{
            enter_read(resources.table_locks);
            increase_operations(resources.global_stats,resources.stats_locks);
            int timestart = return_time(&tv);
            char** keys = table_get_keys(table);
            int timeend = return_time(&tv);
            increase_time(resources.global_stats,resources.stats_locks,(timeend-timestart));
            exit_read(resources.table_locks);
            if (keys){
                msg->n_keys = table_size(table);
                msg->keys = keys;
                msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
                res = 0;
            }
            else msg = respond_err_in_exec(msg);
            break;
        }

        case MESSAGE_T__OPCODE__OP_GETTABLE:{
            enter_read(resources.table_locks);
            increase_operations(resources.global_stats,resources.stats_locks);
            int timestart = return_time(&tv);
            int tab_size = table_size(table);
            int timeend = return_time(&tv);
            increase_time(resources.global_stats,resources.stats_locks,(timeend-timestart));
            exit_read(resources.table_locks);
            struct entry_t** old_entries = table_get_entries(table);
            if (old_entries){
                EntryT** entries = (EntryT**) calloc(tab_size+1,
                    sizeof(struct _EntryT*));
                for (int i=0; i<tab_size; i++){
                    entries[i] = (EntryT*) malloc(sizeof(EntryT));
                    entry_t__init(entries[i]); 
                    entries[i]->key = strdup(old_entries[i]->key);
                    entries[i]->value.len = old_entries[i]->value->datasize;
                    entries[i]->value.data = malloc(entries[i]->value.len);
                    memcpy(entries[i]->value.data, old_entries[i]->value->data,
                           old_entries[i]->value->datasize);
                    entry_destroy(old_entries[i]);
                }
                msg->n_entries = tab_size;
                msg->entries = entries;
                msg->c_type = MESSAGE_T__C_TYPE__CT_TABLE;
                res = 0;
            }
            else msg = respond_err_in_exec(msg);
            free(old_entries);
            break;
        }

        case MESSAGE_T__OPCODE__OP_STATS:{   
            //global_stats is the struct statistics_t that the server 
            //initializes and uses as a global variable
            enter_read(resources.stats_locks);
            // struct statistics_t ret;
            if(!resources.global_stats){
                msg = respond_err_in_exec(msg);
            }else{
                StatisticsT* stats= (StatisticsT*) malloc(sizeof(StatisticsT));
                statistics_t__init(stats);
                stats->n_clientes = resources.global_stats->n_clientes;
                stats->n_operacoes = resources.global_stats->n_operacoes;
                stats->total_time = resources.global_stats->total_time;
                msg->stats = stats;
                msg->c_type = MESSAGE_T__C_TYPE__CT_STATS;
                res = 0;
            }
            exit_read(resources.stats_locks);
            break;
        }

        default:
            break;
    }

    if(res>=0){
        msg->opcode = msg->opcode+1;
    }

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
