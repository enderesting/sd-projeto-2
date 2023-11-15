/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
//extra
#include "network_server.h"
#include "sdmessage.pb-c.h"
#include "server_thread.h"

//tamanho maximo da mensagem enviada pelo cliente
#define MAX_MSG 2048

/* Função para preparar um socket de receção de pedidos de ligação
 * num determinado porto.
 * Retorna o descritor do socket ou -1 em caso de erro.
 */
int network_server_init(short port){
    
    int sockfd = socket(AF_INET, SOCK_STREAM,0);
    if (sockfd<0){
        perror("Erro ao criar socket\n");
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
        perror("setsockopt(SO_REUSEADDR) failed\n");
        return -1;
    }

    //handles bind, listening, and finally accept -> reading loop
    struct sockaddr_in server_addr;
    //fill in the struct
    server_addr.sin_family = AF_INET;                   // always set to AF_INET for IP interface
    server_addr.sin_port = htons(port);                 // the port in network byte order
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // binds to all local interfaces

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Error in Bind()\n");
        close(sockfd);
        return -1;
    };

    if (listen(sockfd, 5) < 0){ // only accepts 1 client, no request is queued?
        perror("Error in Listen()\n");
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
    int connsockfd;
    int ret;

    struct sockaddr_in client_addr;
    socklen_t size_sockaddr_in = sizeof(client_addr);

    // FIXME Number of threads should be provided from the outside
    int thread_i = 0;
    int n_threads = 5;
    pthread_t threads[n_threads];

    printf("Server ready, waiting for connections\n");

    while (!terminated) {
        connsockfd = accept(listening_socket, (struct sockaddr *) &client_addr,
                             &size_sockaddr_in);
        if (connsockfd != -1) {
            printf("Client connection established\n");

            connected = 1;

            int ret_thread_create = pthread_create(&threads[thread_i++], NULL,
                                                   &serve_conn,
                                                   (void*) &connsockfd);
            if (ret_thread_create != 0) {
                printf("Unable to create server thread \n");
                close(connsockfd);
            }
        } else {
            printf("Unable to establish connection. Waiting for other "
                   "connections. \n");
        }
    }

    if (terminated) {
        printf("\nReceived request to shut down server gracefully. Shutting down...\n");
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
    int disconnected;
    MessageT* msg = message_receive_all(client_socket, &disconnected);

    if (disconnected) {
        printf("Client disconnected\n");
        printf("Server ready, waiting for connections\n");
    } else if (!msg) {
        printf("Error in receiving message from client\n");
    }

    return msg;
}

/* A função network_send() deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Enviar a mensagem serializada, através do client_socket.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int network_send(int client_socket, MessageT *msg){
    int sent = message_send_all(client_socket,msg);

    if (sent == -1) {
        printf("Error in sending message to client\n");
    } else if (sent == 0) {
        printf("Client disconnected\n");
        printf("Server ready, waiting for connections\n");
    }

    return sent;

}

/* Liberta os recursos alocados por network_server_init(), nomeadamente
 * fechando o socket passado como argumento.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int network_server_close(int socket){
    return close(socket);
}
