/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdio.h>
#include <stdlib.h>
#include "stats.h"
#include "network_server.h" // This is here because to avoid linking server and client through stats.h
// #include "table_server.h"

/* Recebe um valor value e aumenta o n_clientes na struct stats dada pelo valor,
 * o value, deve ser 1 ou -1, para quando um cliente se conecta e disconecta respetivamente.
 */
void change_client_num(struct statistics_t* stats, mutex_locks* locks, int value) {
    enter_write(locks);
    int current = stats->n_clientes;
    stats->n_clientes = current + value;
    exit_write(locks);
}

/* Recebe uma struct stats e aumenta o seu n_operacoes por 1.
 */
void increase_operations(struct statistics_t* stats, mutex_locks* locks){
    enter_write(locks);
    int current = stats->n_operacoes;
    stats->n_operacoes = current + 1;
    exit_write(locks);
    //mutex
}

/* Recebe um valor time e aumenta o total_time na struct stats dada pelo valor.
 */
void increase_time(struct statistics_t* stats, mutex_locks* locks, int time){
    enter_write(locks);
    int current = stats->total_time;
    stats->total_time = current + time;
    exit_write(locks);
}