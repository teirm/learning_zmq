//
// Simple request-reply broker
//

#include <zhelpers.h>

int main(void)
{
    // Prepare our context and sockets
    void *context = zmq_ctx_new();
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend = zmq_socket(context, ZMQ_DEALER);
    zmq_bind(frontend, "tcp://*:5559");
    zmq_bind(backend, "tcp://*5560");

    // init poll set
    zmq_pollitem_t items[] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend,  0, ZMQ_POLLIN, 0 }
    };

    // switch messages between sockets
    while (1) {
        zmq_msg_t message;
        int more;    // multipart detection
        
        zmq_poll(items, sizeof(items), -1);
        if (items[0].revents & ZMQ_POLLIN) {
            while (1) {
                // process all parts of the message
                zmq_msg_init(&message);
                zmq_msg_recv(&message, frontend, 0);
                size_t more_size = sizeof(more);
                zmq_getsockopt(frontend, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send(&message, backend, more ? ZMQ_SNDMORE : 0);
                zmq_msg_close(&message);

                if (!more) {
                    break;
                }
            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            while (1) {
                // process all parts of the message
                zmq_msg_init(&message);
                zmq_msg_recv(&message, backend, 0);
                size_t more_size = sizeof(more);
                zmq_getsockopt(backend, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send(&message, frontend, more ? ZMQ_SNDMORE : 0);
                zmq_msg_close(&message);

                if (!more) {
                    break;
                }
            }
        }
    }
    // never get here but clean up
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    return 0;
}
