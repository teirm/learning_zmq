//
// Load-balancing broker
// Clients and workers are shown here in-process
//

#include <zhelpers.h>
#include <pthread.h>

#define NBR_CLIENTS 10
#define NBR_WORKERS 3

// Dequeue operation for queue implemented as array of anything
#define DEQUEUE(q) memmove(&(q)[0], &(q)[1], sizeof(q) - sizeof(q[0]))

// Basic request-reply client using REQ socket
// Since s_send and s_recv can't handle 0MQ binary identities
// we set a printable text id to allow routing

static void *
client_task(void *args)
{
    void *context = zmq_ctx_new();
    void *client  = zmq_socket(context, ZMQ_REQ);
    s_set_id(client);
    zmq_connect(client, "ipc://frontend.ipc");

    // Send request, get reply
    s_send(client, "HELLO");
    char *reply = s_recv(client);
    printf("Client: %s\n", reply);
    free(reply);
    zmq_close(client);
    zmq_ctx_destroy(context);
    return NULL;
}

static void *
worker_task(void *args)
{
    void *context = zmq_ctx_new();
    void *worker  = zmq_socket(context, ZMQ_REQ);
    s_set_id(worker);
    zmq_connect(worker, "ipc://backend.ipc");

    // Tell worker we're ready for work
    s_send(worker, "READY");

    while (1) {
        // Read and save all frames until we get an empty frame

        // Only 1 in this example

        char *identity = s_recv(worker);
        char *empty = s_recv(worker);

        assert(*empty == 0);

        free(empty);

        // Get request, send reply
        char *request = s_recv(worker);
        printf("Worker: %s\n", request);
        free(request);

        s_sendmore(worker, identity);
        s_sendmore(worker, "");
        s_send(worker, "OK");

        free(identity);
    }
    zmq_close(worker);
    zmq_ctx_destroy(context);
    return NULL;
}

int main(void)
{
    void *context = zmq_ctx_new();
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend = zmq_socket(context, ZMQ_ROUTER);

    zmq_bind(frontend, "ipc://frontend.ipc");
    zmq_bind(backend, "ipc://backend.ipc");

    int client_nbr;
    for (client_nbr = 0; client_nbr < NBR_CLIENTS; client_nbr++) {
        pthread_t client;
        pthread_create(&client, NULL, client_task, NULL);
    }

    int worker_nbr;
    for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++) {
        pthread_t worker;
        pthread_create(&worker, NULL, worker_task, NULL);
    }

    int available_workers = 0;
    char *worker_queue[10];

    while (1) {
        zmq_pollitem_t items[] = {
            {backend,  0, ZMQ_POLLIN, 0},
            {frontend, 0, ZMQ_POLLIN, 0}
        };

        int rc = zmq_poll(items, available_workers ? 2 : 1, -1);
        if (rc == -1) {
            // EINTR
            break;
        }

        // Handle worker activity on backend
        if (items[0].revents & ZMQ_POLLIN) {
            // Queue worker identity for load-balancing
            char *worker_id = s_recv(backend);
            assert(available_workers < NBR_WORKERS);
            worker_queue[available_workers++] = worker_id;

            // Second frame is empty
            char *empty = s_recv(backend);
            assert(empty[0] == 0);
            free(empty);

            // Third frame is READY or else a client reply identity
            char *client_id = s_recv(backend);

            // If client reply, send rest back to frontend
            if (strcmp(client_id, "READY") != 0) {
                empty = s_recv(backend);
                assert(empty[0] == 0);
                free(empty);
                char *reply = s_recv(backend);
                s_sendmore(frontend, client_id);
                s_sendmore(frontend, "");
                s_sendmore(frontend, reply);
                free(reply);
                if (--client_nbr == 0) {
                    break;
                }
            }
            free(client_id);
        }

        if (items[1].revents & ZMQ_POLLIN) {
            // Get next client request, route to last used worker
            // Client request is [identity][empty][request]
            char *client_id = s_recv(frontend);
            char *empty = s_recv(frontend);
            assert(empty[0] == 0);

            free(empty);

            char *request = s_recv(frontend);

            s_sendmore(backend, worker_queue[0]);
            s_sendmore(backend, "");
            s_sendmore(backend, client_id);
            s_sendmore(backend, "");
            s_send(backend, request);

            free(client_id);
            free(request);

            // Dequeue and drop the next worker identity
            free(worker_queue[0]);
            DEQUEUE(worker_queue);
            available_workers--;
        }
    }

    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);

    return 0;
}
