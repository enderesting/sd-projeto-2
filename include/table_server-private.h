/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H
#include <signal.h>
#include <stats.h>
#include <pthread.h>

extern volatile sig_atomic_t terminated;
extern volatile sig_atomic_t connected;

typedef struct server_resources {
    struct table_t* table;
    struct statistics_t* global_stats; // TODO Add pointer to stats struct
    // TODO Add mutex variables
    int writers_waiting;
    int readers_reading;
    int writer_active;
    // TODO Add mutex cond vars
    pthread_mutex_t m;
    pthread_cond_t c;

} server_resources;

extern server_resources resources;

int init_server_resources(int n_lists);

void set_sig_handlers();

void sigint_handler(int sig);

void sigpipe_handler(int sig);

#endif
