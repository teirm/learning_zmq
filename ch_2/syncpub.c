//
// Synchronized publisher
//

#include <zhelpers.h>

// Only wait for 10 subscribers
#define SUBSCRIBERS_EXPECTED 10

int main(void)
{
    void *context = zmq_ctx_new();

    // Socket to talk to clients
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:5561");

    // socket to receive signals
    void *syncservice = zmq_socket(context, ZMQ_REP);
    zmq_bind(syncservice, "tcp://*:5562");

    // Get synchronization from subscribers
    printf("Waiting for subscribers\n");
    int subscribers = 0;
    while (subscribers < SUBSCRIBERS_EXPECTED) {
        //  - wait for synchronization request
        char *string = s_recv(syncservice);
        free(string);
        //  - send synchronization reply
        s_send(syncservice, "");
        subscribers++;
    }

    sleep(1);

    // Now broadcast exactly 1M updates followed by END
    printf("Broadcasting messages\n");
    for (int update_nbr = 0; update_nbr < 1000000; update_nbr++) {
        s_send(publisher, "Rhubarb");
    }

    s_send(publisher, "END");

    zmq_close(publisher);
    zmq_close(syncservice);
    zmq_ctx_destroy(context);

    return 0;
}
