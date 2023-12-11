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
#include "address.h"
#include "client_stub-private.h"
#include "client_stub.h"
#include "table_server-private.h"
#include "watcher_callbacks.h"

void server_connection_handler(zhandle_t* zh, int evt_type, int conn_state,
                               const char *path, void* context) {
    printf("hello from conn handler");
    if (evt_type != ZOO_SESSION_EVENT) return;

    if (conn_state == ZOO_CONNECTED_STATE) {
        connected_to_zk = 1;
    } else {
        connected_to_zk = 0;
    }
}

void server_watch_children(zhandle_t* zh, int evt_type, int conn_state,
                           const char *path, void* context) {
    printf("hello from watcher");
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

        struct rtable_t* rtable = rtable_connect(new_successor_addr, &conn_hack);

        // FIXME resources.next_addr = new_successor_addr;
        resources.next_id = new_successor_path;

    }  else if (!is_tail && new_successor_path == NULL) {
        rtable_disconnect(resources.next_rtable, &conn_hack);

        resources.next_addr = NULL;
        resources.next_id = NULL;
    }
}

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
