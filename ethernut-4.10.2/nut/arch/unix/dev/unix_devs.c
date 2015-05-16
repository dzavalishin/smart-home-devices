/*
 * Copyright (C) 2000-2004 by ETH Zurich
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
 * THIS SOFTWARE IS PROVIDED BY ETH ZURICH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ETH ZURICH
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/* unix_devs.c - a nut/os device driver for native unix devices
 *
 * 2004.04.01 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 * \todo check block read implementation
 * \todo allow native filee accesss using names like "FAT_C:/.." \see fs/fat.c
 * \todo implement cooked mode and use it as default mode
 *
 */

/* avoid stdio nut wrapper */
#define NO_STDIO_NUT_WRAPPER

#include <fcntl_orig.h>
#include <arch/unix.h>
#include <sys/atom.h>
#include <sys/device.h>
#include <sys/file.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <dev/usart.h>
#include <dev/irqreg.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include <dev/unix_devs.h>

// for sockets 
#include <sys/types.h>
#include <sys/socket.h>  // 
#include <netinet/in.h>  // 
#include <arpa/inet.h>   // 
#include <netdb.h>
#include <sys/uio.h>     
#include <unistd.h>
#include <string.h>


#ifdef __CYGWIN__
#include <sys/select.h>
#endif

/* on mac os x, not all baud rates are defined in termios.h but
   they are mapped to the numeric value anyway, so we define them here */
#ifdef __APPLE__
#ifndef B460800
#define B460800 460800
#endif
#ifndef B500000
#define B500000 500000
#endif
#ifndef B576000
#define B576000 576000
#endif
#ifndef B921600
#define B921600 921600
#endif
#endif



/* thread attributes */
pthread_attr_t unix_devs_attr;

/* protect thread signaling */
pthread_mutex_t unix_devs_mutex;

/* to get a new thread start acked'd */
pthread_cond_t unix_devs_cv;


/*
 * functions available on avr somehow -- not implemented properly here :(
 */
char *dtostre(double f, char *str, uint8_t prec, uint8_t flags);
char *dtostre(double f, char *str, uint8_t prec, uint8_t flags)
{
    sprintf(str, "%e", f);
    return str;
}
char *dtostrf(double f, char width, char prec, char *str);
char *dtostrf(double f, char width, char prec, char *str)
{
    sprintf(str, "%f", f);
    return str;
}

/*
 * Quite unnecessary functions, because linux doesn't define B19200 as 19200
 * we have to map those value to their symbolic counterparts:
 * 0
 * 50
 * 75
 * 110
 * 134
 * 150
 * 200
 * 300
 * 600
 * 1200
 * 1800
 * 2400
 * 4800
 * 9600
 * 19200
 * 38400
 * 57600
 * 115200
 * 230400
 * 460800
 * 500000
 * 576000
 * 921600
 */
static int convertToRealSpeed(int baudSpeed)
{
    switch (baudSpeed) {
    case B0:
        return 0;
    case B50:
        return 50;
    case B75:
        return 75;
    case B110:
        return 110;
    case B134:
        return 134;
    case B150:
        return 150;
    case B200:
        return 200;
    case B300:
        return 300;
    case B600:
        return 600;
    case B1200:
        return 1200;
    case B1800:
        return 1800;
    case B2400:
        return 2400;
    case B4800:
        return 4800;
    case B9600:
        return 9600;
    case B19200:
        return 19200;
    case B38400:
        return 38400;
    case B57600:
        return 57600;
    case B115200:
        return 115200;
    case B230400:
        return 230400;
#ifndef __CYGWIN__
    case B460800:
        return 460800;
    case B500000:
        return 500000;
    case B576000:
        return 576000;
    case B921600:
        return 921600;
#endif
    }
    return -1;
}


static int convertToBaudSpeed(int realSpeed)
{
    switch (realSpeed) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
#ifndef __CYGWIN__
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
#endif
    }
    return -1;
}

/* ======================= sockets ======================== */

unsigned int resolve(char *);
unsigned int resolve(char *ip_addr)
{
    struct hostent *hp;
    unsigned int ip;
    hp = gethostbyname(ip_addr);
    if (!hp)
    {
        ip = inet_addr(ip_addr);
        if ((int)ip == -1) {
            return -1;
        } else {
            return ip;
        }
    }
    // hp->h_length should equal to 4
    memcpy(&ip, hp->h_addr, 4);
    return ip;
}


/*
 * IRQ Handler for correct signaling
 * not currently used (see UnixDevReadThread for more info on variants
static void UnixDevRxIntr(void *arg){
    NUTDEVICE* dev = (NUTDEVICE*) arg;
    UNIXDCB *  dcb = (UNIXDCB*) dev->dev_dcb;
    // printf("UnixDevRxIntr(%s)\n",dev->dev_name);
    NutEventPostFromIrq( &dcb->dcb_rx_rdy);
}
 */


/*
 * Read Thread
 *
 */
static void *UnixDevReadThread( void * arg )
{
    int ret;
    int fd;
    fd_set rfd_set;

    NUTDEVICE* dev = (NUTDEVICE*) arg;
    UNIXDCB *  dcb = (UNIXDCB*) dev->dev_dcb;
    
    // fd
    fd = dcb->dcb_fd;
    if (fd == STDOUT_FILENO)
        fd = STDIN_FILENO;

    // non-nut thread => block IRQ signals
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);

    // printf("UnixDevReadThread() started\n");
    
    // be ready to receive requests
    pthread_mutex_lock(&dcb->dcb_rx_mutex);

    // confirm start
    pthread_mutex_lock(&unix_devs_mutex);
    pthread_cond_signal(&unix_devs_cv);
    pthread_mutex_unlock(&unix_devs_mutex);

    // printf("UnixDevReadThread(%s) start confirmed\n", dev->dev_name);

    // 
    for (;;) {

        pthread_cond_wait(&dcb->dcb_rx_trigger, &dcb->dcb_rx_mutex);

        // printf("UnixDevReadThread(%s) triggered\n", dev->dev_name);

        // wait for data to become ready //
        do {

            FD_ZERO(&rfd_set);
            FD_SET(fd, &rfd_set);
            ret = select(fd + 1, &rfd_set, NULL, NULL, NULL);
        } while (FD_ISSET(fd, &rfd_set) == 0);

        // printf("UnixDevReadThread(%s) task processed\n", dev->dev_name);

        // signale waiting thread

        /* version 1 (working but could cause a race condition)
        NutEventPostAsync( &dcb->dcb_rx_rdy);
        */
        
        /* version 2 (correct, but currently very slow)
        if (dev->dev_name[4] == '0') {
            NutIRQTrigger(IRQ_UART0_RX);
        } else {
            NutIRQTrigger(IRQ_UART1_RX);
        }*/
 
        /* version 3 (another hack, but fast and correct */
        switch (dev->dev_name[4]) {
            case '0':
                NutUnixIrqEventPostAsync( IRQ_UART0_RX,  &dcb->dcb_rx_rdy);
                break;
            case '1':
                NutUnixIrqEventPostAsync( IRQ_UART1_RX,  &dcb->dcb_rx_rdy);
                break;
            case '2':
                NutUnixIrqEventPostAsync( IRQ_UART2_RX,  &dcb->dcb_rx_rdy);
                break;
        }

    }
   
    return 0;
}


/*!
 * \brief Open UnixDev
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *UnixDevOpen(NUTDEVICE * dev, const char *name, int mode, int acc)
{
    NUTFILE *nf;

    int nativeFile;
    char *nativeName;
    struct termios t;
    long baud;
    pthread_t *thread;

    // sockets
    struct sockaddr_in sinaddr;
    struct sockaddr_in remote;
    unsigned int remote_ip;
    unsigned int remote_port;
    char *ip;
    char *port;
//    char *idx;
    
    // map from dev->name to unix name
    if (strncmp("uart", dev->dev_name, 4) == 0) {
        // uart
        switch (dev->dev_name[4]) {
            case '0':
                nativeName = emulation_options.uart_options[0].device;
                break;
            case '1':
                nativeName = emulation_options.uart_options[1].device;
                break;
            case '2':
                nativeName = emulation_options.uart_options[2].device;
                break;
            default:
                return NULL;
        }
    } else
        return NULL;

    // check for sockets
    // try to split "device name" into ip:port
    if (strncmp("stdio", nativeName, 5)==0) {
        ip = nativeName;
        port = NULL;
    } else { 
        ip = strtok( nativeName, ":");
        port = strtok ( NULL, ":");
    }

    // printf("UnixDevOpen: Nut name = %s, unix name = %s\n",  dev->dev_name, nativeName);

    // determine mode -- not implemented yet 
    // set default mode
    mode = O_RDWR | O_NOCTTY;

    // fopen unix device
    if (strcmp("stdio", nativeName) == 0) {

        nativeFile = STDOUT_FILENO;

        // make raw
        if (tcgetattr(nativeFile, &t) == 0) {

            /* set input mode (non-canonical, no echo,...) but allow INTR signal */
            // bzero(&t, sizeof(t));
            t.c_lflag = ISIG;
            t.c_cc[VTIME] = 0;  /* inter-character timer unused */
            t.c_cc[VMIN] = 0;   /* non-blocking read */

            // apply file descriptor options
            if (tcsetattr(nativeFile, TCSANOW, &t) < 0) {
                printf("UnixDevOpen: tcsetattr failed\n\r");
            }
            ((UNIXDCB*)dev->dev_dcb)->dcb_socket = 0;
        }

    } else if (port) {

        // tcp/ip socket
        remote_ip = resolve ( ip );
        remote_port = atoi( port );
        
        if ( ip ) {
        
            NutEnterCritical();

            // create socket
            nativeFile = socket(AF_INET, SOCK_STREAM, 0);
            
            // set inbound address
            sinaddr.sin_family = AF_INET;
            sinaddr.sin_addr.s_addr = INADDR_ANY;
            sinaddr.sin_port = htons(0);

            // bind to socket
            bind(nativeFile, (struct sockaddr *)&sinaddr, sizeof(sinaddr));
            
            // set remote address
            remote.sin_family = AF_INET;
            remote.sin_port = htons(remote_port);
            remote.sin_addr.s_addr = remote_ip;
            
            // try to connect
            if ( connect ( nativeFile, (struct sockaddr *)&remote, sizeof(remote) ) == 0) {
                // connected
                ((UNIXDCB*)dev->dev_dcb)->dcb_socket = 1;
                
            } else {
                NutExitCritical();
                printf( "UnixDevOpen: Connect to %s port %d failed (errno = %d)\n\r", ip, remote_port, errno);
                return NULL;
            }            
        } else {
            NutExitCritical();
            printf("UnixDevOpen: Could not resolve IP address of '%s'!\n", ip);
            return NULL;
        }  
        

    } else {

        nativeFile = open(nativeName, mode);
        
        if (nativeFile < 0) {
            printf("UnixDevOpen: open('%s',%d) failed!\n", nativeName, mode);
            return NULL;
        }
        
        /* flush pending data*/
        if (tcflush( nativeFile, TCIOFLUSH)) {
            printf("UnixDevOpen: tcflush('%s',%d) failed!\n", nativeName, TCIOFLUSH);
            return NULL;
        } 

        if (tcgetattr(nativeFile, &t) == 0) {

            baud = convertToBaudSpeed(USART_INITSPEED);

            bzero(&t, sizeof(t));
            t.c_cflag = CS8 | CLOCAL | CREAD;
            t.c_iflag = IGNPAR;
            t.c_oflag = 0;

            cfsetospeed(&t, baud);
            cfsetispeed(&t, baud);

            /* set input mode (non-canonical, no echo,...) */
            t.c_lflag = 0;
            t.c_cc[VTIME] = 0;  /* inter-character timer unused */
            t.c_cc[VMIN] = 0;   /* non-blocking read */

            tcflush(nativeFile, TCIFLUSH);

            // apply file descriptor options
            if (tcsetattr(nativeFile, TCSANOW, &t) < 0) {
                printf("UnixDevOpen: tcsetattr failed\n\r");
            }
        }
        ((UNIXDCB*)dev->dev_dcb)->dcb_socket = 0;

    }

    if (nativeFile == 0)
        return NULL;

    // set non-blocking
    if (fcntl(nativeFile, F_SETFL, O_NONBLOCK) < 0) {
        printf("UnixDevOpen: fcntl O_NONBLOCK failed\n\r");
    }

    // store unix fd in dev
    ((UNIXDCB*)dev->dev_dcb)->dcb_fd = nativeFile;

    // printf("UnixDevOpen: %s, fd * %d\n\r", nativeName, nativeFile);
    // printf("UnixDevOpen: stdout %d, stdin %d, stderr %d \n\r", fileno(stdin), fileno(stdout), fileno(stderr));

    /* initialized rx IRQ handler -- not currently used (see UnixDevReadThread for more info on variants 
    if (dev->dev_name[4] == '0') {
        NutRegisterIrqHandler(IRQ_UART0_RX, UnixDevRxIntr, dev);
    } else
    {
        NutRegisterIrqHandler(IRQ_UART1_RX, UnixDevRxIntr, dev);
    }
    */
        
    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&unix_devs_mutex, NULL);
    pthread_mutex_init(&((UNIXDCB*)dev->dev_dcb)->dcb_rx_mutex, NULL);

    pthread_attr_init(&unix_devs_attr);
    pthread_attr_setdetachstate(&unix_devs_attr, PTHREAD_CREATE_JOINABLE);

    // unix_devs_cv init
    pthread_cond_init(&unix_devs_cv, NULL);
    pthread_cond_init( &((UNIXDCB*)dev->dev_dcb)->dcb_rx_trigger, NULL);

    // get thread struct
    thread = malloc(sizeof(pthread_t));

    // lock mutex and start thread
    pthread_mutex_lock(&unix_devs_mutex);
    pthread_create(thread, &unix_devs_attr, UnixDevReadThread, dev);

    // printf("UnixDevOpen: %s, waiting for UnixDevRead ack\n\r", nativeName);

    // wait for ack
    pthread_cond_wait(&unix_devs_cv, &unix_devs_mutex);
    pthread_mutex_unlock(&unix_devs_mutex);

    // printf("UnixDevOpen: %s, ack from UnixDevRead received\n\r", nativeName);

    // create new NUTFILE using malloc
    nf = malloc(sizeof(NUTFILE));

    // enter data
    nf->nf_next = 0;
    nf->nf_dev = dev;
    nf->nf_fcb = 0;

    return nf;
}


/*!
 * \brief Blocking write bytes to file
 *
 * \return Number of characters sent.
 */
static int UnixDevWrite(NUTFILE * nf, CONST void *buffer, int len)
{
    int rc;
    int remaining = len;
    UNIXDCB * dcb = (UNIXDCB*) nf->nf_dev->dev_dcb;
    
    /* flush ? */
    if (len == 0)
        return 0;
        
    do {
        rc = write(dcb->dcb_fd, buffer, remaining);
        if (rc > 0) {
            buffer += rc;
            remaining -= rc;
        }
    } while (remaining > 0);
    return len;
}



 
/*!
 * \brief Read bytes from file
 *
 * \return Number of characters read.
 */
static int UnixDevRead(NUTFILE * nf, void *buffer, int len)
{
    int newBytes;
    int fd;
    fd_set rfd_set;
    int ret;
    struct timeval timeout;

    int rc = 0;
    UNIXDCB * dcb = (UNIXDCB*) nf->nf_dev->dev_dcb;

    // fd
    fd = dcb->dcb_fd;
    if (fd == STDOUT_FILENO)
        fd = STDIN_FILENO;
    
    // test for read len. len == 0 => flush fd
    if (len == 0){
        tcflush(fd, TCIFLUSH);
        return 0;
    }
    
    // printf("UnixDevRead: called: len = %d\n\r",len);
    timeout.tv_usec = 0;
    timeout.tv_sec  = 0;

    do {

        // data available ?
        FD_ZERO(&rfd_set);
        FD_SET(fd, &rfd_set);
        ret = select(fd + 1, &rfd_set, NULL, NULL, &timeout);
      
        if (FD_ISSET(fd, &rfd_set) == 0) {

            // no data available. let's block the nut way...

            // printf("UnixDevRead(%s): no data ready, signaling read thread\n\r", nf->nf_dev->dev_name);

            // prepared signaling
            dcb->dcb_rx_rdy = 0;

            // lock mutex and signal read thread
            pthread_mutex_lock(&dcb->dcb_rx_mutex);
            pthread_cond_signal(&dcb->dcb_rx_trigger);
            pthread_mutex_unlock(&dcb->dcb_rx_mutex);

            // printf("UnixDevRead(%s): no data ready, waiting for answer\n\r", nf->nf_dev->dev_name);

            // wait for data to be ready
            NutEventWait( &dcb->dcb_rx_rdy, dcb->dcb_rtimeout);

            // printf("UnixDevRead(%s): got answer\n\r", nf->nf_dev->dev_name);

        }

        //  printf("UnixDevRead(%s): before read\n\r", nf->nf_dev->dev_name);
        newBytes = read( fd, buffer, len);
        // printf("UnixDevRead(%s): read some bytes. res = %d\n\r", nf->nf_dev->dev_name, newBytes);

        /* error or timeout ? */
        if (newBytes < 0) {

            if (errno == EAGAIN) {
                // timeout. No data available right now
                // printf("UnixDevRead: no bytes available, trying again\n\r");
                errno = 0;
                continue;
            } else {

                printf("UnixDevRead: error %d occured, giving up\n\r", errno);
                return newBytes;
            }
        } else
            rc += newBytes;

#ifdef UART_SETBLOCKREAD
        // printf("UnixDevRead: UART_SETBLOCKREAD defined\n\r");
        // check for blocking read: all bytes received
        if ( (dcb->dcb_modeflags & USART_MF_BLOCKREAD) && (rc < len)) {
            // printf("UnixDevRead: block read enabled, but not enough bytes read \n\r");
            continue;
        }
#endif
        // did we get one?
        if (rc > 0)
            break;

    } while (1);

    return rc;
}

/*! 
 * \brief Close ...
 *
 * \return Always 0.
 */
static int UnixDevClose(NUTFILE * nf)
{
    UNIXDCB * dcb = (UNIXDCB*) nf->nf_dev->dev_dcb;
    if ( dcb->dcb_fd > STDERR_FILENO)
        close( dcb->dcb_fd );
    return 0;
}

/*!
 * \brief Perform USART control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - \ref UART_SETSPEED
 *             - \ref UART_GETSPEED
 *             - \ref UART_SETDATABITS
 *             - \ref UART_GETDATABITS
 *             - \ref UART_SETSTOPBITS
 *             - \ref UART_GETSTOPBITS
 *             - \ref UART_SETPARITY
 *             - \ref UART_GETPARITY
 *             - \ref UART_SETFLOWCONTROL
 *             - \ref UART_GETFLOWCONTROL
 *               - \ref UART_SETBLOCKREAD
 *               - \ref UART_GETBLOCKREAD
 *               - \ref UART_SETSTATUS
 *               - \ref UART_GETSTATUS
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 * \warning Timeout values are given in milliseconds and are limited to 
 *          the granularity of the system timer. To disable timeout,
 *          set the parameter to NUT_WAIT_INFINITE.
 */
int UnixDevIOCTL(NUTDEVICE * dev, int req, void *conf)
{
    struct termios t;
    uint32_t *lvp = (uint32_t *) conf;
    uint32_t lv = *lvp;
    UNIXDCB * dcb = (UNIXDCB*) dev->dev_dcb;
    
    // printf("UnixDevIOCTL, native %x, req: %d, lv: %ld\n\r", dcb->dcb_fd , req, lv);

    switch (req) {

    case UART_SETSPEED:
    case UART_SETFLOWCONTROL:
    case UART_SETPARITY:
    case UART_SETDATABITS:
    case UART_SETSTOPBITS:

        // ok on stdio
        if ( dcb->dcb_fd <= STDERR_FILENO)
            return 0;

        // ok on sockets
        if ( dcb->dcb_socket)
            return 0;

        if (tcgetattr( dcb->dcb_fd , &t)) {
            printf("UnixDevIOCTL, tcgetattr failed\n\r");
            return -1;
        }

        switch (req) {

        case UART_SETSPEED:
            lv = convertToBaudSpeed(lv);
            cfsetospeed(&t, lv);
            cfsetispeed(&t, lv);

            break;

        case UART_SETFLOWCONTROL:
            switch (lv) {
            case 0:
                t.c_cflag &= ~CRTSCTS;
                t.c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
            case UART_HS_SOFT:
                t.c_cflag &= ~CRTSCTS;
                t.c_iflag |= (IXON | IXOFF | IXANY);
                return -1;
            case UART_HS_MODEM:
                return -1;
            case UART_HS_RTSCTS:
                t.c_cflag |= CRTSCTS;
                t.c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
            default:
                return -1;
            }
            break;

        case UART_SETPARITY:
            // 0 (none), 1 (odd) or 2 (even).
            t.c_cflag &= ~(PARODD | PARENB);
            switch (lv) {
            case 0:
                break;
            case 1:
                t.c_cflag |= PARENB | PARODD;
                break;
            case 2:
                t.c_cflag |= PARENB;
            default:
                return -1;
            }
            break;


        case UART_SETDATABITS:
            t.c_cflag &= ~CSIZE;
            switch (lv) {
            case 5:
                t.c_cflag |= CS5;
                break;
            case 6:
                t.c_cflag |= CS6;
                break;
            case 7:
                t.c_cflag |= CS7;
                break;
            case 8:
                t.c_cflag |= CS8;
                break;
            default:
                return -1;
            }
            break;

        case UART_SETSTOPBITS:
            switch (lv) {
            case 1:
                t.c_cflag &= ~CSTOPB;
                break;
            case 2:
                t.c_cflag |= CSTOPB;
                break;
            default:
                return -1;
            }
            break;

        }

        /* tcdrain fails on mac os x for some unknown reason -- work around */
        while (tcdrain( dcb->dcb_fd ) < 0) {
            // printf("UnixDevIOCTL: tcdrain failed: errno: %d\n\r", errno);
            errno = 0;
            usleep(1000);
        }

        if (tcsetattr( dcb->dcb_fd , TCSANOW, &t) < 0) {
            printf("UnixDevIOCTL: tcsetattr failed: errno: %d\n\r", errno);
            errno = 0;
            return -1;
        }
        return 0;

    case UART_GETSPEED:
    case UART_GETFLOWCONTROL:
    case UART_GETPARITY:
    case UART_GETDATABITS:
    case UART_GETSTOPBITS:

        if ((dcb->dcb_fd <= STDERR_FILENO) || ( dcb->dcb_socket))
        {
            // default answers for sockets / stdio
            switch (req){
                case UART_GETSPEED:
                    *lvp = 9600; return 0;
                case UART_GETFLOWCONTROL:
                    *lvp = 0; return 0;
                case UART_GETPARITY:
                    *lvp = 0; return 0;
                case UART_GETDATABITS:
                    *lvp = 8; return 0;
                case UART_GETSTOPBITS:
                    *lvp = 1; return 0;
            }
            return 0;
        }
        
        if (tcgetattr(dcb->dcb_fd, &t) != 0)
            return -1;

        switch (req) {

        case UART_GETSPEED:
            *lvp = convertToRealSpeed(cfgetospeed(&t));
            break;

        case UART_GETFLOWCONTROL:
            if (t.c_cflag & CRTSCTS)
                *lvp = UART_HS_RTSCTS;
            else if (t.c_iflag & IXANY)
                *lvp = UART_HS_SOFT;
            else
                *lvp = 0;
            break;

        case UART_GETPARITY:
            if (t.c_cflag & PARENB) {
                if (t.c_cflag & PARODD)
                    *lvp = 1;
                else
                    *lvp = 2;
            } else
                *lvp = 0;
            break;

        case UART_GETDATABITS:
            switch (t.c_cflag & CSIZE) {
            case CS5:
                *lvp = 5;
                break;
            case CS6:
                *lvp = 6;
                break;
            case CS7:
                *lvp = 7;
                break;
            case CS8:
                *lvp = 8;
                break;
            default:
                return -1;
            }
            break;

        case UART_GETSTOPBITS:
            if (t.c_cflag & CSTOPB)
                *lvp = 2;
            else
                *lvp = 1;
            break;
        }
        return 0;

#ifdef UART_SETBLOCKREAD
    case UART_SETBLOCKREAD:
        if (lv)
            dcb->dcb_modeflags |= USART_MF_BLOCKREAD;
        else
            dcb->dcb_modeflags &= ~USART_MF_BLOCKREAD;
        return 0;

    case UART_GETBLOCKREAD:
        if ( dcb->dcb_modeflags & USART_MF_BLOCKREAD)
            *lvp = 1;
        else
            *lvp = 0;
        return 0;
#endif

    case UART_SETCOOKEDMODE:
        if (*lvp == 0)
            return 0;
        else
            return -1;
            
    case UART_GETSTATUS:
        *lvp = 0;
        return 0;

    case UART_SETTXBUFSIZ:
    case UART_SETRXBUFSIZ:
    case UART_SETTXBUFHWMARK:
    case UART_SETRXBUFHWMARK:
    case UART_SETTXBUFLWMARK:
    case UART_SETRXBUFLWMARK:
    case UART_SETSTATUS:
        return 0;
        
    default:
        return -1;
    }
    return -1;
}




/* ======================= Devices ======================== */

/*!
 * \brief USART0 device control block structure.
 */
static UNIXDCB dcb_usart0 = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    0,                          /* dbc_last_eol */
    0,                          /* dcb_fd */
    0,                          /* dcb_rx_rdy */
/*  xx,                            dcb_rx_mutex */
/*  xx,                            dcb_rx_trigger */
};

/*!
 * \brief USART1 device control block structure.
 */
static UNIXDCB dcb_usart1 = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    0,                          /* dbc_last_eol */
    0,                          /* dcb_fd */
    0,                          /* dcb_rx_rdy */
/*  xx,                            dcb_rx_mutex */
/*  xx,                            dcb_rx_trigger */
};

/*!
 * \brief USART1 device control block structure.
 */
static UNIXDCB dcb_usart2 = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    0,                          /* dbc_last_eol */
    0,                          /* dcb_fd */
    0,                          /* dcb_rx_rdy */
/*  xx,                            dcb_rx_mutex */
/*  xx,                            dcb_rx_trigger */
};

/*!
 * \brief Debug device 0 information structure.
 */
NUTDEVICE devDebug0 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart0,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};

/*!
 * \brief Debug device 1 information structure.
 */
NUTDEVICE devDebug1 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '1', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart1,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};


/*!
 * \brief uart device 0 information structure.
 */
NUTDEVICE devUart0 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart0,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};

/*!
 * \brief uart device 1 information structure.
 */
NUTDEVICE devUart1 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '1', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart1,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};

/*!
 * \brief uart device 2 information structure.
 */
NUTDEVICE devUart2 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '2', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart2,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};

/*!
 * \brief usartavr device 0 information structure.
 */
NUTDEVICE devUsartAvr0 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart0,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};

/*!
 * \brief usartavr device 1 information structure.
 */
NUTDEVICE devUsartAvr1 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '1', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart1,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};

/*!
 * \brief usartavr device 2 information structure.
 */
NUTDEVICE devUsartAvr2 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '2', 0, 0, 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_usart2,                /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    UnixDevIOCTL,               /*!< Driver specific control function. */
    UnixDevRead,
    UnixDevWrite,
    UnixDevOpen,
    UnixDevClose,
    0
};


/*@}*/
