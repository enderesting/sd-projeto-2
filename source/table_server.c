//table_server
// #include "client_stub.h"
// #include "data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <signal.h>
#include "inet.h"
#include "table_server.h"


int main(int argc, char *argv[]) {
    //processing args for port & n_list
    if (argc != 3) {
        perror("Incorrect number of arguments");
        exit(-1);
    }
    //stores the chars after the first numerical digits are taken.
    //e.g. "123abc" -> it will store "abc"
    char* endptr = NULL;
    int port = strtol(argv[1],endptr,10); //1024 <= port_range <= 98303 <- is this arbritrary? idk
    if (endptr){ // catches bad port and return. 
        printf("Bad port number");
        return -1;
    }
    int sockfd = network_server_init(port);
    //initiates table
    struct table_t* table = table_skel_init(strtol(argv[2],NULL,10)); //malformed arg still works?? 


    //handles bind, listening, and finally accept -> reading loop
    struct sockaddr_in server_addr, client_addr;
    //fill in the struct
    server_addr.sin_family = AF_INET;                   // always set to AF_INET for IP interface
    server_addr.sin_port = htons(atoi(argv[1]));        // the port in network byte order
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // binds to all local interfaces

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

// void signal_handler(int signal)
// {
//     if (signal == SIGPIPE){
//         printf("Caught SIGPIPE, ignore and continue"); 
//         // since we want it to ignore, eventually we can swap this function out for SIG_IGN instead...
//         // however, for the sake of testing,there's a print here. 
//         // SIGPIPE: Broken pipe: write to pipe with no readers
//     }
// }
// //TODO: handling SIGINT?