/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */
#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

/* Serializa todas as chaves presentes no array de strings keys para o
 * buffer keys_buf, que ser� alocado dentro da fun��o. A serializa��o
 * deve ser feita de acordo com o seguinte formato:
 *    | int   | string | string | string |
 *    | nkeys | key1   | key2   | key3   |
 * Retorna o tamanho do buffer alocado ou -1 em caso de erro.
 */
int keyArray_to_buffer(char **keys, char **keys_buf);

/* De-serializa a mensagem contida em keys_buf, colocando-a num array de
 * strings cujo espaco em mem�ria deve ser reservado. A mensagem contida
 * em keys_buf dever� ter o seguinte formato:
 *    | int   | string | string | string |
 *    | nkeys | key1   | key2   | key3   |
 * Retorna o array de strings ou NULL em caso de erro.
 */
char** buffer_to_keyArray(char *keys_buf);

#endif