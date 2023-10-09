/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "serialization.h"

/* Serializa todas as chaves presentes no array de strings keys para o
 * buffer keys_buf, que será alocado dentro da função. A serialização
 * deve ser feita de acordo com o seguinte formato:
 *    | int   | string | string | string |
 *    | nkeys | key1   | key2   | key3   |
 * Retorna o tamanho do buffer alocado ou -1 em caso de erro.
 */
int keyArray_to_buffer(char **keys, char **keys_buf) {
    // XXX This implementation needs to pass through the keys array several
    // times. One pass is feasible too if needed

    if (!keys || !keys_buf) return -1;

    int size_to_alloc = sizeof(int);
    int nkeys = 0;
    char* key = keys[nkeys];
    while (key != NULL) {
        size_to_alloc += strlen(key) * sizeof(char) + 1;
        nkeys++;
        key = keys[nkeys];
    }

    char* buffer = malloc(size_to_alloc);

    char* buffer_write_ptr = buffer + sizeof(int);
    for (int i = 0; i < nkeys; i++) {
        strcpy(buffer_write_ptr, keys[i]);
        buffer_write_ptr += strlen(keys[i]) + 1;
    }

    nkeys = htonl(nkeys);
    memcpy(buffer, &nkeys, sizeof(int));
    *(keys_buf) = buffer;
    return size_to_alloc;
}

/* De-serializa a mensagem contida em keys_buf, colocando-a num array de
 * strings cujo espaco em memória deve ser reservado. A mensagem contida
 * em keys_buf deverá ter o seguinte formato:
 *    | int   | string | string | string |
 *    | nkeys | key1   | key2   | key3   |
 * Retorna o array de strings ou NULL em caso de erro.
 */
char** buffer_to_keyArray(char *keys_buf) {
    if (!keys_buf) return NULL;

    int nkeys = ntohl(*((int*) keys_buf));
    char** keys_array = (char**) malloc(sizeof(char*) * (nkeys + 1));
    keys_array[nkeys] = NULL;

    char* read_buffer_ptr = keys_buf + sizeof(int);
    for (int i = 0; i < nkeys; i++) {
        int key_size = strlen(read_buffer_ptr) + 1;
        keys_array[i] = malloc(key_size);
        strcpy(keys_array[i], read_buffer_ptr);
        read_buffer_ptr += key_size;
    }

    return keys_array;
}
