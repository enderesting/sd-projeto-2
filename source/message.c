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
int message_send_all(int other_socket, MessageT *msg){
    short content_size = message_t__get_packed_size(msg);
    short content_size_ns = htons(content_size);

    //allocate buffer
    u_int16_t* size_buf = (u_int16_t*)malloc(sizeof(u_int16_t));
    u_int8_t* content_buf = (u_int8_t*)calloc(content_size,sizeof(u_int8_t));

    //copy content into buffer
    memcpy(size_buf,&content_size_ns,sizeof(u_int16_t));
    message_t__pack(msg,content_buf);

    //send the msg size first
    int write_len;
    if ((write_len = write(other_socket, size_buf, sizeof(u_int16_t))) != sizeof(u_int16_t)){
        perror("Error sending msg size to socket");
        return -1;
    }

    //send the msg


    // short buf_len = sizeof(uint16_t) + message_t__get_packed_size(msg);

    // void* buf = malloc(buf_len); //stored in here
    // uint16_t buf_len_ns = htons(buf_len);

    // memcpy(buf, &buf_len_ns, sizeof(uint16_t));
    // message_t__pack(msg, buf + sizeof(uint16_t)); 

    // int write_len;
    while(content_size > MAX_MSG){
        if ((write_len = write(other_socket, content_buf, MAX_MSG)) != MAX_MSG){
            perror("Error writing size to client socket");
            return -1;
        }
        content_buf += MAX_MSG;     // move pointer till after the MAX_MSG written
        content_size -= MAX_MSG;    // the amount needed to send is smaller.
    }


    if ((write_len = write(other_socket, content_buf, content_size)) != content_size){
        perror("Error writing content to client socket");
        return -1;
    }

    free(content_buf);
    free(size_buf);
    return 0;
}

MessageT *message_receive_all(int other_socket){


    //reading size
    short response_size_ns;

    int read_len;
    if ((read_len = read(other_socket, &response_size_ns, sizeof(uint16_t))) !=
            sizeof(response_size_ns)) {
        perror("Error reading message length from socket");
        return NULL;
    }
    short response_size = ntohs(response_size_ns);


    //reading
    u_int8_t* response_buf = (u_int8_t*)calloc(response_size,sizeof(u_int8_t));
    while(response_size > MAX_MSG){ // if message is bigger than max
        if ((read_len = read(other_socket, response_buf, MAX_MSG)) !=
                MAX_MSG) {
            perror("Error reading packed message from socket");
            return NULL;
        }
        response_buf += MAX_MSG;
        response_size -= MAX_MSG;
    }
    
    if ((read_len = read(other_socket, response_buf, response_size)) !=
            response_size) {
        perror("Error reading packed message from socket");
        return NULL;
    }
    // free(response_buf);

    return message_t__unpack(NULL, response_size, response_buf);
}
