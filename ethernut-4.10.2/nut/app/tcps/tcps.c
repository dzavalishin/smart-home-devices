/*
 * Copyright (C) 2009 by egnite GmbH
 * Copyright (C) 2001-2005 by egnite Software GmbH
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*!
 * $Id: tcps.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

/*!
 * \example tcps/tcps.c
 *
 * Simple TCP server.
 *
 * \code telnet x.x.x.x \endcode
 *
 * on a command prompt, replacing x.x.x.x with the
 * IP address of your target board. Enter help
 * for a list of available commands.
 */

/* Device specific definitions. */
#include <dev/board.h>
#include <dev/reset.h>
#include <dev/gpio.h>

/* OS specific definitions. */
#include <sys/version.h>
#include <sys/confnet.h>
#include <sys/heap.h>
#include <sys/timer.h>
#include <sys/socket.h>

/* Network specific definitions. */
#include <arpa/inet.h>
#include <net/if_var.h>
#include <pro/dhcp.h>

/* Standard C header files. */
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* Version of this application sample. */
#define APP_VERSION     "2.0.0"

/* Max. size of the input line. */
#define MAX_INPUT_LINE  32

/* TCP server port. Telnet default is 23. */
#define TCP_SERVER_PORT 23

static time_t start_time;

/*
 * Halt the application on fatal errors.
 */
static void FatalError(char *msg)
{
    /* Print a message ... */
    puts(msg);
    /* ... and never return. */
    for (;;);
}

/*
 * Extract command and up to 2 parameters from an input line.
 *
 * This helper routine splits a line into words. A pointer to the
 * first word is returned as a function result. A pointer to an
 * optional second word is set via a funtion parameter and a
 * pointer to the rest of the line is set via a second function
 * parameter.
 *
 * Example:
 *
 * If line points to "help me to get this done", then we can use
 *
 * char *cmd;
 * char *param1;
 * char *param2;
 *
 * cmd = ParseLine(line, &param1, &param2);
 *
 * On return, cmd will point to "help", param1 will point to "me"
 * and param 2 will point to "to get this done".
 *
 * If the line contains less than 3 words, then the second
 * parameter pointer is set to NULL. With one word only, also
 * the first parameter pointer is set to NULL. The function 
 * result is NULL on empty lines, including lines containing 
 * all spaces.
 *
 * Leading spaces are skipped. Trailing end of line characters
 * are removed.
 *
 * Note, that the original contents of the line will be
 * modified.
 */
static char *ParseLine(char *line, char **pp1, char **pp2)
{
    char *p0;
    char *cp;

    /* Initialize parameter pointers to NULL. */
    *pp1 = NULL;
    *pp2 = NULL;

    /* Chop off EOL. */
    cp = strchr(line, '\r');
    if (cp) {
        *cp = 0;
    }
    cp = strchr(line, '\n');
    if (cp) {
        *cp = 0;
    }

    /*
     * Parse line for command and parameters.
     */
    p0 = line;
    while (isspace((int)*p0)) {
        /* Skip leading spaces. */
        p0++;
    }
    if (*p0 == '\0') {
        /* Return NULL on empty lines. */
        return NULL;
    }
    cp = strchr(p0, ' ');
    if (cp) {
        *cp++ = '\0';
        while (isspace((int)*cp)) {
            /* Skip leading spaces. */
            cp++;
        }
        if (*cp) {
            /* First parameter found. */
            *pp1 = cp;
            cp = strchr(cp, ' ');
        } else {
            cp = NULL;
        }
        if (cp) {
            *cp++ = '\0';
            while (isspace((int)*cp)) {
                /* Skip leading spaces. */
                cp++;
            }
            if (*cp) {
                /* Remaining parameter(s) found. */
                *pp2 = cp;
            }
        }
    }
    /* Return pointer to command. */
    return p0;
}

/*
 * Process client requests.
 *
 * This function is called when a connection has been established
 * and returns when the connection is closed.
 */
static void ProcessRequests(FILE * stream)
{
    char *buff;
    char *cmd;
    size_t clen;
    char *p1;
    char *p2;

    /*
     * Allocate an input buffer. Check the result.
     */
    buff = malloc(MAX_INPUT_LINE + 1);
    if (buff == NULL) {
        return;
    }

    /*
     * Send a welcome banner to the new client.
     */
    fputs("200 Welcome to tcps. Type help to get help.\r\n", stream);
    for (;;) {

        /* 
         * Flush any pending output and read in a new line. 
         *
         * If you want line editing capabilities, check
         * http://www.ethernut.de/nutwiki/Input_Line_Editor
         */
        fflush(stream);
        if (fgets(buff, MAX_INPUT_LINE, stream) == NULL) {
            /* Probably a disconnect, return. */
            break;
        }
        /* Parse the input line. */
        cmd = ParseLine(buff, &p1, &p2);
        if (cmd == NULL) {
            /* Skip empty lines. */
            continue;
        }
        /* Retrieve command length for abbreviations. */
        clen = strlen(cmd);

        /*
         * Process memory info request.
         *
         * http://www.ethernut.de/nutwiki/Heap_Memory
         */
        if (strncmp(cmd, "heap", clen) == 0) {
            fprintf(stream, "210 %u bytes RAM free\r\n", (unsigned int)NutHeapAvailable());
            continue;
        }

        /*
         * Process IP address configuration.
         */
        if (strncmp(cmd, "ip", clen) == 0) {
            uint32_t ip = p1 ? inet_addr(p1) : (uint32_t) -1;

            if (ip == (uint32_t) -1) {
                fputs("420 Invalid or missing address\r\n", stream);
            } else {
                confnet.cdn_cip_addr = ip;
                if (NutNetSaveConfig()) {
                    fputs("421 Failed to save configuration\r\n", stream);
                } else {
                    fputs("220 Configuration saved\r\n", stream);
                }
            }
            continue;
        }

        /*
         * Process IP mask configuration.
         */
        if (strncmp(cmd, "mask", clen) == 0) {
            uint32_t mask = p1 ? inet_addr(p1) : (uint32_t) -1;

            if (mask == (uint32_t) -1) {
                fputs("430 Invalid or missing mask\r\n", stream);
            } else {
                confnet.cdn_ip_mask = mask;
                if (NutNetSaveConfig()) {
                    fputs("421 Failed to save configuration\r\n", stream);
                } else {
                    fputs("230 Configuration saved\r\n", stream);
                }
            }
            continue;
        }

#ifndef MCU_GBA
        /*
         * Process GPIO pin status, not available on GameBoy.
         *
         * http://www.ethernut.de/nutwiki/LowLevelPortIo
         */
        if (strncmp(cmd, "pin", clen) == 0) {
            int bank = p1 ? atoi(p1) : 0;
            int bit = p2 ? atoi(p2) : 0;
            int state = GpioPinGet(bank, bit);

            fprintf(stream, "240 %d at GPIO bank %d bit %d\r\n", state, bank, bit);
            continue;
        }
#endif

        /*
         * Process serial line send request.
         */
        if (strncmp(cmd, "send", clen) == 0) {
            if (p1) {
                printf("%s", p1);
                if (p1) {
                    printf(" %s", p2);
                }
            }
            putchar('\n');
            fputs("250 Message sent\r\n", stream);
            continue;
        }

        /*
         * Process time info request.
         */
        if (strncmp(cmd, "uptime", clen) == 0) {
            fprintf(stream, "220 %ld seconds running\r\n", (long)(time(NULL) - start_time));
            continue;
        }

        /*
         * Process system reset request.
         *
         * http://www.ethernut.de/nutwiki/System_Reset
         */
        if (strncmp(cmd, "reset", clen) == 0) {
            fputs("910 System reset\r\n", stream);
            fflush(stream);
            NutSleep(1000);
            NutReset();
            fputs("490 System reset not implemented\r\n", stream);
            continue;
        }

        /*
         * Quit connection.
         */
        if (strncmp(cmd, "quit", clen) == 0) {
            fputs("900 Bye\r\n", stream);
            fflush(stream);
            break;
        }

        /*
         * Display help text on any unknown command.
         */
        fputs("400 List of commands follows\r\n"
              "h[eap]    Query heap memory bytes available.\r\n"
              "i[p]      Set IP <address>.\r\n"
              "m[ask]    Set IP <mask>.\r\n"
#ifndef MCU_GBA
              "p[in]     Query status of GPIO pin <bank> <bit>.\r\n"
#endif
              "r[eset]   Reset system.\r\n"
              "s[end]    Send <message> to serial port.\r\n"
              "u[ptime]  Query number of seconds the system is running.\r\n"
              "q[uit]    Terminates connection.\r\n" 
              ".\r\n", stream);
    }
    free(buff);
}

/*
 * Main application routine. 
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
    TCPSOCKET *sock;
    FILE *stream;
    uint32_t baud = 115200;

    /*
     * Assign stdout to the DEBUG device.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);

    /*
     * Print out our version information.
     */
    printf("\n\nNut/OS %s\n", NutVersionString());
    printf("TCP Server Sample %s\n", APP_VERSION);

    /*
     * Configure the network interface. It is assumed, that 
     * we got a valid configuration in non-volatile memory.
     *
     * For alternatives see
     * http://www.ethernut.de/nutwiki/Network_Configuration
     */
    printf("Configure %s...", DEV_ETHER_NAME);
    if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
        FatalError("failed");
    }
    if (NutDhcpIfConfig("eth0", 0, 60000)) {
        FatalError("no valid network configuration");
    }
    printf("OK\nRun 'telnet %s", inet_ntoa(confnet.cdn_ip_addr));
#if TCP_SERVER_PORT != 23
    printf(" %d", TCP_SERVER_PORT);
#endif
    puts("' to connect to this server");

    /* Set the start time. */
    start_time = time(NULL);

    /*
     * Now loop endless for client connections.
     *
     * Note, that we are only able to serve one client at a time.
     * If you want to allow concurrent connections, then this
     * loop must run in threads. Then each thread can handle one
     * client. See
     * http://www.ethernut.de/nutwiki/Multithreading
     */
    for (;;) {
        /* Create a socket. */
        if ((sock = NutTcpCreateSocket()) != 0) {
            printf("Waiting for a telnet client...");
            /* Listen on port 23. If we return, we got a client. */
            if (NutTcpAccept(sock, TCP_SERVER_PORT) == 0) {
                puts("connected");

                /*
                 * Open a stream and associate it with the socket, so 
                 * we can use standard I/O. Note, that socket streams
                 * currently do support cooked text mode.
                 */
                stream = _fdopen((int) sock, "r+b");
                if (stream) {
                    /* Process client requests as long as the connection is
                     * established.
                     *
                     * Note, that unplugging the network cable will not terminate
                     * a TCP connection by default. If you need this, you may set
                     * a receive timeout on the socket:
                     *
                     * uint32_t to = 5000;
                     * NutTcpSetSockOpt(sock, SO_RCVTIMEO, &to, sizeof(to));
                     *
                     * See also
                     * http://www.ethernut.de/nutwiki/Socket_Timeouts
                     */
                    ProcessRequests(stream);
                    /* Close the stream. */
                    fclose(stream);
                } else {
                    puts("Assigning a stream failed");
                }
            } else {
                puts("failed");
            }

            /* Close our socket. */
            NutTcpCloseSocket(sock);
            puts("Disconnected");
        }
    }
    /* Never reached, but required to suppress compiler warning. */
    return 0;
}
