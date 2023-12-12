/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H
#include <signal.h>
#include <stats.h>
#include <pthread.h>
#include <zookeeper/zookeeper.h>
#include "client_stub-private.h"
#include "mutex.h"
#include "client_stub.h"
#include "address.h"

extern volatile sig_atomic_t terminated;
extern volatile sig_atomic_t connected_to_zk;

#define ZDATALEN 1024 * 1024
#define ZVALLEN 1024

typedef struct server_resources {
    struct table_t* table;
    struct statistics_t* global_stats; 
    mutex_locks* table_locks;
    mutex_locks* stats_locks;
    // extras, for zk
    zhandle_t* zh;
    server_address* my_addr;
    server_address* next_addr;
    struct rtable_t* next_rtable;
    int next_sockfd;
    char* id;
    char* next_id;
} server_resources;

extern server_resources resources;

int dup_table_from_server(char* last_node_addr);

int init_server_resources(int n_lists, char* zk_addr, char* my_addr);

int destroy_server_resources();

void set_sig_handlers();

void sigint_handler(int sig);

void sigpipe_handler(int sig);

#endif
