/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _TABLE_CLIENT_PRIVATE_H
#define _TABLE_CLIENT_PRIVATE_H
#include <signal.h>
#include <zookeeper/zookeeper.h>
#include "watcher_callbacks.h"
#include <unistd.h>

// extern volatile sig_atomic_t connected_to_head; //i think they might not need to be global since we're passing them manually around.
// extern volatile sig_atomic_t connected_to_tail;
extern volatile sig_atomic_t client_connected_to_zk; 
extern char* head_path;
extern char* tail_path;
typedef struct String_vector zoo_string;

int children_has_difference(zoo_string* children, zoo_string* new_children);

#endif
