//
// Pub-sub envelope publisher
//
#include <zhelpers.h>

int main(void)
{
    // Prepare context and publisher
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:5563");

    while (1) {
        // Write two messages, each with an envelope and content
        s_sendmore(publisher, "A");
        s_send(publisher, "Don't want to see this");
        s_sendmore(publisher, "B");
        s_send(publisher, "We want to see this");
        sleep(1);
    }

    zmq_close(publisher);
    zmq_ctx_destroy(context);
    return 0;
};
