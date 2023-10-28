//table_server
// #include "client_stub.h"
// #include "data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "table_server.h"

volatile sig_atomic_t terminated;
volatile sig_atomic_t connected;


int main(int argc, char *argv[]) {
    //processing args for port & n_list
    if (argc != 3) {
        perror("Incorrect number of arguments");
        exit(-1);
    }

    signal(SIGPIPE, sigint_handler);
    signal(SIGINT, sigpipe_handler);

    //stores the chars after the first numerical digits are taken.
    //e.g. "123abc" -> it will store "abc"
    char* endptr = NULL;
    int port = strtol(argv[1], &endptr, 10); //1024 <= port_range <= 98303 <- is this arbritrary? idk
    if (endptr){ // catches bad port and return. 
        printf("Bad port number");
        return -1;
    }

    //initializing server socket
    int sockfd = network_server_init(port);
    if (sockfd==-1){
        perror("Error initializing server");
        return -1;
    }

    //initiates table
    struct table_t* table = table_skel_init(strtol(argv[2],NULL,10)); //malformed arg still works?? 
    if (!table){
        perror("Error initializing table");
        return -1;
    }

    int ret_net = network_main_loop(sockfd,table);

    network_server_close(sockfd);
    table_skel_destroy(table);

    return ret_net;
}

void sigint_handler(int signal) {
    terminated = 1;
}

void sigpipe_handler(int signal) {
    connected = 0;
}
