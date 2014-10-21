#include "modbus.h"
#include "uart.h"
#include "modbus_reg.h"
#include "util.h"


static void modbus_copy_rx_tx( int nb );
static void modbus_send_packet( void );

#if HAVE_MODBUS_RTU
static void modbus_process_rtu_pkt(void);
void modbus_make_tx_CRC(void);
void modbus_timeout_occured(void);
#endif

#if HAVE_MODBUS_ASCII
static void modbus_process_ascii_pkt(void);
#endif

#if HAVE_DCON
//void dcon_process_pkt( uint8_t *cp, uint8_t len );
#endif

static void modbus_decode_pkt( unsigned char type, unsigned char *rx, int rx_size );
static void modbus_send_error_reply(int cmd, int nerr);


unsigned char modbus_our_address = MODBUS_DEFAULT_ADDRESS;

unsigned char rx_corrupt = 0;
unsigned char process_rx_packet_flag = 0;
unsigned char rx_stop = 0;

unsigned char modbus_rx_buf[MODBUS_MAX_RX_PKT];
unsigned char rx_pos = 0;


unsigned char modbus_tx_buf[MODBUS_MAX_TX_PKT];
unsigned char tx_len = 0;
unsigned char tx_start = 0;

uint16_t modbus_event_cnt;
uint16_t modbus_crc_cnt;
uint16_t modbus_exceptions_cnt;



static uint8_t is_for_us(void)
{
    return (modbus_rx_buf[0] == modbus_our_address) || (modbus_rx_buf[0] == 0);
}


#if HAVE_MODBUS_RTU
int modbus_timeout_ms = 11;

int modbus_calc_timeout_ms( int baud )
{
    baud /= 10; // t0 bits per byte, now have byte frequency

    if( baud > 1000 )
        return 1;

    return (1000/baud)+1;
}
#endif

void modbus_init( void )
{
    modbus_set_baud( DEFAULT_UART_BAUD );
}

void modbus_set_baud( uint16_t baud )
{

#if HAVE_MODBUS_RTU
    // +6 is to make sure one callout is ignored - it can happen too early
    modbus_timeout_ms = modbus_calc_timeout_ms( baud ) + 6;
#endif

    uart_set_baud( baud );
}


#if HAVE_MODBUS_RTU
int modbus_timer = 0;

void modbus_start_timer( void )
{
    modbus_timer = modbus_timeout_ms;
}

// Called by timer code once a 5 msec
void modbus_timer_callout_5msec()
{
    if( modbus_timer <=0 )
        return;

    modbus_timer -= 5;

    if( modbus_timer <=0 )
        modbus_timeout_occured();
}


void modbus_timeout_occured()
{
    if( rx_corrupt )
    {
        rx_pos = 0; // just ignore packet, start over
        return;
    }

    uart_auto_protocol = PROTOCOL_MODBUS_RTU;
    process_rx_packet_flag = 1;
}
#endif


void modbus_rx_byte(unsigned char inbyte)
{
    if( process_rx_packet_flag )
        return;

    if( rx_pos >= MODBUS_MAX_RX_PKT )
    {
        rx_corrupt = 1; // TODO clear and restart here, don't wait 4 timeout
        return;
    }

#if HAVE_MODBUS_RTU
    modbus_start_timer();
#endif

    modbus_rx_buf[rx_pos++] = inbyte;

#if HAVE_MODBUS_ASCII
    if( is_modbus_ascii_frame( modbus_rx_buf, rx_pos ) )
    {
        uart_auto_protocol = PROTOCOL_MODBUS_ASCII;
        process_rx_packet_flag = 1;
    }
#endif
#if HAVE_DCON
    if( is_dcon_frame( modbus_rx_buf, rx_pos ) )
    {
        uart_auto_protocol = PROTOCOL_DCON;
        process_rx_packet_flag = 1;
    }
#endif
}



// Called from main
void modbus_process_rx(void)
{
//flash_led2_once();
    if(!process_rx_packet_flag) return;

    if(!rx_corrupt)
    {
#if HAVE_MODBUS_RTU
        if( uart_auto_protocol == PROTOCOL_MODBUS_RTU )
            modbus_process_rtu_pkt();
#endif
#if HAVE_MODBUS_ASCII
        if( uart_auto_protocol == PROTOCOL_MODBUS_ASCII )
            modbus_process_ascii_pkt();
#endif
#if HAVE_DCON
        if( uart_auto_protocol == PROTOCOL_DCON )
            dcon_process_pkt( modbus_rx_buf, rx_pos );
#endif
    }

    rx_pos = 0;
    rx_corrupt = 0;
    process_rx_packet_flag = 0;

}


#if HAVE_MODBUS_ASCII
void modbus_process_ascii_pkt(void)
{
    uint8_t nRecv = modbus_ascii_decode( modbus_rx_buf, rx_pos );

    if(!is_for_us())
        return;

    if(!check_modbus_ascii_lrc( modbus_rx_buf, nRecv ))
    {
        modbus_crc_cnt++;
        return;
    }

    modbus_decode_pkt(modbus_rx_buf[1], modbus_rx_buf+2, nRecv-4);
}
#endif

#if HAVE_MODBUS_RTU
void modbus_process_rtu_pkt(void)
{
    int temp;

    // Wrong addr?
    if(!is_for_us())
        return;

    //flash_led_once();
    int nRecv = rx_pos;

    int CRC16 = modbus_CRC16( modbus_rx_buf, nRecv-2 );

    temp = (int) ( modbus_rx_buf[nRecv-1] << 8 ) + modbus_rx_buf[nRecv-2];

    if( temp != CRC16 )
    {
        modbus_crc_cnt++;
        return;
    }
//flash_led2_once();
    //int need_reply =
    modbus_decode_pkt(modbus_rx_buf[1], modbus_rx_buf+2, nRecv-4);

    // TODO no reply if broadcast?
    //if(need_reply)
}
#endif


void modbus_decode_pkt( unsigned char type, unsigned char *rx, int rx_size )
{
    switch(type)
    {
    case 3: // read hold regs
    case 4: // read in regs
        {
            int startReg = GET_WORD(0,rx);
            int nReg = GET_WORD(2,rx);

            int nReplyBytes = modbus_read_regs( modbus_tx_buf+3, startReg, nReg );
            if( nReplyBytes <= 0 )
            {
                modbus_send_error_reply(type, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS); // invalid address
                return;
            }
            else
            {
                modbus_tx_buf[1] = type;
                modbus_tx_buf[2] = nReplyBytes;
                tx_len = nReplyBytes + 3;
            }
        }
        break;

    case 6: // write one reg
        {
            int startReg = GET_WORD(0,rx);
            int value = GET_WORD(2,rx);

            //int err = modbus_write_regs( rx+4, startReg, 1 );
            int err = modbus_write_register( startReg, value );

            if(err)
            {
                modbus_send_error_reply(type, err);
                return;
            }
            else
            {
                modbus_copy_rx_tx(6);
                tx_len = 6;
            }
        }
        break;


    case 7: // read exception status
        {
            // no param
            modbus_tx_buf[1] = type;
            modbus_tx_buf[2] = error_flags;
            tx_len = 3;
        }
        break;


    case 0x0B: // read event cnt
        {
            // no param
            modbus_tx_buf[1] = type;

            PUT_WORD(2,0,modbus_tx_buf); // status = 0
            PUT_WORD(4,modbus_event_cnt,modbus_tx_buf); //


            tx_len = 6;
        }
        break;

    case 0x10: // write multiple regs
        {
            int startReg = GET_WORD(0,rx);
            int nReg = GET_WORD(2,rx);
            uint8_t byteCnt = rx[4];
            if( byteCnt*2 != nReg )
            {
                modbus_send_error_reply(type, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS);
                return;
            }

            int err = modbus_write_regs( rx+5, startReg, nReg );
            if(err)
            {
                modbus_send_error_reply(type, err);
                return;
            }

            modbus_copy_rx_tx(6);
            tx_len = 6;

        }
        break;

    case 0x11: // read slave id
        {
            // no param
            modbus_tx_buf[1] = type;
            modbus_tx_buf[2] = 2; // byte cnt

#if OW_SERIAL_ID
            modbus_tx_buf[3] = serialNumber[1]+serialNumber[2]+serialNumber[3]+serialNumber[4]+serialNumber[5]+serialNumber[6]+; // 1Wire ID DS2401
#else
            modbus_tx_buf[3] = 0; // No ID
#endif
            modbus_tx_buf[4] = 0xFF; // Run

            tx_len = 5;
        }
        break;

#if 1
    case 0x17: // read/write multiple
        {
            int startRegRead = GET_WORD(0,rx);
            int nRegRead = GET_WORD(2,rx);
            int startRegWrite = GET_WORD(4,rx);
            int nRegWrite = GET_WORD(6,rx);
            uint8_t byteCnt = rx[8];

            if( byteCnt*2 != nRegWrite )
            {
                modbus_send_error_reply(type, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS);
                return;
            }

            int err = modbus_write_regs( rx+9, startRegWrite, nRegWrite );
            if(err)
            {
                modbus_send_error_reply(type, err);
                return;
            }

            int nReplyBytes = modbus_read_regs( modbus_tx_buf+3, startRegRead, nRegRead );
            if( nReplyBytes <= 0 )
            {
                modbus_send_error_reply(type, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS);
                return;
            }


            modbus_tx_buf[1] = type;
            modbus_tx_buf[2] = nReplyBytes;
            tx_len = nReplyBytes + 3;

        }
        break;
#endif


    case 8: // diagnostics
        {
            int subFunc = GET_WORD(0,rx);
            //int value = GET_WORD(2,rx);

            if( subFunc != 0 )
            {
                modbus_send_error_reply(type, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
                return;
            }

            modbus_copy_rx_tx(6);
            tx_len = 6;
        }
        break;


    default:
        modbus_send_error_reply(type, MODBUS_EXCEPTION_ILLEGAL_FUNCTION); // invalid cmd
        return;
    }


    modbus_send_packet();


    modbus_event_cnt++;
}

void modbus_copy_rx_tx(int nb )
{
    unsigned char *rp = modbus_rx_buf;
    unsigned char *tp = modbus_tx_buf;

    while(nb--)
        *tp++ = *rp++;
}


void modbus_send_error_reply(int cmd, int nerr)
{
    //modbus_tx_buf[0] = modbus_our_address;
    modbus_tx_buf[1] = cmd+0x80;;//команда с ошибкой
    modbus_tx_buf[2] = nerr;

    tx_len = 3;

    modbus_send_packet();
    modbus_exceptions_cnt++;
}


void modbus_send_packet( void )
{
    modbus_tx_buf[0] = modbus_our_address;

#if HAVE_MODBUS_RTU
    if( uart_auto_protocol == PROTOCOL_MODBUS_RTU )
        modbus_make_tx_CRC();
#endif
#if HAVE_MODBUS_ASCII
    if( uart_auto_protocol == PROTOCOL_MODBUS_ASCII )
        tx_len = modbus_ascii_encode( modbus_tx_buf, tx_len );
#endif
    modbus_start_tx();
}


#if HAVE_MODBUS_RTU
void modbus_make_tx_CRC(void)
{
    int CRC16 = modbus_CRC16( modbus_tx_buf, tx_len );

    uint8_t crc_hi = CRC16 >> 8;
    uint8_t crc_lo = CRC16 & 0xFF;

    modbus_tx_buf[tx_len++] = crc_lo;
    modbus_tx_buf[tx_len++] = crc_hi;
}
#endif


