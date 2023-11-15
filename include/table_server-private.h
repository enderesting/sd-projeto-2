/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H
#include <signal.h>

extern volatile sig_atomic_t terminated;
extern volatile sig_atomic_t connected;

typedef struct server_resources {
    struct table_t* table;
    // TODO Add pointer to stats struct
    // TODO Add mutexes
    // TODO Add mutex cond vars
    
} server_resources;

server_resources resources;

int init_server_resources(int n_lists);

void set_sig_handlers();

void sigint_handler(int sig);

void sigpipe_handler(int sig);

#endif
