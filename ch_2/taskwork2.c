//
// Task worker - design 2
// Adds pub-sub flow to receive and respond to kill signal
//
#include <zhelpers.h>

int main(void)
{
    void *context = zmq_ctx_new();

    // socket to receive messages on
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_connect(receiver, "tcp://localhost:5557");

    // socket to send messages to
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sender, "tcp://localhost:5558");

    // socket for control input
    void *controller = zmq_socket(context, ZMQ_SUB);
    zmq_connect(controller, "tcp://localhost:5559");
    zmq_setsockopt(controller, ZMQ_SUBSCRIBE, "", 0);

    // process messages from receiver and controller
    zmq_pollitem_t items[] = {
        {receiver, 0, ZMQ_POLLIN, 0},
        {controller, 0, ZMQ_POLLIN, 0}
    };

    // Process messages from both sockets
    while (1) {
        zmq_msg_t message;
        zmq_poll(items, sizeof(items)/sizeof(items[0]), -1);
        if (items[0].revents & ZMQ_POLLIN) {
            zmq_msg_init(&message);
            zmq_msg_recv(&message, receiver, 0);

            // Do the work
            s_sleep(atoi((char *)zmq_msg_data(&message)));

            // Send results to sink
            zmq_msg_init(&message);
            zmq_msg_send(&message, sender, 0);

            // simple progress indicator
            printf(".");
            fflush(stdout);

            zmq_msg_close(&message);
        }
        // Any waiting controller comman acts as a 'KILL'
        if (items[1].revents & ZMQ_POLLIN) {
            break;
        }
    }
    // Finished
    zmq_close(receiver);
    zmq_close(sender);
    zmq_close(controller);
    zmq_ctx_destroy(&context);
    return 0;
}
