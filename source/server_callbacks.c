/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zookeeper/zookeeper.h>
#include <unistd.h>

#include "network_client.h"
#include "server_callbacks.h"
#include "table_server-private.h"

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
    printf("\nhello from watcher, path is %s\n", path);
    int check_new_successor;
    char* new_successor_path = NULL;
    int is_tail = resources.next_id == NULL;

    if (evt_type != ZOO_CHILD_EVENT) return;

    check_new_successor =
        is_tail ||
        zoo_exists(zh, resources.next_id, 0, NULL) == ZNONODE;

    if (check_new_successor) {
        zoo_string* children_list = (zoo_string *) malloc(sizeof(zoo_string));
        int ret_wget_children = zoo_wget_children(zh, path,
                                                  server_watch_children, context,
                                                  children_list);
        if (ret_wget_children != ZOK) {
            perror("Error wget_children in callback");
            free(children_list);
            return;
        }

        for (int i = 0; i < children_list->count; ++i) {
            char* child_path = children_list->data[i];
            int path_cmp = strcmp(resources.id, child_path);
            if (path_cmp < 0) {
                strcpy(new_successor_path, child_path);
                break;
            }
        }

        free(children_list);
    }


    int conn_hack = 0;

    if (new_successor_path != NULL) {
        char* new_successor_abs_path = concat_zpath("/chain",
                                                    new_successor_path);
        if (!is_tail) {
            rtable_disconnect(resources.next_rtable, &conn_hack);
        }

        char* new_successor_addr = (char*) malloc(ZDATALEN * sizeof(char));
        int new_successor_addr_len = ZDATALEN;
        int ret_get = zoo_get(zh, new_successor_abs_path, 0, new_successor_addr,
                              &new_successor_addr_len, NULL);
        if (ret_get != ZOK) {
            perror("Error get in callback");
            return;
        }

        resources.next_rtable = rtable_connect(new_successor_addr, &conn_hack);

        // FIXME resources.next_addr = new_successor_addr;
        resources.next_id = new_successor_path;

    } else if (!is_tail && new_successor_path == NULL) {
        rtable_disconnect(resources.next_rtable, &conn_hack);

        resources.next_addr = NULL;
        resources.next_id = NULL;
    }
}
