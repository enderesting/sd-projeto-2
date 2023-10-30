/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
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
// Returns -1 on error, or 0 if socket was disconnected
// otherwise return the number of total bytes sent, including the size
// indicating short.
int message_send_all(int other_socket, MessageT *msg){
    short content_size = message_t__get_packed_size(msg);
    short content_size_ns = htons(content_size);

    //allocate buffer
    u_int16_t* size_buf = (u_int16_t*)malloc(sizeof(u_int16_t));
    u_int8_t* content_buf = (u_int8_t*)calloc(content_size,sizeof(u_int8_t));

    //copy content into buffer
    memcpy(size_buf,&content_size_ns,sizeof(u_int16_t));
    message_t__pack(msg,content_buf);

    // FIXME write() error checks should follow the same structure
    // as in message_receive_all

    //send the msg size first
    int total_write_len;
    int write_len = write(other_socket, size_buf, sizeof(u_int16_t));
    if (write_len == -1 || write_len > sizeof(uint16_t)) {
        perror("Error sending msg size to socket");
        return -1;
    } else if (write_len < sizeof(uint16_t)) {
        return 0;
    }
    total_write_len = write_len;

    //send the msg
    while(content_size > MAX_MSG){
        write_len = write(other_socket, content_buf, MAX_MSG);
        if (write_len == -1 || write_len > MAX_MSG){
            perror("Error writing content to socket");
            return -1;
        } else if (write_len < MAX_MSG) {
            return 0;
        }

        content_buf += write_len;     // move pointer till after the MAX_MSG written
        content_size -= write_len;    // the amount needed to send is smaller.
        total_write_len += write_len;
    }

    write_len = write(other_socket, content_buf, content_size);
    if (write_len == -1 || write_len > content_size){
        perror("Error writing content to socket");
        return -1;
    } else if (write_len < content_size) {
        return 0;
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
// Returns NULL on error or a buffer containing the MessageT otherwise.
MessageT* message_receive_all(int other_socket, int* disconnected){
    //reading size
    short response_size_ns;
    *disconnected = 0;

    int read_len = read(other_socket, &response_size_ns, sizeof(uint16_t));
    if ((read_len == -1 && errno == EINTR) ||
        (read_len >= 0 && read_len < sizeof(response_size_ns))) {
        *disconnected = 1;
        return NULL;
    } else if (read_len == -1 || read_len > sizeof(response_size_ns)) {
        perror("Error reading message length from socket");
        return NULL;
    }

    short response_size = ntohs(response_size_ns);

    //reading
    u_int8_t* response_buf = (u_int8_t*) calloc(response_size,sizeof(u_int8_t));
    while (response_size > MAX_MSG) {
        read_len = read(other_socket, response_buf, MAX_MSG);
        if ((read_len == -1 && errno == EINTR) ||
            (read_len >= 0 && read_len < MAX_MSG)) {
            *disconnected = 1;
            return NULL;
        } else if (read_len == -1 || read_len > MAX_MSG) {
            perror("Error reading packed message from socket");
            return NULL;
        }

        response_buf += MAX_MSG;
        response_size -= MAX_MSG;
    }

    read_len = read(other_socket, response_buf, response_size);
    if ((read_len == -1 && errno == EINTR) ||
        (read_len >= 0 && read_len < response_size)) {
        *disconnected = 1;
        return NULL;
    } else if (read_len == -1 || read_len > response_size) {
        perror("Error reading packed message from socket");
        return NULL;
    }

    MessageT* ret = message_t__unpack(NULL, response_size, response_buf);
    free(response_buf);
    return ret;
}
