#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//extra
#include "message_private.h"
#include "table_skel.h"
#include "sdmessage.pb-c.h"

#define MAX_MSG 2048

//TODO: actually implement this so it has the ability to send/read multiple packets
//returns -1 on error and 0 otherwise.
int message_send_all(int client_socket, MessageT *msg){
    short buf_len = sizeof(uint16_t) + message_t__get_packed_size(msg);

    void* buf = malloc(buf_len); //stored in here
    uint16_t buf_len_ns = htons(buf_len);

    memcpy(buf, &buf_len_ns, sizeof(uint16_t));
    message_t__pack(msg, buf + sizeof(uint16_t)); 

    while(buf_len > MAX_MSG){
        int write_len;
        if ((write_len = write(client_socket, buf, MAX_MSG)) != MAX_MSG){
            perror("Error writting to client socket");
            return -1;
        }
        buf += MAX_MSG;
        buf_len -= MAX_MSG;
    }


    int write_len;
    if ((write_len = write(client_socket, buf, buf_len)) != buf_len){
        perror("Error writting to client socket");
        return -1;
    }

    return 0;
}

MessageT *message_receive_all(int client_socket){
    int read_len;
    uint16_t response_len_ns;
    if ((read_len = read(client_socket, &response_len_ns, sizeof(uint16_t))) !=
            sizeof(response_len_ns)) { //CHECKTHIS: huh?
        perror("Error reading message length from socket");
        return NULL;
    }
    short response_len = ntohs(response_len_ns);

    void* read_buf = malloc(response_len);
    
    while(response_len > MAX_MSG){ // if message is bigger than max
        if ((read_len = read(client_socket, read_buf, MAX_MSG)) !=
                MAX_MSG) {
            perror("Error reading packed message from socket");
            return NULL;
        }
        read_buf += MAX_MSG;
        response_len -= MAX_MSG;
    }
    
    if ((read_len = read(client_socket, read_buf, response_len)) !=
            response_len) {
        perror("Error reading packed message from socket");
        return NULL;
    }
    free(read_buf);

    return message_t__unpack(NULL, response_len, read_buf);
}
