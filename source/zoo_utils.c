/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#include <stdlib.h>
#include <string.h>

#include "zoo_utils.h"

char* concat_zpath(char* root, char* node) {
   char* concatenated = (char*) malloc(ZDATALEN * sizeof(char*));
   strcpy(concatenated, "");

   strcat(concatenated, root);
   strcat(concatenated, "/");
   strcat(concatenated, node);

   return concatenated;
}
