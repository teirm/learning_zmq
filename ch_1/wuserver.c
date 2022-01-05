//
// Weather update server
// Binds PUB socket to tcp://*:5556
// Publishes random weather updates
//

#include <zhelpers.h>

int main()
{
    void *context   = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc          = zmq_bind(publisher, "tcp://*:5556");
    assert(rc == 0);
    
    rc = zmq_bind(publisher, "ipc://weather.ipc");
    assert(rc == 0);

    // Initialize random number generator
    srandom((unsigned) time(NULL));
    while (1) {
        // Get mischeaveous values
        int zipcode, temperature, relhumidity;
        zipcode     = random() % 100000;
        temperature = (random() % 215) - 80;
        relhumidity = (random() % 50) + 10;

        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, relhumidity);
    }
}
