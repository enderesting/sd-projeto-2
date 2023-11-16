/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "server_thread.h"
#include "sdmessage.pb-c.h"
#include "network_server.h"

void* serve_conn(void* connsockfd) {
    int ret;
    int processing_error = 0;

    while (!processing_error && !terminated && connected) {
        // receive a message, deserialize it
        MessageT *msg = network_receive(* (int*) connsockfd);
        if (!msg) {
            close(* (int*) connsockfd);
            if(!terminated) processing_error = 1;
            break;
        }

        // get table_skel to process and get response
        if ((ret = invoke(msg, resources.table)) < 0) {
            printf("Error in processing command in internal table, shutting "
                "server down\n");
            close(* (int*) connsockfd);
            if(!terminated) processing_error = 1;
            break;
        }

        // wait until response is here
        if (network_send(* (int*) connsockfd, msg) <= 0) {
            close(* (int*) connsockfd);
            if(!terminated) processing_error = 1;
            break;
        }

        message_t__free_unpacked(msg, NULL);
    }

    int* return_val = malloc(sizeof(int));
    *return_val = processing_error ? -1 : 0;
    return (void*) return_val;
}
