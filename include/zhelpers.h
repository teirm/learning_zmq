/*
 * Helper file for examples
 */

#ifndef __ZHELPERS_H_INCLUDED__
#define __ZHELPERS_H_INCLUDED__

#include <zmq.h>

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

//  Provide random number from 0..(num-1)
#define randof(num) (int)((float) (num) * random() / (RAND_MAX + 1.0))

// Receive 0MQ string socket and covert into C string
// Caller must free returned string.
static char *
s_recv(void *socket) {
    enum { cap = 256 };
    char buffer[cap];
    int size = zmq_recv(socket, buffer, cap - 1, 0);
    if (size == -1) {
        return NULL;
    }
    buffer[size < cap ? size : cap - 1] = '\0';
    return strndup(buffer, sizeof(buffer) - 1);
}

// Convert C string to 0MQ string and send to socket
static int
s_send(void *socket, char *string) {
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}

// Sends strings as 0MQ string, as multipart non-terminal
static int
s_sendmore(void *socket, char *string) {
    int size = zmq_send(socket, string, strlen(string), ZMQ_SNDMORE);
    return size;
}

// Receives all message parts from socket and prints neatly
static void
s_dump(void *socket)
{
    int rc;

    zmq_msg_t message;
    rc = zmq_msg_init(&message);
    assert(rc == 0);

    puts("-----------------------------------------");
    // Process all parts of the message
    do {
        int size = zmq_msg_recv(&message, socket, 0);
        assert(size >= 0);

        // Dump the message as text or binary
        char *data = (char*)zmq_msg_data (&message);
        assert (data != 0);
        int is_text = 1;
        int char_nbr;
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if ((unsigned char) data [char_nbr] < 32
                || (unsigned char) data [char_nbr] > 126) {
                is_text = 0;
            }
        }

        printf ("[%03d] ", size);
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if (is_text) {
                printf ("%c", data [char_nbr]);
            } else {
                printf ("%02X", (unsigned char) data [char_nbr]);
            }
        }
        printf ("\n");
    } while (zmq_msg_more (&message));

    rc = zmq_msg_close (&message);
    assert (rc == 0);

}

// Set simple random printable identity on socket
static void
s_set_id (void *socket)
{
    char identity[10];
    sprintf(identity, "%04X-%04X", randof(0x10000), randof(0x10000));
    zmq_setsockopt(socket, ZMQ_IDENTITY, identity, strlen(identity));
}

//  Sleep for a number of milliseconds
static void
s_sleep(int msecs)
{
    struct timespec t;
    t.tv_sec  =  msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep(&t, NULL);
}

//  Report 0MQ version number
//
static void
s_version (void)
{
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    printf("Current 0MQ version is %d.%d.%d\n", major, minor, patch);
}

//  Return current system clock as milliseconds
static int64_t
s_clock (void)
{
#if (defined (WIN32))
    SYSTEMTIME st;
    GetSystemTime(&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

//  Print formatted string to stdout, prefixed by date/time and
//  terminated with a newline.

static void
s_console(const char *format, ...)
{
    time_t curtime = time(NULL);
    struct tm *loctime = localtime(&curtime);
    char *formatted = (char*)malloc (20);
    strftime(formatted, 20, "%y-%m-%d %H:%M:%S ", loctime);
    printf("%s", formatted);
    free(formatted);

    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);
    printf("\n");
}

#endif 
