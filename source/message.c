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
#include "message-private.h"
#include "table_skel.h"
#include "sdmessage.pb-c.h"

#define MAX_MSG 2048

// Given a socket, write the given message into the connection.
// First a short carrying the size of the message will be written
// into the socket, followed by the serialized MessageT.
// The serialized message can be bigger than the MAX_MSG, in which case
// it will be written into the socket in packets.
//
// Returns -1 on error, otherwise return the number of total bytes sent,
// including the size indicating short.
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
    int total_write_len, write_len;
    if ((write_len = write(other_socket, size_buf, sizeof(u_int16_t))) != sizeof(u_int16_t)){
        perror("Error sending msg size to socket");
        return -1;
    }
    total_write_len = write_len;

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
        content_buf += write_len;     // move pointer till after the MAX_MSG written
        content_size -= write_len;    // the amount needed to send is smaller.
        total_write_len += write_len;
    }

    if ((write_len = write(other_socket, content_buf, content_size)) != content_size){
        perror("Error writing content to client socket");
        return -1;
    }
    total_write_len += write_len;

    free(content_buf);
    free(size_buf);
    return total_write_len;
}

//Given a socket, read the content and unpack it into MessageT format.
// First a short carrying the size of the message will be read
// into the socket, followed by the serialized MessageT.
// The serialized message can be bigger than the MAX_MSG, in which case
// it will be read into the socket in packets and unpacked at the end.
//
// Returns NULL on error and MessageT otherwise.
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
    MessageT* ret = message_t__unpack(NULL, response_size, response_buf);
    free(response_buf);
    return ret;
}
