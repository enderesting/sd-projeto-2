//table_server
// #include "client_stub.h"
// #include "data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inet.h"
#include "table_server.h"


int main(int argc, char *argv[]) {
    //handles args
    if (argc != 3) {
        perror("Incorrect number of arguments");
        exit(-1);
    }
    //stores the chars after the first numerical digits are taken.
    //e.g. "123abc" -> it will store "abc"
    char* endptr = NULL;
    int sockfd = network_server_init(strtol(argv[1],endptr,10));
    if (endptr){
        printf("Bad port number");
        return -1;
    }
    //initiates table
    endptr = NULL;
    struct table_t* table = table_skel_init(strtol(argv[2],endptr,10));
    if (endptr){
        printf("Bad port number"); // 1024 <= port <= 98303
        return -1;
    }

    //handles bind, listening, and finally accept
    struct sockaddr_in server_addr, client_addr;
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Error in Bind()");
        close(sockfd);
        return -1;
    };

    if (listen(sockfd, 5) < 0){ // only accepts 1 client, no request is queued?
        perror("Error in Listen()");
        close(sockfd);
        return -1;
    };
    printf("Server ready, waiting for connections");
    
    if(accept(sockfd,(struct sockaddr *) &client_addr,sizeof(client_addr))){
        return network_main_loop(sockfd,table); // loop in here until it's out
    }
}

//TODO: handling SIGINT?