/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#ifndef _TABLE_CLIENT_PRIVATE_H
#define _TABLE_CLIENT_PRIVATE_H
#include <signal.h>
#include <zookeeper/zookeeper.h>
#include <unistd.h>

#include "client_callbacks.h"
#include "client_stub-private.h"

extern sig_atomic_t connected_to_head;
extern sig_atomic_t connected_to_tail;
extern volatile sig_atomic_t client_connected_to_zk; 
extern char* head_path;
extern char* tail_path;
extern struct rtable_t* rtable_head;
extern struct rtable_t* rtable_tail;
typedef struct String_vector zoo_string;

int children_has_difference(zoo_string* children, zoo_string* new_children);

#endif
