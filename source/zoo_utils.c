/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#include <stdlib.h>
#include <string.h>

#include "server_callbacks.h"
#include "zoo_utils.h"

char* concat_zpath(char* node) {
   char* concatenated = (char*) malloc(ZDATALEN * sizeof(char*));
   strcpy(concatenated, "");

   strcat(concatenated, chain_path);
   strcat(concatenated, "/");
   strcat(concatenated, node);

   return concatenated;
}

zoo_string* children_abs_zpaths(zoo_string* children) {
   zoo_string* abs_zpaths = (zoo_string*) malloc(sizeof(zoo_string*));

   for (int i = 0; i < children->count; ++i) {
       abs_zpaths->data[i] = concat_zpath(children->data[i]);
   }

   abs_zpaths->count = children->count;

   return abs_zpaths;
}
