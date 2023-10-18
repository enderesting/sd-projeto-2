#include "client_stub.h"
#include "data.h"
#include "table_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Incorrect number of arguments");
        exit(-1);
    }

    //FIXME Error checking needs to be done in several parts of this function

    char* address_port = argv[1];
    struct rtable_t* rtable = rtable_connect(argv[1]);

    printf("SD's Client module:\n");

    int terminated = 0;
    while (!terminated) {
        char line[100]; //FIXME Remove magic number
        char* ret_fgets = fgets(line, 100, stdin);

        char* operation = strtok(line, " ");
        switch (parse_operation(operation)) {
            // XXX data that is put can be generic. How to process that?
            case PUT:
                break;

            case GET: {
                char* key = strtok(line, "\n");
                struct data_t* data = rtable_get(rtable, key);
                printf("%.*s\n", data->datasize, (char*) data->data);
                data_destroy(data);
                break;
            }

            case DEL: {
                char* key = strtok(line, "\n");
                int ret_destroy = rtable_del(rtable, key);
                if (!ret_destroy) printf("Key %s was destroyed\n", key);
                else printf("Key %s does not exist\n", key);
                break;
            }

            case SIZE: {
                char* key = strtok(line, "\n");
                int size = rtable_size(rtable);
                printf("%d\n", size);
                break;
            }

            case GETKEYS:
                break;

            case GETTABLE:
                break;

            case QUIT:
                terminated = 1;
                printf("Quitting...\n");
                break;

            default:
                printf("Invalid command. Please try again.\n");
                break;
        }
    }

    rtable_disconnect(rtable); //FIXME Error checking

    return 0;
}

operation parse_operation(char *op_str) {
    if (strcmp(op_str, PUT_STR) == 0) {
        return PUT;
    } else if (strcmp(op_str, GET_STR) == 0) {
        return GET;
    } else if (strcmp(op_str, DEL_STR) == 0) {
        return DEL;
    } else if (strcmp(op_str, SIZE_STR) == 0) {
        return SIZE;
    } else if (strcmp(op_str, GETKEYS_STR) == 0) {
        return GETKEYS;
    } else if (strcmp(op_str, GETTABLE_STR) == 0) {
        return GETTABLE;
    } else if (strcmp(op_str, QUIT_STR) == 0) {
        return QUIT;
    } else {
        return INVALID;
    }
}
