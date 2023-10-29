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

// #include "client_stub.h"
#include "network_client.h"
#include "table_client-private.h" //hmm
// #include "client_stub-private.h"
// #include "table_client-private.h"

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
        network_close(rtable);
        perror("Error parsing IP address");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*) &server, sizeof(server)) < 0) {
        network_close(rtable);
        perror("Error connecting to remote server");
        return -1;
    }

    rtable->sockfd = sockfd;
    connected_to_server = 1;
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

    if (message_send_all(sockfd,msg)<0){
        perror("Error writing to client socket");
        network_close(rtable);
        return NULL;
    }

    MessageT* received_msg = message_receive_all(rtable->sockfd);
    if (!received_msg){
        perror("Error reading message from socket");
        network_close(rtable);
        return NULL;
    }

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

    connected_to_server = 0;
    return 0;
}
