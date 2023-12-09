/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "address.h"

/*
given addr_str, interprets it and put it into the struct pointed to by addr.
returns -1 on failure and 0 on success.
*/
int interpret_addr(char* addr_str, server_address* addr){
    if(!addr_str) return -1;

    char* colon = strchr(addr_str, ':');
    int ip_len = colon - addr_str;

    addr->ip = strndup(addr_str, ip_len); //FIXME: maybe error check ip...
    char* endptr = NULL;
    int port = strtol(colon + 1,&endptr,10);
    if (strcmp(endptr,"")!=0){ // catches bad port and return. 
        printf("Bad port number\n");
        return -1;
    }
    addr->port = port;
    addr->addr_str = addr_str;

    return 0;
}

int destory_addr_struct(server_address* addr){
    if(!addr){
        return -1;
    }
    free(addr->addr_str);
    free(addr->ip);
    free(addr);
    return 0;
}