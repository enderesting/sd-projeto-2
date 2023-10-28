#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//extra
#include "network_server.h"
#include "table_skel.h"
#include "sdmessage.pb-c.h"
#include "table_server.h"
#include "table_server-private.h"

//tamanho maximo da mensagem enviada pelo cliente
#define MAX_MSG 2048

/* Função para preparar um socket de receção de pedidos de ligação
 * num determinado porto.
 * Retorna o descritor do socket ou -1 em caso de erro.
 */
int network_server_init(short port){
    
    int sockfd = socket(AF_INET, SOCK_STREAM,0);
    if (sockfd<0){
        perror("Erro ao criar socket");
        return -1;
    }

    //https://linux.die.net/man/3/setsockopt <- reference
    //https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr
    //this setting is used to make sure local address can be reused
    //SOL_SOCKET:   - set options at socket level (unsure?)
    //SO_REUSEADDR  - address can be reused
    //&(int){1}     - 1 as in enable the previous option
    //sizeof(int)   - specification indicates that this option takes an int value
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))==-1){
        perror("setsockopt(SO_REUSEADDR) failed");
        return -1;
    }

    //handles bind, listening, and finally accept -> reading loop
    struct sockaddr_in server_addr;
    //fill in the struct
    server_addr.sin_family = AF_INET;                   // always set to AF_INET for IP interface
    server_addr.sin_port = htons(port);                 // the port in network byte order
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // binds to all local interfaces

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Error in Bind()");
        close(sockfd);
        return -1;
    };

    if (listen(sockfd, 5) < 0){ // only accepts 1 client, no request is queued?
        perror("Error in Listen()");
        close(sockfd);
        return -1;
    };

    return sockfd;

}

/* A função network_main_loop() deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada
     na tabela table;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 * A função não deve retornar, a menos que ocorra algum erro. Nesse
 * caso retorna -1.
 */

int network_main_loop(int listening_socket, struct table_t *table){
    //connect to socket, send/receive
    int connsockfd, ret;
    struct sockaddr_in client_addr;
    socklen_t size_sockaddr_in = sizeof(client_addr);
    int bad_termination = 0;

    while (
        !bad_termination || !terminated ||
        (connsockfd = accept(listening_socket, (struct sockaddr *)&client_addr,
                             &size_sockaddr_in)) != -1) {
        connected = 1;
        while (!bad_termination && !terminated && connected) {
            // receive a message, deserialize it
            MessageT *msg = network_receive(listening_socket);
            if (!msg) {
                perror("Error in receiving message from client");
                close(connsockfd);
                break;
            }

            // get table_skel to process and get response
            if ((ret = invoke(msg, table)) < 0) {
                perror("Error in processing command in internal table, shutting "
                    "server down");
                close(connsockfd);
                if(!terminated) bad_termination = 1;
                break;
            }

            // wait until response is here?
            if (network_send(listening_socket, msg) == -1) {
                perror("Error in sending message to client");
                close(connsockfd);
                break;
            }

            message_t__free_unpacked(msg, NULL);
        }
    }

    if (bad_termination || connsockfd == -1) {
        return -1;
    }

    return 0;
}


/* A função network_receive() deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura MessageT.
 * Retorna a mensagem com o pedido ou NULL em caso de erro.
 */
MessageT *network_receive(int client_socket){
    // short buf_len = sizeof(uint16_t) + MAX_MSG;
    // void* buf = malloc(buf_len);
    // int nbytes;

    int read_len;
    uint16_t response_len_ns;
    read_len = read(client_socket, &response_len_ns, sizeof(uint16_t));
    if (!connected) {
        perror("Lost connection to client");
        return NULL;
    }

    if (read_len != sizeof(response_len_ns)) {
        perror("Error reading message length from socket");
        return NULL;
    }

    short response_len = ntohs(response_len_ns);
    void* read_buf = malloc(response_len);

    read_len = read(client_socket, read_buf, response_len);
    if (!connected) {
        perror("Lost connection to client");
        free(read_buf);
        return NULL;
    }

    if (read_len != response_len) {
        perror("Error reading packed message from socket");
        free(read_buf);
        return NULL;
    }

    free(read_buf);
    return message_t__unpack(NULL, response_len, read_buf);
}

/* A função network_send() deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Enviar a mensagem serializada, através do client_socket.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int network_send(int client_socket, MessageT *msg){
    short buf_len = sizeof(uint16_t) + message_t__get_packed_size(msg);
    void* buf = malloc(buf_len);
    uint16_t buf_len_ns = htons(buf_len);
    memcpy(buf, &buf_len_ns, sizeof(uint16_t));
    message_t__pack(msg, buf + sizeof(uint16_t));

    // FIXME If writting fails then msg needs to be unpacked
    int write_len = write(client_socket, buf, buf_len);
    if (!connected) {
        perror("Lost connection to client");
        return -1;
    }

    if (write_len != buf_len){
        perror("Error writting to client socket");
        return -1;
    }
    return 0;
}

/* Liberta os recursos alocados por network_server_init(), nomeadamente
 * fechando o socket passado como argumento.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int network_server_close(int socket){
    return close(socket);
}
