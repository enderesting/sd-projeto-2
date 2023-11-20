/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <mutex.h>
#include <pthread.h>


// int writers_waiting = 0;
// int readers_reading = 0;
// int writer_active = 0;

// pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t c = PTHREAD_COND_INITIALIZER;


void enter_read(mutex_locks* locks){
    pthread_mutex_lock(&locks->m);

    while(locks->writers_waiting>0 || locks->writer_active){
        pthread_cond_wait(&locks->c,&locks->m);
    }
    locks->readers_reading++;
    pthread_mutex_unlock(&locks->m);
}

void exit_read(mutex_locks* locks){
    pthread_mutex_lock(&locks->m);
    locks->readers_reading--;
    if(locks->readers_reading==0){
        pthread_cond_broadcast(&locks->c); //all the writers waiting for readers needs to know
    }
    pthread_mutex_unlock(&locks->m);
}

void enter_write(mutex_locks* locks){
    pthread_mutex_lock(&locks->m);
    while(locks->readers_reading>0 || locks->writer_active){
        pthread_cond_wait(&locks->c,&locks->m);
    }
    locks->writer_active = true;
    locks->writers_waiting--;
    pthread_mutex_unlock(&locks->m);
}

void exit_write(mutex_locks* locks){
    pthread_mutex_lock(&locks->m);
    locks->writer_active = false;
    pthread_cond_broadcast(&locks->c); //all the writers waiting or other readers should get updated
    pthread_mutex_unlock(&locks->m);
}