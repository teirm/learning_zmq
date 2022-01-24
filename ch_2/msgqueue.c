//
// Simple message queuing broker
// Same as request-reply broker using QUEUE device
//
#include <zhelpers.h>

int main(void)
{
    void *context = zmq_ctx_new();

    // Socket facing clients
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    int rc = zmq_bind(frontend, "tcp://*:5559");
    assert(rc == 0);

    // Socket facing services
    void *backend = zmq_socket(context, ZMQ_DEALER);
    rc = zmq_bind(backend, "tcp://*:5560");
    assert(rc == 0);

    // start the proxy
    zmq_proxy(frontend, backend, NULL);

    // never get here -- but clean up
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    return 0;
}
