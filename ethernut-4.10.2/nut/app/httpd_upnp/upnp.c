/****************************************************************************
*  Copyright (c) 2011 by Michael Fischer. All rights reserved.
*
*  This work based on source from proconX Pty Ltd. Therefore
*  partial copyright by: Copyright (c) 2010 proconX Pty Ltd. 
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
*****************************************************************************
*  History:
*
*  27.05.2011  mifi  First Version based on source from proconX Pty Ltd. 
*                    Information about UPnP can be found at "www.upnp.org".
*                    Note: I had no success with the UPnP Device Validator
*                    from the "Open Software Projects". The old original
*                    Intel tools from the book "UPnP Design by Example"
*                    works a little bit better, but has problems too.
****************************************************************************/
#define __MAIN_C__

#include <sys/version.h>
#include <sys/thread.h>
#include <sys/socket.h>
#include <sys/device.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <net/if_var.h>
#include <arpa/inet.h>
#include <pro/httpd.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/

/* 
 * Enable to fulfil the UPnP specification for notify messages.
 * I think this is not needed to advertise only the device on
 * windows networks.
 */
#define UPNP_FULFIL_SPEC      0

#ifndef UPNP_SERVICE_STACK
#define UPNP_SERVICE_STACK    ((1024 * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD)
#endif

#define UPNP_CGI_NAME         "upnp.cgi"
#define UPNP_UUID_PART1       "56F9C1D5-5083-4ee5-A6B3-"

#define SSDP_IP               0xfaffffefU       /* 239.255.255.250 */
#define SSDP_PORT             1900
#define SSDP_BUFFER_SIZE      512
#define SSDP_NOTIFY_TIMEOUT   300000U   /* 5 minutes */


/* *INDENT-OFF* */

static char HTTP_TEXT_XML[] = "text/xml; charset=\"utf-8\"";

static char NOTIFY_ALIVE[] =
   "NOTIFY * HTTP/1.1\r\n"
   "Host: 239.255.255.250:1900\r\n"
   "Cache-Control: max-age=900\r\n"
   "Location: http://%s/cgi-bin/" UPNP_CGI_NAME "\r\n"
   "NT: %s\r\n"
   "NTS: ssdp:alive\r\n"
   "USN: %s%s\r\n"
   "Server: %s\r\n"
   "Content-Length: 0\r\n"
   "\r\n";

static char NOTIFY_BYE[] =
   "NOTIFY * HTTP/1.1\r\n"
   "Host: 239.255.255.250:1900\r\n"
   "NT: upnp:rootdevice\r\n"
   "NTS: ssdp:byebye\r\n"
   "USN: %s::upnp:rootdevice\r\n"
   "Content-Length: 0\r\n"
   "\r\n";

static char MSEARCH_RESPONSE[] =
   "HTTP/1.1 200 OK\r\n"
   "Cache-Control: max-age=900\r\n"
   "Ext:\r\n"
   "Location: http://%s/cgi-bin/" UPNP_CGI_NAME "\r\n"
   "Server: %s\r\n"
   "ST: upnp:rootdevice\r\n"
   "USN: %s::upnp:rootdevice\r\n"
   "Content-Length: 0\r\n"
   "\r\n";
   
/*
 * All listed elements which are not tagged are "REQUIRED" as per
 * UPnP Device Architecture 1.0. Some other are "OPTIONAL" or "RECOMMENDED".
 */
static char HTML_UPNP[] =
"<?xml version=\"1.0\"?>\r\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"
   "<specVersion>\r\n"
      "<major>1</major>\r\n"
      "<minor>0</minor>\r\n"
   "</specVersion>\r\n"
   "<URLBase>http://%s</URLBase>\r\n"                                                  /* OPTIONAL */
   "<device>\r\n"
      "<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>\r\n"
      "<friendlyName>NutOS Server (%02X%02X%02X%02X%02X%02X)</friendlyName>\r\n"
      "<manufacturer>egnite</manufacturer>\r\n"
      "<manufacturerURL>http://www.egnite.de</manufacturerURL>\r\n"                    /* OPTIONAL */
      "<modelDescription>NutOS HTTP Daemon</modelDescription>\r\n"                     /* RECOMMENDED */
      "<modelName>Ethernut</modelName>\r\n"
      "<modelNumber>%02X%02X%02X%02X%02X%02X</modelNumber>\r\n"                        /* RECOMMENDED */
      "<modelURL>http://www.ethernut.de</modelURL>\r\n"                                /* OPTIONAL */
      "<serialNumber>%02X%02X%02X%02X%02X%02X</serialNumber>\r\n"                      /* RECOMMENDED */
      "<UDN>uuid:" UPNP_UUID_PART1 "%02X%02X%02X%02X%02X%02X</UDN>\r\n"
      
      /*
       * Despite being not required by the UPnP standard, we must
       * define at leat one service to make the device show up on Windows XP.
       */
      "<serviceList>\r\n"
         "<service>\r\n"
            "<serviceType>urn:schemas-upnp-org:service:Dummy:1</serviceType>\r\n"
            "<serviceId>urn:upnp-org:serviceId:Dummy</serviceId>\r\n"
            "<SCPDURL>/upnp/dummy.xml</SCPDURL>\r\n"
            "<controlURL>/upnp</controlURL>\r\n"
            "<eventSubURL></eventSubURL>\r\n"
         "</service>\r\n"
      "</serviceList>\r\n"
      "<presentationURL>http://%s/index.html</presentationURL>\r\n"                    /* RECOMMENDED */
   "</device>\r\n"
"</root>\r\n";

/* *INDENT-ON* */

/*=========================================================================*/
/*  DEFINE: Prototypes                                                     */
/*=========================================================================*/

/*=========================================================================*/
/*  DEFINE: Definition of all local Data                                   */
/*=========================================================================*/

static NUTDEVICE *dev;
static IFNET *nif;
static char MyUSN[48];
static char ServerInfo[32];

/*=========================================================================*/
/*  DEFINE: Definition of all local Procedures                             */
/*=========================================================================*/

/***************************************************************************/
/*  GetRand                                                                */
/***************************************************************************/
static uint32_t GetRand(uint32_t MaxValue)
{
    uint32_t Value;

    Value = rand() & MaxValue;

    return (Value);
} /* GetRand */

/***************************************************************************/
/*  LocationCGIHandler                                                     */
/***************************************************************************/
static int LocationCGIHandler(FILE * stream, REQUEST * req)
{
    /*
     * Generate HTTP header
     */
    NutHttpSendHeaderTop(stream, req, 200, "OK");
    NutHttpSendHeaderBottom(stream, 0, HTTP_TEXT_XML, -1L);

    /*
     * Output XML data
     */
    fprintf(stream, HTML_UPNP, 
            inet_ntoa(nif->if_local_ip),      /* URLBase */
            
            nif->if_mac[0], nif->if_mac[1],   /* friendlyName */
            nif->if_mac[2], nif->if_mac[3], 
            nif->if_mac[4], nif->if_mac[5], 
            
            nif->if_mac[0], nif->if_mac[1],   /* modelNumber */
            nif->if_mac[2], nif->if_mac[3], 
            nif->if_mac[4], nif->if_mac[5], 
            
            nif->if_mac[0], nif->if_mac[1],   /* serialNumber */
            nif->if_mac[2], nif->if_mac[3], 
            nif->if_mac[4], nif->if_mac[5], 
            
            nif->if_mac[0], nif->if_mac[1],   /* UDN */
            nif->if_mac[2], nif->if_mac[3], 
            nif->if_mac[4], nif->if_mac[5], 
            
            inet_ntoa(nif->if_local_ip));     /* presentationURL */

    return 0;
} /* LocationCGIHandler */

/***************************************************************************/
/*  SendNotifyAliveChunk                                                   */
/***************************************************************************/
static void SendNotifyAliveChunk(char *Buffer, UDPSOCKET * TxSock)
{
    int Size;

    /*
     * Send device discovery messages.
     * The first message is really needed.
     * All other to fulfil the UPnP specification.
     */

    /* Send first alive message */
    Size = sprintf(Buffer, NOTIFY_ALIVE, 
                   inet_ntoa(nif->if_local_ip), 
                   "upnp:rootdevice",           /* NT */
                   MyUSN, "::upnp:rootdevice",  /* USN */
                   ServerInfo);
    NutUdpSendTo(TxSock, SSDP_IP, SSDP_PORT, Buffer, Size);
    NutSleep(25);

#if (UPNP_FULFIL_SPEC >= 1)

    /* Send second alive message */
    Size = sprintf(Buffer, NOTIFY_ALIVE, 
                   inet_ntoa(nif->if_local_ip), 
                   MyUSN,     /* NT */
                   MyUSN, "", /* USN */
                   ServerInfo);
    NutUdpSendTo(TxSock, SSDP_IP, SSDP_PORT, Buffer, Size);
    NutSleep(25);

    /* Send third alive message */
    Size = sprintf(Buffer, NOTIFY_ALIVE, 
                   inet_ntoa(nif->if_local_ip), 
                   "urn:schemas-upnp-org:device:Basic:1",            /* NT */
                   MyUSN, "::urn:schemas-upnp-org:device:Basic:1",   /* USN */
                   ServerInfo);
    NutUdpSendTo(TxSock, SSDP_IP, SSDP_PORT, Buffer, Size);
    NutSleep(25);


    /*
     * Send embedded device discovery messages
     */

    /* No embedded device available */


    /*
     * Send service discovery messages
     */
    Size = sprintf(Buffer, NOTIFY_ALIVE, 
                   inet_ntoa(nif->if_local_ip), 
                   "urn:schemas-upnp-org:service:Dummy:1",           /* NT */
                   MyUSN, "::urn:schemas-upnp-org:service:Dummy:1",  /* USN */
                   ServerInfo);
    NutUdpSendTo(TxSock, SSDP_IP, SSDP_PORT, Buffer, Size);
#endif
} /* SendNotifyAliveChunk */

/***************************************************************************/
/*  SendNotifyAlive                                                        */
/***************************************************************************/
static void SendNotifyAlive(char *Buffer, UDPSOCKET * TxSock)
{
    uint32_t RandomDelay;

    /*
     * Send device discovery messages. Here two chunks will be send
     * with a random delay between each chunk.
     */

    SendNotifyAliveChunk(Buffer, TxSock);

    /* Create random value between 500...2000ms */
    RandomDelay = 500 + GetRand(1500);

    NutSleep(RandomDelay);

    SendNotifyAliveChunk(Buffer, TxSock);

} /* SendNotifyAlive */

/***************************************************************************/
/*  NotifyTask                                                             */
/***************************************************************************/
THREAD(NotifyTask, arg)
{
    char *Buffer;
    UDPSOCKET *TxSock;

    /*
     * Alloc buffer and create sockets
     */
    Buffer = malloc(SSDP_BUFFER_SIZE + 1);      /* Add one for the string termination 0 */
    TxSock = NutUdpCreateSocket(0);

    for (;;) {
        NutSleep(SSDP_NOTIFY_TIMEOUT);

        /* Send alive message. */
        SendNotifyAlive(Buffer, TxSock);

        NutThreadYield();
    }
} /* NotifyTask */

/***************************************************************************/
/*  SSDPTask                                                               */
/***************************************************************************/
THREAD(SSDPTask, arg)
{
    int Size;
    char *Buffer;
    UDPSOCKET *RxSock;
    UDPSOCKET *TxSock;
    uint32_t RemoteIP;
    uint16_t Port;
    char *Start;
    char *End;
    uint32_t MaxDelay;
    uint32_t Delay;

    sprintf(MyUSN, "uuid:%s%02X%02X%02X%02X%02X%02X",
            UPNP_UUID_PART1, 
            nif->if_mac[0], nif->if_mac[1], 
            nif->if_mac[2], nif->if_mac[3], 
            nif->if_mac[4], nif->if_mac[5]);

    sprintf(ServerInfo, "NutOS/%d.%d UPnP/1.0", NUT_VERSION_MAJOR, NUT_VERSION_MINOR);

    /*
     * Alloc buffer and create sockets
     */
    Buffer = malloc(SSDP_BUFFER_SIZE + 1);      /* Add one for the string termination 0 */
    RxSock = NutUdpCreateSocket(SSDP_PORT);
    TxSock = NutUdpCreateSocket(0);

    /*
     * First send ByeBye to the network.
     */
    Size = sprintf(Buffer, NOTIFY_BYE, MyUSN);
    NutUdpSendTo(TxSock, SSDP_IP, SSDP_PORT, Buffer, Size);

    NutSleep(500);

    /*
     * Now we can send a "hello" to the network, we are back.
     */
    SendNotifyAlive(Buffer, TxSock);

    for (;;) {
        Size = NutUdpReceiveFrom(RxSock, &RemoteIP, &Port, Buffer, SSDP_BUFFER_SIZE, NUT_WAIT_INFINITE);
        if (Size > 0) {
            Buffer[Size] = 0;   /* Terminate string */

            /*
             * Check if a control point serach for a network device.
             */
            if ((strstr(Buffer, "M-SEARCH") != NULL) &&        /* <= REQUIRED */
                (strstr(Buffer, "ssdp:discover") != NULL)) {   /* <= REQUIRED */
                /*
                 * Check if the control point search for "all", "root devices"
                 * or for a special device.
                 */
                if ((strstr(Buffer, "ssdp:all") != NULL) ||
                    (strstr(Buffer, "upnp:rootdevice") != NULL) || (strstr(Buffer, MyUSN) != NULL)) {
                    Start = strstr(Buffer, "MX:");
                    if (Start != NULL) {
                        /* Jump over the "MX:" */
                        Start += 3;

                        /* Get delay in seconds */
                        MaxDelay = strtol(Start, &End, 10);
                        if (MaxDelay == 0) {
                            MaxDelay = 1;
                        }

                        /* Change to millisecond */
                        MaxDelay *= 1000;

                        /* Get random value */
                        Delay = GetRand(MaxDelay);
                    } else {
                        /* Ups, error */
                        Delay = 1234;
                    }

                    NutSleep(Delay);

                    /*
                     * We must send a M-SEARCH response
                     */
                    Size = sprintf(Buffer, MSEARCH_RESPONSE, 
                                   inet_ntoa(nif->if_local_ip), 
                                   ServerInfo, MyUSN);
                    NutUdpSendTo(TxSock, RemoteIP, Port, Buffer, Size);
                    NutSleep(25);
                    NutUdpSendTo(TxSock, RemoteIP, Port, Buffer, Size);
                }
            }
        }

        NutThreadYield();
    }
} /* SSDPTask */

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/
/***************************************************************************/
/*  upnp_Init                                                              */
/***************************************************************************/
void upnp_Init(void)
{

    /* Set UPnP multicast address */
    printf("Add UPnP multicast address...");
    if (NutNetIfAddMcastAddr("eth0", SSDP_IP) != 0) {
        puts("failed");
    } else {
        puts("OK");

        /*
         * Multicast will be supported, 
         * get device and network interface info for eth0.
         */
        dev = NutDeviceLookup("eth0");
        if (dev != NULL) {
            /* Get network interface pointer */
            nif = dev->dev_icb;

            if (NutRegisterCgi(UPNP_CGI_NAME, LocationCGIHandler) == 0) {
                /* Start the UPnP threads */
                NutThreadCreate("ssdp", SSDPTask, NULL, UPNP_SERVICE_STACK);
                NutThreadCreate("ssdp-notify", NotifyTask, NULL, UPNP_SERVICE_STACK);
            }
        }
    }
} /* upnp_Init */

/*** EOF ***/
