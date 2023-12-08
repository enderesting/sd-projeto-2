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
#include <zookeeper/zookeeper.h>
#include "mutex.h"

extern volatile sig_atomic_t terminated;
extern volatile sig_atomic_t connected;

typedef struct server_address{
    char* ip;
    int port;
} server_address;

typedef struct server_resources {
    struct table_t* table;
    struct statistics_t* global_stats; // TODO Add pointer to stats struct
    mutex_locks* table_locks;
    mutex_locks* stats_locks;
    // extras, for zk
    zhandle_t* zh;
    server_address* my_addr;
    server_address* next_addr;
} server_resources;

extern server_resources resources;

server_address* interpret_addr(char* addr_str);

int boot_server(int n_lists);

int init_server_resources(int n_lists);

int destroy_server_resources();

void set_sig_handlers();

void sigint_handler(int sig);

void sigpipe_handler(int sig);

#endif
