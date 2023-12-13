/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#ifndef _ZOO_UTILS_H
#define _ZOO_UTILS_H

#include <zookeeper/zookeeper.h>

#define ZVALLEN 1024
#define ZDATALEN 1024 * 1024
#define chain_path "/chain"

typedef struct String_vector zoo_string;

char* concat_zpath(char* node);

zoo_string* children_abs_zpaths(zoo_string* children);

#endif
