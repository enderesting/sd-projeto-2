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

extern volatile sig_atomic_t connected_to_head;
extern volatile sig_atomic_t connected_to_tail;
typedef struct String_vector zoo_string;

#endif
