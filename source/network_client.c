#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "client_stub.h"
#include "sdmessage.pb-c.h"
#include "client_stub-private.h"

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) com base na
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t *rtable) {
    struct sockaddr_in server;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(rtable->server_port);
    if (inet_pton(AF_INET, rtable->server_address, &server.sin_addr) < 1) {
        close(sockfd); //FIXME is this necessary?
        perror("Error parsing IP address");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*) &server, sizeof(server))) {
        close(sockfd);
        perror("Error connecting to remote server");
        return -1;
    }

    rtable->sockfd = sockfd;

    return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Tratar de forma apropriada erros de comunicação;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
MessageT *network_send_receive(struct rtable_t *rtable, MessageT *msg) {
    //FIXME error handling is not implemented

    int sockfd = rtable->sockfd;

    short buf_len = sizeof(uint16_t) + message_t__get_packed_size(msg);
    void* buf = malloc(buf_len);
    uint16_t buf_len_ns = htons(buf_len);
    memcpy(buf, &buf_len_ns, sizeof(uint16_t));
    message_t__pack(msg, buf + sizeof(uint16_t));

    int write_len;
    if ((write_len = write(sockfd, buf, buf_len)) != buf_len){
        perror("Error writting to client socket");
        close(sockfd);
        return NULL;
    }

    int read_len;
    uint16_t response_len_ns;
    if ((read_len = read(sockfd, &response_len_ns, sizeof(uint16_t))) !=
            sizeof(response_len_ns)) {
        perror("Error reading message length from socket");
        close(sockfd);
        return NULL;
    }
    short response_len = ntohs(response_len_ns);

    void* read_buf = malloc(response_len);
    if ((read_len = read(sockfd, read_buf, response_len)) !=
            response_len) {
        perror("Error reading packed message from socket");
        close(sockfd);
        return NULL;
    }

    MessageT* received_msg = message_t__unpack(NULL, response_len, read_buf);

    free(buf);
    free(read_buf);

    return received_msg;
}

/* Fecha a ligação estabelecida por network_connect().
 * Retorna 0 (OK) ou -1 (erro).
 */
int network_close(struct rtable_t *rtable) {
    if (close(rtable->sockfd) == -1) {
        perror("Error closing socket");
        return -1;
    }

    return 0;
}
