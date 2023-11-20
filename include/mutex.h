/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */
#ifndef _MUTEX_H
#define _MUTEX_H

#include <pthread.h>
// #include <network_server.h>

// int writers_waiting=0;
// int readers_reading=0;
// int writer_active=0;

// pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t c = PTHREAD_COND_INITIALIZER;

typedef struct mutex_locks{
    int writers_waiting;
    int readers_reading;
    int writer_active;
    pthread_mutex_t m;
    pthread_cond_t c;
} mutex_locks;

void enter_read(mutex_locks* locks);

void exit_read(mutex_locks* locks);

void enter_write(mutex_locks* locks);

void exit_write(mutex_locks* locks);

#endif