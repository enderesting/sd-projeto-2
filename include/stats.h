/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */
#ifndef _STATS_H
#define _STATS_H 
#include "mutex.h" 
#include "table_server-private.h" // This is here because to avoid linking server and client through stats.h


struct statistics_t {
    int n_clientes;
    int n_operacoes;
    int total_time;
};

/* Recebe um valor value e aumenta o n_clientes na struct stats dada pelo valor,
 * o value, deve ser 1 ou -1, para quando um cliente se conecta e disconecta respetivamente.
 */
void change_client_num(struct statistics_t* stats, mutex_locks* locks,int value);

/* Recebe uma struct stats e aumenta o seu n_operacoes por 1.
 */
void increase_operations(struct statistics_t* stats, mutex_locks* locks);

/* Recebe um valor time e aumenta o total_time na struct stats dada pelo valor.
 */
void increase_time(struct statistics_t* stats, mutex_locks* locks, int time);

#endif