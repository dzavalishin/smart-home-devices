#include "crc16.h"

#include "defs.h"
//#include "crumb_defs.h"
//#include "delay.h"
#include "adc.h"
#include "pwm.h"
#include "dports.h"
#include "packet_types.h"
#include "alarm.h"
#include "freq.h"
#include "temperature.h"
#include "util.h"
//#include "serial_number.h"
//#include "onewire.h"
//#include "timer0.h"

#include "net_io.h"

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h> // printf

#include <sys/socket.h>
#include <sys/thread.h>
#include <arpa/inet.h>


unsigned volatile char send_pong_flag = 0;
unsigned volatile char send_ack_flag = 0; // crashes if static!
unsigned volatile char send_nak_flag = 0;

// NB - unused on UDP io?
unsigned char ourNodeNumber = 0; // On start we are node zero. This is a broadcast address.

u_long multicast_addr;


#define PACKET_SIZE 8

#define UDP_PORT 16113
#define UDP_LONG_PORT 16114

void process_rx_data(unsigned char *rx_packet);
void process_rx_packet( unsigned char node, unsigned char type, unsigned char port, unsigned int data);
void process_broadcast_rx_packet( unsigned char type, unsigned char port, unsigned int data);






static UDPSOCKET * udp_send_socket;

THREAD(UDP_Recv, arg)
{
    UDPSOCKET *sock;
    u_short udp_bufsiz = 1024;

    u_long  	addr;
    u_short  	port;

    sock = NutUdpCreateSocket(UDP_PORT);
    NutUdpSetSockOpt(sock, SO_RCVBUF, &udp_bufsiz, sizeof(udp_bufsiz));

    udp_send_socket = NutUdpCreateSocket(0);

    char inPacket[PACKET_SIZE];

    for(;;)
    {
        int size = NutUdpReceiveFrom(
                                     sock, &addr, &port,
                                     inPacket, PACKET_SIZE, 0 );
        if( size == 0 )
        {
            NutThreadYield();
            continue;
        }

        process_rx_data(inPacket);
        NutThreadYield();
    }

}


void init_udp_net_io(void)
{
    //multicast_addr = inet_addr("238.16.0.13");
    multicast_addr = inet_addr("255.255.255.255");
    NutThreadCreate("UDP recv", UDP_Recv, (void *) (uptr_t) 0, 640);
}



void process_rx_data(unsigned char *rx_packet)
{
    //sei();
    //if(0 == rx_count )        return;

    unsigned int crc = crc16_calc(rx_packet, PACKET_SIZE-2);
    unsigned char crc_hi= (crc>>8) & 0xFFu;

    //if(rx_packet[0] == 0x11)        turn_led_on();

//turn_led_on();
    if(
       (rx_packet[PACKET_SIZE-2] != crc_hi) ||
       (rx_packet[PACKET_SIZE-1] != (crc & 0xFFu))
      )
    {
        send_nak_flag++;
        printf("CRC err - NAK sent, pkt: 0x.. ");

        int ii;
        for( ii = 0; ii < PACKET_SIZE; ii++ )
            printf("%02X ", rx_packet[ii] );

        printf("\n");

        return;
    }

//turn_led_on();

    unsigned char node = rx_packet[1];

    // Not for us?
    if( node != 0 && node != ourNodeNumber )
    {
        return;
    }

    unsigned char type = rx_packet[2];
    unsigned char port = rx_packet[3];
    unsigned int data = rx_packet[4];
    data |= (rx_packet[5]) << 8;
    process_rx_packet( node, type, port, data );
}


void process_rx_packet(
                       unsigned char node,
                       unsigned char type,
                       unsigned char port,
                       unsigned int data)
{
    if( (node == 0) && (type & 0x80) )
    {
        // Broadcast, process specially
        process_broadcast_rx_packet( type, port, data);
        return;
    }
    switch(type) {
    
    case TOHOST_NAK: break; // Echo?
    case TOHOST_ACK: break; // Echo?

    case FROMHOST_PING:
        send_pong_flag++;
        break;
    
    case FROMHOST_DIGITAL_REQUEST: request_dig_data_send(port); break;
    
    case FROMHOST_ANALOG_REQUEST: request_adc_data_send(port); break;

    case FROMHOST_DIGITAL_OUT_REQUEST:
    case FROMHOST_ANALOG_OUT_REQUEST:
        // TODO implement
        send_nak_flag++;
        break;

    case FROMHOST_FREQ_VALUE_REQUEST:  
#if SERVANT_NFREQ > 0
        request_freq_data_send(port); 
#endif
        break;

    case FROMHOST_TEMPERATURE_VALUE_REQUEST: 
#if N_TEMPERATURE_IN > 0
        request_temp_data_send(port);
#endif
        break;


    case FROMHOST_DIGITAL_OUT:
        set_dig(port, data);      send_ack_flag++;
        printf("set dig %d to 0x%02X\n", port, data);
        //fail_led();
        break;

    case FROMHOST_ANALOG_OUT:
#if SERVANT_NPWM > 0
        set_an(port, data);       send_ack_flag++;
#endif
        break;

    case FROMHOST_DIGITAL_OUT_ENABLE:
        set_ddr(port, data);      send_ack_flag++;
        printf("set ddr %d to 0x%02X\n", port, data);
        break;

    case FROMHOST_ANALOG_OUT_ENABLE:
    //case 0x31:			// включение PWM-ов
#if SERVANT_NPWM > 0
        pwm_mask_byte=data;
#endif
        send_ack_flag++;
        break;
        // "warning" setup
    case 0x40:		 	// задание битовой маски для предупреждающего сигнала
        warn_mask[port]=data;     send_ack_flag++;
        break;
    case 0x41:			//задание нижнего значения аналогового порта
        warn_min[port]=data;      send_ack_flag++;
        break;
    case 0x48:		 	// задание сравниваемого значения
        warn_byte[port]=data;     send_ack_flag++;
        break;
    case 0x49:			// задание верхнего значения
        warn_max[port]=data;      send_ack_flag++;
        break;
        // "alarm" setup
        // то же, что и для "warning"
    case 0x50:		 	// задание битовой маски
        al_mask[port]=data;       send_ack_flag++;
        break;
    case 0x51:			//задание нижнего значения аналогового порта
        al_min[port]=data;        send_ack_flag++;
        break;
    case 0x58:		 	// задание сравниваемого значения
        al_byte[port]=data;       send_ack_flag++;
        break;
    case 0x59:			// задание верхнего значения
        al_max[port]=data;        send_ack_flag++;
        break;

    default:
        send_nak_flag++;
        printf("unkn type 0x%02X, NAK\n", type );
        break;
    }
}


void process_broadcast_rx_packet(
                       unsigned char type,
                       unsigned char port,
                       unsigned int data)
{
    switch(type) {
    case FROMHOST_BROADCAST_SETNODE_A:
    case FROMHOST_BROADCAST_SETNODE_B:
    case FROMHOST_BROADCAST_SETNODE_C:
        // Just ignore - we're on UDP now
        break;
    }
 
}


void net_request_pong(void) {    send_pong_flag++; }

char pongNo = 9; // so that it will be sent at the beginning
void send_protocol_replies(void)
{
    while(send_nak_flag > 0)
    {
        send_nak_flag--;
        send_pack( TOHOST_NAK, 0, 0);
    }

    while(send_ack_flag > 0)
    {
        send_ack_flag--;
        send_pack( TOHOST_ACK, 0, 0);
    }

    while(send_pong_flag > 0)
    {
        send_pong_flag--;
        send_pack( TOHOST_PONG, 0, 0);
        if(pongNo++ > 2)
        {
            send_long_packet(TOHOST_SERIAL, OW_ROMCODE_SIZE, serialNumber );
            send_long_packet(TOHOST_DEVTYPE, sizeof(DEVICE_NAME)-1, DEVICE_NAME );
            pongNo = 0;
        }
    }

}

// отослать пакет на ПК
void send_pack(unsigned char send_type, unsigned char send_port, unsigned int send_data) {
    unsigned char pack[PACKET_SIZE];
    unsigned int crc, crc1;

    pack[0]=0x12;
    pack[1]=ourNodeNumber;
    pack[2]=send_type;
    pack[3]=send_port;
    pack[4]=send_data;
    pack[5]=send_data >> 8;

    crc=crc16_calc(pack, PACKET_SIZE-2);
    crc1=crc>>8;

    pack[6]= crc1;	// старший байт crc
    pack[7]= crc;	// младший байт crc

    NutUdpSendTo(udp_send_socket, multicast_addr, UDP_PORT, pack, PACKET_SIZE );
}

/**
 *
 * Send extended packet up to 255 bytes long.
 *
 * Packet is a combination of standard short packet of a special
 * type and extended data with separate additional CRC bytes.
 *
 * Short packet's type is fixed, data byte is length of extension
 * (without CRC) and port number is used for extended packet type. 
 *
 * Java code also supposes that first byte of ext data is 
 * port, if applicable.
 *
**/


//void send_long_packet(unsigned char short_val, unsigned char type, unsigned char port, unsigned char long_len, unsigned char *long_data )
//static int long_pack_id = 0;

void send_long_packet(unsigned char ext_type, unsigned char long_len, unsigned char *long_data )
{
    unsigned char *pp = malloc(long_len+1);
    if(pp == 0) return; // what can we do else? TODO send some failure info/pkt!
    //send_pack( TOHOST_LONG_PACKET, ext_type, ++long_pack_id );
    *pp = ext_type;
    //*((int*)(pp+1)) = long_len;
    memcpy( pp+1, long_data, long_len );
    NutUdpSendTo(udp_send_socket, multicast_addr, UDP_LONG_PORT, pp, long_len+1 );
    free(pp);
}



