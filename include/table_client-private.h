#ifndef _TABLE_CLIENT_PRIVATE_H
#define _TABLE_CLIENT_PRIVATE_H
#include <signal.h>

volatile sig_atomic_t connected_to_server = 0;

void sigpipe_handler(int sig);

#endif
