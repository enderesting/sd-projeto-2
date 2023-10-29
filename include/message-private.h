#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "table.h"
#include "sdmessage.pb-c.h"

int message_send_all(int client_socket, MessageT *msg);

MessageT *message_receive_all(int client_socket);

#endif