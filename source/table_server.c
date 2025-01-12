/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/enderesting/sd-projeto-2
 */

#include "address.h"
#include "client_stub-private.h"
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include "server_callbacks.h"
#include "zoo_utils.h"
#include "table_server.h"
#include "table_server-private.h"


typedef struct String_vector zoo_string; 
server_resources resources = {}; 
volatile sig_atomic_t terminated = 0;
volatile sig_atomic_t connected_to_zk = 0;
sig_atomic_t connected_to_server = 0; // FIXME: check if this right? used for copying. might be used later?
char* root_path = "/chain";

int main(int argc, char *argv[]) {
    // processing args for port & n_list
    if (argc != 4) {
        printf("Incorrect number of arguments\n");
        printf("Usage: table_server <zookeeper ip:port> <n_list> <server ip:port>\n");
        exit(-1);
    }

    set_sig_handlers();

    // handle arguments
    int n_lists = strtol(argv[2],NULL,10);
    char* zk_addr = argv[1];
    char* my_addr = argv[3];

    // initiates table & initiate resources
    int ret_resources = init_server_resources(n_lists, zk_addr, my_addr);
    if (ret_resources == -1) {
        return -1;
    }

    server_address* my_server_addr =
        (server_address*) calloc(1, sizeof(server_address));
    interpret_addr(my_addr, my_server_addr);

    // hook up sockets
    int sockfd = network_server_init(my_server_addr->port);
    if (sockfd==-1){
        perror("Error initializing server\n");
        return -1;
    }

    while(1){
        if(connected_to_zk){ //FIXME: check how connected is rewritten now
            //if rootpath doesn't exist, create it  
            if (ZNONODE == zoo_exists(resources.zh, root_path, 0, NULL)) {
                if (ZOK == zoo_create(resources.zh, root_path, NULL, -1, & ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0)) {
                    fprintf(stderr, "%s created!\n", root_path);
                } else {
                    fprintf(stderr,"Error Creating %s!\n", root_path);
                    exit(EXIT_FAILURE);
                } 
            }

            zoo_string* children_nodes_list =
                (zoo_string*) malloc(sizeof(zoo_string));

            // FIXME Put in callback again
            if (ZOK == zoo_wget_children(resources.zh,root_path,
                                         server_watch_children, NULL,
                                         children_nodes_list)){

                zoo_string* children_paths =
                    children_abs_zpaths(children_nodes_list);

                if (children_paths->count > 0) { // if /chain HAS children
                    //find tail node path
                    char* last_node_path = (char*) malloc(ZDATALEN * sizeof(char));

                    for (int i = 0; i < children_paths->count; ++i) {
                        char* child_path = children_paths->data[i];

                        if (strcmp(last_node_path, "") == 0) {
                            strcpy(last_node_path, child_path);
                            continue;
                        }

                        int path_cmp = strcmp(last_node_path, child_path);
                        if (path_cmp < 0) {
                            strcpy(last_node_path, child_path);
                        }
                    }

                    //find tail node address
                    char* last_node_addr = (char*) malloc(ZDATALEN * sizeof(char));
                    int last_node_size = ZDATALEN;

                    if (zoo_get(resources.zh, last_node_path, 0, last_node_addr,
                                &last_node_size, NULL) != ZOK) {
                        return -1;
                    }

                    //duplicate the server
                    dup_table_from_server(last_node_addr); // gets table and put it into resources.table
                }

                //else just continue and start loop
                break;
            }
        }
        sleep(3);
    }

    //register in zk
    //get node_path
    char node_path[ZVALLEN] = "";
    strcat(node_path,root_path); 
    strcat(node_path,"/node");

    int addr_len = strlen(my_server_addr->addr_str);

    char* created_node_path = (char*) malloc(ZVALLEN * sizeof(char));

    if (ZOK != zoo_create(
            resources.zh, node_path, my_server_addr->addr_str, addr_len,
            & ZOO_OPEN_ACL_UNSAFE,
            ZOO_EPHEMERAL | ZOO_SEQUENCE,
            created_node_path, ZVALLEN)
    ) {
        fprintf(stderr,"Error Creating %s!\n", root_path);
        exit(EXIT_FAILURE);
    }

    strcpy(resources.this_node_path, "");
    strcat(resources.this_node_path, created_node_path);

    int ret_net = network_main_loop(sockfd, resources.table); //start loop

    //it basically wont stop until server error/manual quits. if any changes happen cb will be called?

    network_server_close(sockfd);
    destroy_server_resources();
    destory_addr_struct(my_server_addr);

    return ret_net;
}


/*
Given the ip of an open server, join it as a client and request the table through gettable.
The copied tata is put into resources.table (later passed into network_main_loop)
returns -1 if error, 0 if fine.
*/
int dup_table_from_server(char* last_node_addr){
    // connect as client
    struct rtable_t* rtable = rtable_connect(last_node_addr,
                                             &connected_to_server);
    if (!rtable) return -1;
    struct entry_t** entries = rtable_get_table(rtable,&connected_to_server);
    if (!entries) return -1;
    int tab_size = rtable_size(rtable,&connected_to_server);
    if (tab_size==-1) return -1;
    rtable_disconnect(rtable,&connected_to_server);
    int ret = 0;
    for(int i = 0; i < tab_size;i++){
        printf("Copying %s key to new server", entries[i]->key);
        ret = table_put(resources.table,entries[i]->key,entries[i]->value);
        if (ret == -1){
            rtable_free_entries(entries);
            return ret;
        }
    }
    rtable_free_entries(entries);
    return 0;
}

int init_server_resources(int n_lists, char* zk_addr, char* my_addr) {
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

    resources.zh = zookeeper_init(zk_addr, server_connection_handler, 2000, 0,
                                  0, 0);

    resources.next_server_rtable = calloc(1, sizeof(struct rtable_t*));
    resources.next_server_rtable->server_address = calloc(120, sizeof(char));

    resources.this_node_path = calloc(1,ZVALLEN);
    resources.next_server_node_path = calloc(1,ZVALLEN);

    return 0;
}

int destroy_server_resources(){
    table_skel_destroy(resources.table);
    free(resources.global_stats);
    pthread_mutex_destroy(&resources.table_locks->m);
    pthread_cond_destroy(&resources.table_locks->c);
    free(resources.table_locks);
    pthread_mutex_destroy(&resources.stats_locks->m);
    pthread_cond_destroy(&resources.stats_locks->c);
    free(resources.stats_locks);

    // FIXME: free addresses?
    free(resources.this_node_path);
    free(resources.next_server_node_path);
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
