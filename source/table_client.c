/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#define MAX_MSG 2048

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sdmessage.pb-c.h"
#include "table_client.h"
#include "stats.h"
#include "client_stub-private.h"
#include "table_server-private.h"
#include "zoo_utils.h"

sig_atomic_t connected_to_head = 0;
sig_atomic_t connected_to_tail = 0; 
volatile sig_atomic_t client_connected_to_zk = 0; 
static zhandle_t *zh;
const char* zoo_root = "/chain"; /* put here the location of the child nodes */
struct rtable_t* rtable_head = NULL;
struct rtable_t* rtable_tail = NULL;
char* head_path = NULL;
char* tail_path = NULL;

int children_has_difference(zoo_string* children, zoo_string* new_children) {

    int result = 1;

    if(children->count != new_children->count) {
        return result;
    }

    for(int i = 0; i < children->count; i++) {
        result = result && strcmp(children->data[i], new_children->data[i]);
    }

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid args!\nUsage: table-client <server>:<port>\n");
        exit(-1);
    }

    zh = zookeeper_init(argv[1], client_connection_handler, 20000, 0, NULL, 0);
    zoo_string* children_list = (zoo_string *) malloc(sizeof(zoo_string));

    if(!zh) {
        perror("Error connecting to remote server\n");
        exit(-1);
    }

    while(1) {
        if (client_connected_to_zk) break;
        sleep(1);
    }

    if (zoo_wget_children(zh, zoo_root, client_watch_children, NULL, children_list) != ZOK) {
        perror("Error getting child nodes\n");
        exit(-1);
    }

    if (!children_list) {
        connected_to_head = 0;
        connected_to_tail = 0;
        perror("No servers found in chain\n");
        exit(-1);
    }

    zoo_string* children_paths = children_abs_zpaths(children_list);

    head_path = (char*) calloc(ZVALLEN, sizeof(char));
    tail_path = (char*) calloc(ZVALLEN, sizeof(char));

    for (int i = 0; i < children_paths->count; ++i) {
        char* child_path = children_paths->data[i];
        if (strcmp(head_path, "") == 0) {
            strcpy(head_path, child_path);
            continue;
        }

        int path_cmp = strcmp(head_path, child_path);
        if (path_cmp > 0) {
            strcpy(head_path, child_path);
        }
    }

    for (int i = 0; i < children_paths->count; ++i) {
        char* child_path = children_paths->data[i];
        if (strcmp(tail_path, "") == 0) {
            strcpy(tail_path, child_path);
            continue;
        }

        int path_cmp = strcmp(tail_path, child_path);
        if (path_cmp < 0) {
            strcpy(tail_path, child_path);
        }
    }

    char *head_server_addr = malloc(ZDATALEN * sizeof(char));
    char *tail_server_addr = malloc(ZDATALEN * sizeof(char));
    int zoo_data_len = ZDATALEN;

    zoo_get(zh, head_path, 0, head_server_addr, &zoo_data_len, NULL);
    zoo_get(zh, tail_path, 0, tail_server_addr, &zoo_data_len, NULL);

    struct rtable_t* rtable_head = rtable_connect(head_server_addr,&connected_to_head);
    struct rtable_t* rtable_tail = rtable_connect(tail_server_addr,&connected_to_tail);

    printf("\nConnected to head in node %s, and to tail in node %s\n", head_path, tail_path);

    if (!rtable_head || !rtable_tail) {
        perror("Error connecting to remote server\n");
        exit(-1);
    }

    int terminated = 0;
    while (!terminated && connected_to_head && connected_to_tail && client_connected_to_zk) {

        printf("Command: ");
        char line[MAX_MSG];
        fgets(line, 99, stdin);

        char* ret_fgets = strtok(line, "");
        switch (parse_operation(ret_fgets)) {
            case PUT: {
                char* key = strtok(NULL, " ");
                char* data_temp = strtok(NULL, " ");
                char* data = strtok(data_temp, "\n");

                if(!key || !data) {
                    printf("Invalid arguments. Usage: put <key> <value>\n");
                    break;
                }

                struct data_t* data_obj = data_create(strlen(data), data);
                struct entry_t* entry = entry_create(strdup(key), data_dup(data_obj));

                int ret_put = rtable_put(rtable_head, entry,&connected_to_head);
                if (ret_put == 0) {
                    printf("Entry with key \"%s\" was added\n", key);
                } else {
                    printf(
                        "There was an error adding entry with key \"%s\"\n", key
                    );
                }

                entry_destroy(entry);
                free(data_obj);
                break;
            }

            case GET: {
                char* key_temp = strtok(NULL, " ");
                char* key = strtok(key_temp, "\n");

                if(!key) {
                    printf("Invalid arguments. Usage: get <key>\n");
                    break;
                }
                
                struct data_t* data = rtable_get(rtable_tail, key,&connected_to_tail);

                if (!data) {
                    printf("Error in rtable_get or key not found!\n");
                    break;
                }

                printf("%.*s\n", data->datasize, (char*) data->data);
                free(data->data);
                free(data);
                break;
            }

            case DEL: {
                char* key_temp = strtok(NULL, " ");
                char* key = strtok(key_temp, "\n");

                if(!key) {
                    printf("Invalid arguments. Usage: del <key>\n");
                    break;
                }

                int ret_destroy = rtable_del(rtable_head, key,&connected_to_head);

                if (!ret_destroy) {
                    printf("Entry removed\n");
                } 
                else {
                    printf("Error in rtable_del or key not found!\n");
                }

                break;
            }

            case SIZE: {
                int size = rtable_size(rtable_tail,&connected_to_tail);
                if (size < 0) {
                    printf("There was an error retrieving table's size\n");
                } else {
                    printf("Table size: %d\n", size);
                }
                break;
            }

            case GETKEYS: {
                char** keys = rtable_get_keys(rtable_tail,&connected_to_tail);

                if (!keys) {
                    printf("There was an error retrieving keys\n");
                    break;
                }

                int i = 0;
                char* key = keys[i];
                while (key) {
                    printf("%s\n", key);
                    key = keys[++i];
                }

                rtable_free_keys(keys);
                break;
            }

            case GETTABLE: {
                struct entry_t** entries = rtable_get_table(rtable_tail,&connected_to_tail);

                if (!entries) {
                    printf("There was an error retrieving table\n");
                    break;
                }

                int i = 0;
                struct entry_t* entry = entries[i];
                while (entry) {
                    printf("%s: %.*s\n", entry->key, entry->value->datasize,
                           (char*) entry->value->data);
                    entry = entries[++i];
                }

                rtable_free_entries(entries);
                break;
            }

            case STATS: {
                struct statistics_t* stats = rtable_stats(rtable_tail,&connected_to_tail);
        
                if (!stats) {
                    printf("There was an error retrieving stats\n");
                    break;
                }

                printf("Number of connected clients: %d\nNumber of operations: %d\nTotal time spent: %d\n", stats->n_clientes, stats->n_operacoes, stats->total_time);
                free(stats);
                break;
            }

            case QUIT:
                terminated = 1;
                printf("Bye, bye!\n");
                break;

            default:
                printf("Invalid command. Please try again.\nUsage: p[ut] <key> <value> | g[et] <key> | d[el] <key> | s[ize] | [get]k[eys] | [get]t[able] | st[ats] | q[uit]\n");
                break;
        }
    }

    if (rtable_disconnect(rtable_head,&connected_to_head) == -1 || rtable_disconnect(rtable_tail,&connected_to_tail) == -1) {
        perror("Error disconnecting from remote server\n");
        exit(-1);
    }

    return 0;
}

operation parse_operation(char *op_str) {
    if (strcmp(op_str, SIZE_STR) == 0 || strcmp(op_str, "s\n") == 0) {
        return SIZE;
    } else if (strcmp(op_str, GETKEYS_STR) == 0 || strcmp(op_str, "k\n") == 0) {
        return GETKEYS;
    } else if (strcmp(op_str, GETTABLE_STR) == 0 || strcmp(op_str, "t\n") == 0) {
        return GETTABLE;
    } else if (strcmp(op_str, QUIT_STR) == 0 || strcmp(op_str, "q\n") == 0) {
        return QUIT;
    } else if (strcmp(op_str, STATS_STR) == 0 || strcmp(op_str, "st\n") == 0) {
        return STATS;
    }

    char* operation = strtok(op_str, " ");
    if (strcmp(operation, PUT_STR) == 0 || strcmp(operation, "p") == 0 
    || strcmp(operation, "put\n") == 0 || strcmp(operation, "p\n") == 0) {
        return PUT;
    } else if (strcmp(operation, GET_STR) == 0 || strcmp(operation, "g") == 0
    || strcmp(operation, "get\n") == 0 || strcmp(operation, "g\n") == 0) {
        return GET;
    } else if (strcmp(operation, DEL_STR) == 0 || strcmp(operation, "d") == 0
    || strcmp(operation, "del\n") == 0 || strcmp(operation, "d\n") == 0) {
        return DEL;
    } 

    return INVALID;
}
