//
// ROURTER-to-REQ example
//
#include <zhelpers.h>
#include <pthread.h>

#define NBR_WORKERS 10

static void*
worker_task(void *args)
{
    void *context = zmq_ctx_new();
    void *worker = zmq_socket(context, ZMQ_REQ);

    // set a printable identity
    s_set_id(worker);

    zmq_connect(worker, "tcp://localhost:5671");

    int total = 0;
    while (1) {
        s_send(worker, "Hi Boss");

        // Get workload from broker, until finished
        char *workload = s_recv(worker);
        int finished = (strcmp(workload, "Fired!") == 0);

        free(workload);
        if (finished) {
            printf("Completed: %d tasks\n", total);
            break;
        }
        total++;

        // Do some random work
        s_sleep(randof(500) + 1);
    }
    zmq_close(worker);
    zmq_ctx_destroy(context);
    return NULL;
}

int main()
{
    void *context = zmq_ctx_new();
    void *broker = zmq_socket(context, ZMQ_ROUTER);

    zmq_bind(broker, "tcp://*:5671");
    srandom((unsigned)time(NULL));

    int worker_nbr;
    for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++) {
        pthread_t worker;
        pthread_create(&worker, NULL, worker_task, NULL);
    }

    // Run for a short period of time before indicating end
    int64_t end_time = s_clock() + 5000;
    int workers_fired = 0;
    while (1) {
        // Next message gives us least recently used worker
        char *identity = s_recv(broker);
        // This sets up the routing to this worker
        s_sendmore(broker, identity);

        // this reads anything else that may come
        // from the worker
        free(identity);
        free(s_recv(broker));
        free(s_recv(broker));

        // Need to send an emty frame as delimiter
        s_sendmore(broker, "");

        if (s_clock() < end_time) {
            s_send(broker, "Work more");
        } else {
            s_send(broker, "Fired!");
            if (++workers_fired == NBR_WORKERS) {
                break;
            }
        }
    }
    zmq_close(broker);
    zmq_ctx_destroy(context);
    return 0;
}
