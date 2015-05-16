#ifndef _DEV_AX88796_H_
#define _DEV_AX88796_H_

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>


/*!
 * brief Network interface controller information structure and type.
 */
typedef struct {
    HANDLE volatile ni_rx_rdy;      /*!< Receiver event queue. */
    HANDLE volatile ni_tx_rdy;      /*!< Transmitter event queue. */
    uint16_t ni_tx_cnt;              /*!< Number of bytes in transmission queue. */
    uint32_t ni_rx_packets;           /*!< Number of packets received. */
    uint32_t ni_tx_packets;           /*!< Number of packets sent. */
    uint32_t ni_interrupts;           /*!< Number of interrupts. */
    uint32_t ni_overruns;             /*!< Number of packet overruns. */
    uint32_t ni_rx_frame_errors;      /*!< Number of frame errors. */
    uint32_t ni_rx_crc_errors;        /*!< Number of CRC errors. */
    uint32_t ni_rx_missed_errors;     /*!< Number of missed packets. */
    uint8_t volatile ni_tx_bsy;      /*!< NIC transmitter busy flags. */
    uint32_t ni_rx_pending;           /*!< Number of pending receiver interrupts. */
    uint8_t ni_curr_page;            /*!< Current receive page. */
}NICINFO;

/*
 * Available drivers.
 */
extern NUTDEVICE devAx88796;

#ifndef DEV_ETHER
#define DEV_ETHER   devAx88796
#endif

#ifndef devEth0
#define devEth0   devAx88796
#endif


/*! 
 * \brief Charon III Ethernet controller parameters.
 */
#define ASIX_BASE        0x20000000

/* ! Read and write byte from controller */

#define Asix_Read(reg) *(unsigned char *) (reg + ASIX_BASE)
#define Asix_ReadWord(reg) *(unsigned short *) (reg + ASIX_BASE)
#define Asix_Write(reg, data) *(unsigned char *) (reg + ASIX_BASE) = data
#define Asix_WriteWord(reg, data) *(unsigned short *) (reg + ASIX_BASE) = data

// buffer boundaries - transmit has 6 256-byte pages = 1536
//   receive has 52 256-byte pages = 14848
//   entire available packet buffer space is allocated
#define TXSTART_INIT   	0x40
#define RXSTART_INIT   	0x46
#define RXSTOP_INIT    	0x80

extern int AsixInit(NUTDEVICE *dev);
extern int AsixOutput(NUTDEVICE *dev, NETBUF *nb);
void NicPhyWrite(uint8_t reg, uint16_t val);
uint16_t NicPhyRead(uint8_t reg);

#endif

