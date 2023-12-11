/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _CLIENT_CALLBACKS_PRIVATE_H
#define _CLIENT_CALLBACKS_PRIVATE_H

#include <zookeeper/zookeeper.h>

#include "network_client.h"
#include "table_client-private.h"

#define ZDATALEN 1024 * 1024

typedef struct String_vector zoo_string;

void client_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);

void client_watch_children(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);
#endif
