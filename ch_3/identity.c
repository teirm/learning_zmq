//
// Demonstrate identities as used by the request-reply pattern.
//

#include <zhelpers.h>

int main(void)
{
    void *context = zmq_ctx_new();

    void *sink = zmq_socket(context, ZMQ_ROUTER);
    zmq_bind(sink, "inproc://example");

    // First allow 0MQ to set the identity
    void *anonymous = zmq_socket(context, ZMQ_REQ);
    zmq_connect(anonymous, "inproc://example");
    s_send(anonymous, "ROUTER uses a genreated UUID");
    s_dump(sink);

    // Set identity
    void *identified = zmq_socket(context, ZMQ_REQ);
    zmq_setsockopt(identified, ZMQ_IDENTITY, "PEER2", 5);
    zmq_connect(identified, "inproc://example");
    s_send(identified, "ROUTER socket uses REQ's socket identity");
    s_dump(sink);

    zmq_close(sink);
    zmq_close(anonymous);
    zmq_close(identified);
    zmq_ctx_destroy(context);
    return 0;
}
