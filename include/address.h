/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#ifndef _ADDRESS_H
#define _ADDRESS_H

typedef struct server_address{
    char* addr_str;
    char* ip;
    int port;
} server_address;

int interpret_addr(char* addr_str, server_address* addr);

char* concat_zpath(char* root, char* node);

int destory_addr_struct(server_address* addr);

#endif
