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

#include "client_stub.h"
#include "client_callbacks.h"
#include "table_client-private.h"
#include "zoo_utils.h"

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
    if (evt_type != ZOO_CHILD_EVENT) return;

    zoo_string* children_node_paths = (zoo_string*) malloc(sizeof(zoo_string));
    int ret_wget_children = zoo_wget_children(zh, path, client_watch_children,
                                              context, children_node_paths);

    if (ret_wget_children != ZOK) {
        perror("Error wget_children in callback");
        free(children_node_paths);
        return;
    }

    zoo_string* children_paths = children_abs_zpaths(children_node_paths);

    char* new_head = (char*) calloc(ZVALLEN, sizeof(char));
    char* new_tail = (char*) calloc(ZVALLEN, sizeof(char));

    for (int i = 0; i < children_paths->count; ++i) {
        char* child_path = children_paths->data[i];
        if (strcmp(new_head, "") == 0) {
            strcpy(new_head, child_path);
            continue;
        }

        int path_cmp = strcmp(new_head, child_path);
        if (path_cmp > 0) {
            strcpy(new_head, child_path);
        }
    }

    for (int i = 0; i < children_paths->count; ++i) {
        char* child_path = children_paths->data[i];
        if (strcmp(new_tail, "") == 0) {
            strcpy(new_tail, child_path);
            continue;
        }

        int path_cmp = strcmp(new_tail, child_path);
        if (path_cmp < 0) {
            strcpy(new_tail, child_path);
        }
    }

    int has_new_head = strcmp(head_path, new_head) != 0;
    int has_new_tail = strcmp(tail_path, new_tail) != 0;

    printf("\nNew head: %d\n", has_new_head);
    printf("\nNew tail: %d\n", has_new_tail);

    if (has_new_head) {
        char *head_server_addr = malloc(ZDATALEN * sizeof(char));
        int zoo_data_len = ZDATALEN;

        zoo_get(zh, new_head, 0, head_server_addr, &zoo_data_len, NULL);

        rtable_disconnect(rtable_head, &connected_to_head);
        rtable_head = rtable_connect(head_server_addr, &connected_to_head);

        head_path = new_head;
        printf("\nConnected to new head server in node %s\n", head_path);
    }

    if (has_new_tail) {
        char *tail_server_addr = malloc(ZDATALEN * sizeof(char));
        int zoo_data_len = ZDATALEN;

        zoo_get(zh, new_tail, 0, tail_server_addr, &zoo_data_len, NULL);

        rtable_disconnect(rtable_tail, &connected_to_tail);
        rtable_tail = rtable_connect(tail_server_addr, &connected_to_tail);

        tail_path = new_tail;
        printf("\nConnected to new tail server in node %s\n", tail_path);
    }
}
