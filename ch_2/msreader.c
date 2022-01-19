// 
// Reading from multiple sockets 
// Using a recv loop
//

#include <zhelpers.h>

int main()
{
    void *context = zmq_ctx_new();

    // Connect to ventilator task
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_connect(receiver, "tcp://localhost:5557");

    // Connect to weather server
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://localhost:5557");
    // Set subscriber filter
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "10001", 6);

    while (1) {
        // Process any waiting tasks
        int rc;
        for (rc = 0; !rc; ) {
            zmq_msg_t task;
            zmq_msg_init(&task);
            if ((rc = zmq_msg_recv(&task, receiver, ZMQ_DONTWAIT)) != -1) {
                // process
            }
            zmq_msg_close(&task);
        }

        // Process any waiting weather updates
        for (rc = 0; !rc; ) {
            zmq_msg_t update;
            zmq_msg_init(&update);
            if ((rc = zmq_msg_recv(&update, subscriber, ZMQ_DONTWAIT)) != -1) {
                // handle weather update
            }
            zmq_msg_close(&update);
        }
        s_sleep(1);
    }
    zmq_close(receiver);
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
    return 0;
}
