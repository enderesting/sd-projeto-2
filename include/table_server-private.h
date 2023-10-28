#ifndef _TABLE_CLIENT_PRIVATE_H
#define _TABLE_CLIENT_PRIVATE_H
#include <signal.h>

volatile sig_atomic_t terminated = 0;
volatile sig_atomic_t connected = 0;

void sigint_handler(int sig);

void sigpipe_handler(int sig);

#endif
