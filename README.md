# sd-projeto
Grupo 50 <br>
Filipe Costa - 55549 <br>
Yichen Cao - 58165 <br>
Emily Sá - 58200 <br>
Github repo: https://github.com/enderesting/sd-projeto-2

## Build
A compilação deste projeto é realizada correndo o comando `make`. A makefile também admite uma receita para a remoção dos ficheiros compilados através de `make clean`

## About

Esta fase do projeto teve como objetivo, a adaptação do sistema cliente-servidor
desenvolvido na última fase a um modelo multithreaded, de modo a que o servidor
seja capaz de servir vários clientes concorrentemente. Isso implica considerações
de sincronização em relação aos recursos comuns entre as threads. Também foi agora 
requerido que o servidor mantenha estatísticas sobre o seu funcionamente, e que 
os clientes possam pedir para consultá-las.

### Threading

A adaptação a um modelo multithreaded foi implementado via o uso da biblioteca
POSIX `pthread`. 

```c
// source/network_server.c:147, em network_main_loop()

int ret_thread_create = pthread_create(&threads[vacant_thread], NULL, 
                                       &serve_conn, (void*) &connsockfd);
                                       
// source/server_thread.c:21, em serve_conn()

while (!processing_error && !terminated) {
    // receive a message, deserialize it
    MessageT *msg = network_receive(* (int*) connsockfd);
    if (!msg) {
        close(* (int*) connsockfd);
        if(!terminated) processing_error = 1;
        continue;
    }

    // get table_skel to process and get response
    if ((ret = invoke(msg, resources.table)) < 0) {
        printf("Error in processing command in internal table, shutting "
            "server down\n");
        close(* (int*) connsockfd);
        if(!terminated) processing_error = 1;
        message_t__free_unpacked(msg, NULL);
        continue;
    }

    // wait until response is here
    if (network_send(* (int*) connsockfd, msg) <= 0) {
        close(* (int*) connsockfd);
        if(!terminated) processing_error = 1;
        message_t__free_unpacked(msg, NULL);
        continue;
    }

    message_t__free_unpacked(msg, NULL);
}
```

As threads são agora as responsáveis por comunicar com os clientes e responder
aos seus pedidos via a função `invoke()`.

### Sincronização

As threads acedem a recursos comuns ao programa, nomeadamente a tabela hash que 
foi desenvolvida na primeira fase deste projeto, mas também uma tabela de estatísticas 
relativas ao funcionamento do servidor que foi adicionada nesta fase. Assim, o acesso 
a estes recursos são secções críticas que devem ser protegidos por primitivas de sincronização,
como mutexes.

A implementação relativa ao _locking/unlocking_ dos mutexes encontra-se em `mutex.c`, via as 
funções `enter_read()`, `enter_write()`, `exit_read()` e `exit_write()`, que aceitam um `struct mutex_locks` 
composta pelos parâmetros necessários para controlar o acesso às secções críticas. Durante a execução
do programa são usados dois conjuntos de `struct mutex_locks`, um para o acesso à tabela hash, e outro para o acesso
à tabela de estatísticas.

```c
// include/mutex.h:14

typedef struct mutex_locks{
    int writers_waiting;
    int readers_reading;
    int writer_active;
    pthread_mutex_t m;
    pthread_cond_t c;
} mutex_locks;
```

### Estatísticas

