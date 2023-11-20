# sd-projeto
Grupo 50 <br>
Filipe Costa - 55549 <br>
Yichen Cao - 58165 <br>
Emily Sá - 58200 <br>
Github repo: https://github.com/enderesting/sd-projeto-2

## Build
A compilação deste projeto é realizada correndo os comandos:

```bash
make
```

A makefile também admite uma receita para a remoção dos ficheiros compilados através de `make clean`

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
// source/network_server.c:147

int ret_thread_create = pthread_create(&threads[vacant_thread], NULL, 
                                       &serve_conn, (void*) &connsockfd);
                                       
// source/server_thread.c:21

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

### Estatísticas

