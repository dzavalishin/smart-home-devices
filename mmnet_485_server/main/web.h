#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/socket.h>
#include <pro/httpd.h>

// httpd
THREAD(Service, arg);


int ShowForm(FILE * stream, REQUEST * req);


#define HTML(__text) do {     static prog_char form[] = __text;    fputs_P(form, stream); } while(0)


