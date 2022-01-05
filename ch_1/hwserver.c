// Hello World Server
// Bind REP socket to tcp://*:5555 
// Expects 'Hello' from client and replies with 'World'

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    void *context = zmq_ctx_new();

    // Socket to talk to clients
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");

    while (1) {
        // Wait for next client request
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, responder, 0);
        printf("Received hello\n");
        zmq_msg_close(&request);

        // Do stuff
        sleep(1);

        // Send message back to client
        zmq_msg_t reply;
        zmq_msg_init_size(&reply, 5);
        memcpy(zmq_msg_data(&reply), "World", 5);
        zmq_msg_send(&reply, responder, 0);
        zmq_msg_close(&reply);
    }

    // Never executed
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}
