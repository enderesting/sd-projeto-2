/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "table_server.h"

server_resources resources = {}; //TODO


volatile sig_atomic_t terminated = 0;
volatile sig_atomic_t connected = 0;

int main(int argc, char *argv[]) {
    // processing args for port & n_list
    if (argc != 3) {
        perror("Incorrect number of arguments\n");
        exit(-1);
    }

    set_sig_handlers();

    //stores the chars after the first numerical digits are taken.
    //e.g. "123abc" -> it will store "abc"
    char* endptr = NULL;
    int port = strtol(argv[1], &endptr, 10); //1024 <= port_range <= 98303 <- is this arbritrary? idk
    if (strcmp(endptr,"")!=0){ // catches bad port and return. 
        printf("Bad port number\n");
        return -1;
    }

    //initializing server socket
    int sockfd = network_server_init(port);
    if (sockfd==-1){
        perror("Error initializing server\n");
        return -1;
    }

    //initiates table
    struct table_t* table = table_skel_init(strtol(argv[2],NULL,10));
    if (!table){
        perror("Error initializing table\n");
        return -1;
    }

    int ret_net = network_main_loop(sockfd,table);

    network_server_close(sockfd);
    table_skel_destroy(table);

    return ret_net;
}


void set_sig_handlers() {
    struct sigaction sigint_act;
    sigaction(SIGINT, NULL, &sigint_act); //read sigaction from SIGINT and put it in sigint_act
    sigint_act.sa_handler = sigint_handler;
    sigint_act.sa_flags &= ~SA_RESTART; //kill that SA_RESTART
    sigaction(SIGINT, &sigint_act, NULL); //replace it

    struct sigaction sigpipe_act;
    sigaction(SIGPIPE, NULL, &sigpipe_act); 
    sigpipe_act.sa_handler = sigpipe_handler;
    sigpipe_act.sa_flags &= ~SA_RESTART;
    sigaction(SIGPIPE, &sigpipe_act, NULL);
}

void sigint_handler(int signal) {
    terminated = 1;
}

void sigpipe_handler(int signal) {
    connected = 0;
}
