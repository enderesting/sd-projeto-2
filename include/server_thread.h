/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _SERVER_THREAD_H
#define _SERVER_THREAD_H
#include "table_server-private.h"
// #include "stats.h"

void* serve_conn(void* connsockfd);

#endif