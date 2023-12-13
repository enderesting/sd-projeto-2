/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zookeeper/zookeeper.h>
#include <unistd.h>

#include "network_client.h"
#include "server_callbacks.h"
#include "table_server-private.h"
#include "zoo_utils.h"

void server_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context) {
    if (evt_type != ZOO_SESSION_EVENT) return;

    if (conn_state == ZOO_CONNECTED_STATE) {
        connected_to_zk = 1;
    } else {
        connected_to_zk = 0;
    }
}

void server_watch_children(zhandle_t* zh, int evt_type, int conn_state,
                           const char *path, void* context) {
    int check_new_successor;
    char* new_successor_path = (char*) calloc(1, ZVALLEN * sizeof(char));
    int is_tail = strcmp(resources.next_server_node_path, "") == 0;

    if (evt_type != ZOO_CHILD_EVENT) return;

    check_new_successor =
        is_tail ||
        zoo_exists(zh, resources.next_server_node_path, 0, NULL) == ZNONODE;

    if (check_new_successor) {
        zoo_string* children_node_paths =
            (zoo_string*) malloc(sizeof(zoo_string));
        int ret_wget_children =
            zoo_wget_children(zh, path, server_watch_children, context,
                              children_node_paths);
        if (ret_wget_children != ZOK) {
            perror("Error wget_children in callback");
            free(children_node_paths);
            return;
        }

        zoo_string* children_paths = children_abs_zpaths(children_node_paths);
        for (int i = 0; i < children_paths->count; ++i) {
            char* child_path = children_paths->data[i];

            int path_cmp = strcmp(resources.this_node_path, child_path);
            if (path_cmp < 0) {
                strcpy(new_successor_path, child_path);
                break;
            }

        }

        free(children_node_paths);
        free(children_paths);
    }

    printf("\nNew successor is %s, this_node_path is %s and is_tail is %d\n",
           new_successor_path, resources.this_node_path, is_tail);

    if (strcmp(new_successor_path, "") != 0) {
        if (!is_tail) {
            rtable_disconnect(resources.next_server_rtable,
                              &connected_to_server);
        }

        char* new_successor_addr = (char*) malloc(ZDATALEN * sizeof(char));
        int new_successor_addr_len = ZDATALEN;
        int ret_get = zoo_get(zh, new_successor_path, 0, new_successor_addr,
                              &new_successor_addr_len, NULL);
        if (ret_get != ZOK) {
            perror("Error get in callback");
            return;
        }

        resources.next_server_rtable = rtable_connect(new_successor_addr,
                                                      &connected_to_server);

        resources.next_server_node_path = new_successor_path;

    } else if (!is_tail && strcmp(new_successor_path, "") == 0) {
        rtable_disconnect(resources.next_server_rtable, &connected_to_server);

        resources.next_server_node_path = strcpy(resources.next_server_node_path,
                                                 "");
    }
}
