/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>


#include "table_server.h"
#include "table_server-private.h"

server_resources resources = {}; //TODO
volatile sig_atomic_t terminated = 0;

int main(int argc, char *argv[]) {
    // processing args for port & n_list
    if (argc != 4) {
        printf("Incorrect number of arguments\n");
        printf("Usage: table_server <zookeeper ip:port> <n_list> <server ip:port>\n");
        exit(-1);
    }

    set_sig_handlers();


    // connects to zk session
    // if server's first then it's good?
    // else: connect to previous server and copy contents, disconnects. 
    //       resets stats
    //       register this server in zk
    //       (the callback will probably jump into action at this point)

    resources.my_addr = interpret_addr(argv[3]); //FIXME: this is just to double check, im like half sure this isnt passed in correctly so it's first suspect if anything goes wrong
    // resources.zh = zookeeper_init(argv[1], connection_watcher, 2000, 0, 0, 0);

    int n_lists = strtol(argv[2],NULL,10);
    return boot_server(n_lists);

}

server_address* interpret_addr(char* addr_str){
    server_address* addr = (server_address*) malloc(sizeof(server_address));
    if(!addr_str) return NULL;

    char* colon = strchr(addr_str, ':');
    int ip_len = colon - addr_str;

    addr->ip = strndup(addr_str, ip_len); //FIXME: maybe error check ip...
    char* endptr = NULL;
    int port = strtol(colon + 1,&endptr,10);
    if (strcmp(endptr,"")!=0){ // catches bad port and return. 
        printf("Bad port number\n");
        return NULL;
    }
    addr->port = port;

    return addr;
}

int boot_server(int n_lists){
    //stores the chars after the first numerical digits are taken.
    //e.g. "123abc" -> it will store "abc"
    // char* endptr = NULL;
    // int port = strtol(arg_server_addr, &endptr, 10); //1024 <= port_range <= 98303 <- is this arbritrary? idk
    // if (strcmp(endptr,"")!=0){ // catches bad port and return. 
    //     printf("Bad port number\n");
    //     return -1;
    // }

    //initializing server socket
    int sockfd = network_server_init(resources.my_addr->port);
    if (sockfd==-1){
        perror("Error initializing server\n");
        return -1;
    }

    //initiates table
    // int n_lists = strtol(arg_n_list,NULL,10);
    int ret_resources = init_server_resources(n_lists);
    if (ret_resources == -1) {
        return -1;
    }

    int ret_net = network_main_loop(sockfd, resources.table);

    network_server_close(sockfd);
    destroy_server_resources();

    return ret_net;
}

int init_server_resources(int n_lists) {
    struct table_t* table = table_skel_init(n_lists);
    if (!table){
        perror("Error initializing table\n");
        return -1;
    }
    resources.table = table;

    struct statistics_t* stats = (struct statistics_t*) calloc(1,sizeof(struct statistics_t));
    stats->n_clientes = 0;
    stats->n_operacoes = 0;
    stats->total_time=0;
    resources.global_stats = stats;


    mutex_locks* tab_locks = (mutex_locks*) calloc(1,sizeof(mutex_locks));
    tab_locks->readers_reading = 0;
    tab_locks->writer_active = 0;
    tab_locks->writers_waiting = 0;
    tab_locks->c = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    tab_locks->m = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    resources.table_locks = tab_locks;

    mutex_locks* stat_locks = (mutex_locks*) calloc(1,sizeof(mutex_locks));
    stat_locks->readers_reading = 0;
    stat_locks->writer_active = 0;
    stat_locks->writers_waiting = 0;
    stat_locks->c = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    stat_locks->m = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    resources.stats_locks = stat_locks;
  
    return 0;
}

int destroy_server_resources(){
    table_skel_destroy(resources.table);
    free(resources.global_stats);
    pthread_mutex_destroy(&resources.table_locks->m);
    pthread_cond_destroy(&resources.table_locks->c);
    free(resources.table_locks);
    // free(&resources.table_locks->m);
    // free(&resources.table_locks->c);
    pthread_mutex_destroy(&resources.stats_locks->m);
    pthread_cond_destroy(&resources.stats_locks->c);
    free(resources.stats_locks);
    // free(&resources.stats_locks->m);
    // free(&resources.stats_locks->c);
    return 0;
}


void set_sig_handlers() {
    struct sigaction sigint_act;
    sigaction(SIGINT, NULL, &sigint_act); //read sigaction from SIGINT and put it in sigint_act
    sigint_act.sa_handler = sigint_handler;
    sigint_act.sa_flags &= ~SA_RESTART; //kill that SA_RESTART
    sigaction(SIGINT, &sigint_act, NULL); //replace it

    struct sigaction sigpipe_act;
    sigaction(SIGPIPE, NULL, &sigpipe_act); 
    sigpipe_act.sa_handler = SIG_IGN;
    sigpipe_act.sa_flags &= ~SA_RESTART;
    sigaction(SIGPIPE, &sigpipe_act, NULL);
}

void sigint_handler(int signal) {
    terminated = 1;
}
