/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "table.h"
#include "sdmessage.pb-c.h"
#include <stdint.h>

int message_send_all(int client_socket, MessageT *msg);

MessageT *message_receive_all(int client_socket, int* disconnected);

#endif
