//
// Multithreaded hello world server
//
#include <zhelpers.h>
#include <pthread.h>

static void *
worker_routine(void *context) {
    // Socket to talk to dispatcher
    void *receiver = zmq_socket(context, ZMQ_REP);
    zmq_connect(receiver, "inproc://workers");

    while (1) {
        char *string = s_recv(receiver);
        printf("Received request: [%s]\n", string);
        free(string);
        // Do 'stuff'
        sleep(1);
        // Send reply back to client
        s_send(receiver, "world");
    }
    zmq_close(receiver);
    return NULL;
}

int main(void)
{
    void *context = zmq_ctx_new();

    // Socket to talk to clients
    void *clients = zmq_socket(context, ZMQ_ROUTER);
    zmq_bind(clients, "tcp://*:5555");

    // Socket to talk to workers
    void *workers = zmq_socket(context, ZMQ_DEALER);
    zmq_bind(workers, "inproc://workers");

    // Launch pool of worker threads
    for (int thread_nbr = 0; thread_nbr < 5; thread_nbr++) {
        pthread_t worker;
        pthread_create(&worker, NULL, worker_routine, context);
    }

    // Connect work threads to client threads via queue proxy
    zmq_proxy(clients, workers, NULL);

    // never get here
    zmq_close(clients);
    zmq_close(workers);
    zmq_ctx_destroy(context);
    return 0;
}
