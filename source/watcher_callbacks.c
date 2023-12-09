/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdio.h>
#include <string.h>
#include <zookeeper/zookeeper.h>
#include <unistd.h>

#include "network_client.h"
#include "table_server-private.h"

void server_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context) {

    if (evt_type != ZOO_SESSION_EVENT) return;

    int is_path_this_server = strcmp(path, resources.id);
    if (conn_state == ZOO_CONNECTED_STATE && is_path_this_server == 0) {
        connected = 1;
        return;
    } else if (conn_state != ZOO_CONNECTED_STATE && is_path_this_server == 0) {
        connected = 0;
        // XXX Should this set table_server.c:terminated to 0?
        return;
    }

    if (conn_state == ZOO_CONNECTED_STATE && resources.next_id == NULL &&
        is_path_this_server < 0) {
        // A new server has joined with a higher ID
        int new_server_ip_port_len;
        char new_server_ip_port[20];
        int ret_wget = zoo_wget(zh, path, NULL, NULL, new_server_ip_port,
                                &new_server_ip_port_len, NULL);
        if (ret_wget != 0) {
            perror("Error zoo_wget");
            return;
        }
    }

    int is_path_next_server = strcmp(path, resources.next_id);
    if (conn_state != ZOO_CONNECTED_STATE && is_path_next_server == 0) {
        int ret_close = network_close(resources.next_server);
        if (ret_close != 0) return;


    }
}

void client_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context) {

}

void server_data_watcher(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);

void client_data_watcher(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context);
