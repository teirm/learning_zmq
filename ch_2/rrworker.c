// Hello world worker
// Connects REP socket to tcp://*:5560
// Expects "Hello" from client replies with "World"
//

#include <zhelpers.h>

int main()
{
    void *context = zmq_ctx_new();

    // Socket to talk to clients
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, "tcp://localhost:5560");

    while (1) {
        // Wait for next request from client
        char *string = s_recv(responder);
        printf("Received request: [%s]\n", string);
        free(string);

        // Do stuff
        sleep(1);

        // send reply
        s_send(responder, "World");
    }

    // Never get here but still cleanup
    zmq_close(responder);
    zmq_ctx_destroy(context);

    return 0;
}
