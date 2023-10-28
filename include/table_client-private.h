#ifndef _TABLE_CLIENT_PRIVATE_H
#define _TABLE_CLIENT_PRIVATE_H
#include <signal.h>

extern volatile sig_atomic_t connected_to_server;

void sigpipe_handler(int sig);

#endif
