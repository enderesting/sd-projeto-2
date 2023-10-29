#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H
#include <signal.h>

extern volatile sig_atomic_t terminated;
extern volatile sig_atomic_t connected;

void sigint_handler(int sig);

void sigpipe_handler(int sig);

#endif
