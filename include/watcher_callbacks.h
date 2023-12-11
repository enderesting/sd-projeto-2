/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _WATCHER_CALLBACK_PRIVATE_H
#define _WATCHER_CALLBACK_PRIVATE_H

#include <zookeeper/zookeeper.h>

void server_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);

void client_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);

void server_data_watcher(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);

void client_data_watcher(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);
#endif
