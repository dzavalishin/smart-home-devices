#include <sys/heap.h>

#include "web.h"


/*! \fn Service(void *arg)
 * \brief HTTP service thread.
 *
 * The endless loop in this thread waits for a client connect,
 * processes the HTTP request and disconnects. Nut/Net doesn't
 * support a server backlog. If one client has established a
 * connection, further connect attempts will be rejected.
 * Typically browsers open more than one connection in order
 * to load images concurrently. So we run this routine by
 * several threads.
 *
 */
THREAD(Service, arg)
{
    TCPSOCKET *sock;
    FILE *stream;
    u_char id = (u_char) ((uptr_t) arg);

    /*
     * Now loop endless for connections.
     */
    for (;;) {

        /*
         * Create a socket.
         */
        if ((sock = NutTcpCreateSocket()) == 0) {
            printf("[%u] Creating socket failed\n", id);
            NutSleep(5000);
            continue;
        }

        /*
         * Listen on port 80. This call will block until we get a connection
         * from a client.
         */
        NutTcpAccept(sock, 80);
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
        printf("[%u] Connected, %u bytes free\n", id, NutHeapAvailable());
#else
        printf("[%u] Connected, %lu bytes free\n", id, NutHeapAvailable());
#endif

        /*
         * Wait until at least 8 kByte of free RAM is available. This will 
         * keep the client connected in low memory situations.
         */
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
        while (NutHeapAvailable() < 8192) {
#else
        while (NutHeapAvailable() < 4096) {
#endif
            printf("[%u] Low mem\n", id);
            NutSleep(1000);
        }

        /*
         * Associate a stream with the socket so we can use standard I/O calls.
         */
        if ((stream = _fdopen((int) ((uptr_t) sock), "r+b")) == 0) {
            printf("[%u] Creating stream device failed\n", id);
        } else {
            /*
             * This API call saves us a lot of work. It will parse the 
             * client's HTTP request, send any requested file from the
             * registered file system or handle CGI requests by calling
             * our registered CGI routine.
             */
            NutHttpProcessRequest(stream);

            /*
             * Destroy the virtual stream device.
             */
            fclose(stream);
        }

        /*
         * Close our socket.
         */
        NutTcpCloseSocket(sock);
        printf("[%u] Disconnected\n", id);
    }
}


