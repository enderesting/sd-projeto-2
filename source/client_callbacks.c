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

// #include "network_client.h"
// #include "table_client-private.h"
// #include "table_server-private.h"
#include "client_callbacks.h"

void client_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context) {
    if (evt_type != ZOO_SESSION_EVENT) return;

    if (conn_state == ZOO_CONNECTED_STATE) {
        client_connected_to_zk = 1;
    } else {
        client_connected_to_zk = 0;
    }
}

void client_watch_children(zhandle_t* zh, int evt_type, int conn_state,
                         const char *path, void* context) {
//*** FIXME Added these to ignore the LSP's errors, will delete them later ****//
    char* head_path = NULL;
    char* tail_path = NULL;
//******************************************************************************/

    if (evt_type != ZOO_CHILD_EVENT) return;

    int has_new_head = zoo_exists(zh, head_path, 0, NULL) == ZNONODE;
    int has_new_tail = zoo_exists(zh, tail_path, 0, NULL) == ZNONODE;

    zoo_string* children_list = (zoo_string*) malloc(sizeof(zoo_string));
    int ret_wget_children = zoo_wget_children(zh, path, client_watch_children,
                                              context, children_list);

    if (ret_wget_children != ZOK) {
        perror("Error wget_children in callback");
        free(children_list);
        return;
    }

    char* new_head = NULL;
    char* new_tail = NULL;

    if (has_new_head) {
        for (int i = 0; i < children_list->count; ++i) {
            char* child_path = children_list->data[i];
            if (new_head == NULL) {
                strcpy(new_head, child_path);
                continue;
            }

            int path_cmp = strcmp(new_head, child_path);
            if (path_cmp > 0) {
                strcpy(new_head, child_path);
            }
        }

        // TODO disconnect from old head and connect to new head

        head_path = new_head;
    }

    if (has_new_tail) {
        for (int i = 0; i < children_list->count; ++i) {
            char* child_path = children_list->data[i];
            if (new_tail == NULL) {
                strcpy(new_tail, child_path);
                continue;
            }

            int path_cmp = strcmp(new_tail, child_path);
            if (path_cmp < 0) {
                strcpy(new_tail, child_path);
            }
        }

        // TODO disconnect from old head and connect to new head

        tail_path = new_tail;
    }

    free(children_list);
}
