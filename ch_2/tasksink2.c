//
// Task Sink - design 2
//

#include <zhelpers.h>

int main(void)
{
    void *context = zmq_ctx_new();

    // Socket to receive messages on
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://*:5558");

    // Socket for worker control
    void *controller = zmq_socket(context, ZMQ_PUB);
    zmq_bind(controller, "tcp://*:5559");

    // Wait for start of batch
    char *string = s_recv(receiver);
    free(string);

    // start our clock
    int64_t start_time = s_clock();

    // Process 100 confirmations
    for (int task_nbr = 0; task_nbr < 100; task_nbr++) {
        char *string = s_recv(receiver);
        free(string);
        if ((task_nbr / 10) * 10 == task_nbr) {
            printf(":");
        } else {
            printf(".");
        }
        fflush(stdout);
    }

    printf("Total elapsed time: %d msec\n", (int)(s_clock() - start_time));

    // Send kill signal to workers
    s_send(controller, "KILL");

    // finished
    sleep(1);

    zmq_close(receiver);
    zmq_close(controller);
    zmq_ctx_destroy(context);

    return 0;
}
