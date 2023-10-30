#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "table_client.h"
// #include "client_stub.h"
// #include "data.h"
// #include "entry.h"
// #include "table_client-private.h"

// #include "table_server.h"
// #include "table_server-private.h"

volatile sig_atomic_t connected_to_server = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid args!\nUsage: table-client <server>:<port>\n");
        exit(-1);
    }

    //FIXME Error checking needs to be done in several parts of this function

    //char* address_port = argv[1];
    struct rtable_t* rtable = rtable_connect(argv[1]);

    if (!rtable) {
        perror("Error connecting to remote server\n");
        exit(-1);
    }

    signal(SIGPIPE, sigpipe_handler);

    int terminated = 0;
    while (!terminated && connected_to_server) {
        printf("Command: ");
        char line[100]; //FIXME Remove magic number
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

                int ret_put = rtable_put(rtable, entry);
                if (ret_put == 0) {
                    printf("Entry with key \"%s\" was added\n", key);
                } else {
                    printf(
                        "There was an error adding entry with key \"%s\"\n", key
                    );
                }

                entry_destroy(entry);
                free(data_obj);
                //FIXME cant use data_destroy here, becuase data->data is a
                // reference that is freed elsewhere
                break;
            }

            case GET: {
                char* key_temp = strtok(NULL, " ");
                char* key = strtok(key_temp, "\n");

                if(!key) {
                    printf("Invalid arguments. Usage: get <key>\n");
                    break;
                }
                
                struct data_t* data = rtable_get(rtable, key);

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

                int ret_destroy = rtable_del(rtable, key);

                if (!ret_destroy) {
                    printf("Entry removed\n");
                } 
                else {
                    printf("Error in rtable_del or key not found!\n");
                }

                break;
            }

            case SIZE: {
                int size = rtable_size(rtable);
                if (size < 0) {
                    printf("There was an error retrieving table's size\n");
                } else {
                    printf("Table size: %d\n", size);
                }
                break;
            }

            case GETKEYS: {
                char** keys = rtable_get_keys(rtable);

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
                struct entry_t** entries = rtable_get_table(rtable);

                if (!entries) {
                    printf("There was an error retrieving table\n");
                    break;
                }

                //printf("Entries in table:\n");

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

            case QUIT:
                terminated = 1;
                printf("Bye, bye!\n");
                break;

            default:
                printf("Invalid command. Please try again.\nUsage: p[ut] <key> <value> | g[et] <key> | d[el] <key> | s[ize] | [get]k[eys] | [get]t[able] | q[uit]\n");
                break;
        }
    }

    if (rtable_disconnect(rtable) == -1) {
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


void sigpipe_handler(int sig) {
    connected_to_server = 0;
}