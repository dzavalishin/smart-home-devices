#ifndef USARTSC16IS752_H
#define USARTSC16IS752_H


#include <sys/device.h>
#include <dev/uart.h>
#include <dev/usart.h>
#include <dev/irqreg.h>

void Sc16is752UsartEnable(uint8_t dev, uint8_t ch);
void Sc16is752UsartDisable(uint8_t dev, uint8_t ch);
uint32_t Sc16is752UsartGetSpeed(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetSpeed(uint32_t rate, uint8_t dev, uint8_t ch);
uint8_t Sc16is752UsartGetDataBits(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetDataBits(uint8_t bits, uint8_t dev, uint8_t ch);
uint8_t Sc16is752UsartGetParity(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetParity(uint8_t mode, uint8_t dev, uint8_t ch);
uint8_t Sc16is752UsartGetStopBits(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetStopBits(uint8_t bits, uint8_t dev, uint8_t ch);
uint32_t Sc16is752UsartGetStatus(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetStatus(uint32_t flags, uint8_t dev, uint8_t ch);
uint8_t Sc16is752UsartGetClockMode(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetClockMode(uint8_t mode, uint8_t dev, uint8_t ch);
uint32_t Sc16is752UsartGetFlowControl(uint8_t dev, uint8_t ch);
int Sc16is752UsartSetFlowControl(uint32_t flags, uint8_t dev, uint8_t ch);
void Sc16is752UsartTxStart(uint8_t dev, uint8_t ch);
void Sc16is752UsartRxStart(uint8_t dev, uint8_t ch);
int Sc16is752UsartInit(uint8_t dev, uint8_t ch, NUTDEVICE *nutDev, IRQ_HANDLER *irq);
int Sc16is752UsartDeinit(uint8_t dev, uint8_t ch, IRQ_HANDLER *irq);



enum {
    EEFBIT=0x01,	// Enhanced Functions enabled
    TCRBIT=0x02,   	// TCR, TLR regs enables
    DEFSEL=0x00,	// Default Selection
    TCRSEL=0x20,	// Select TCR, TLR regs
    SRSSEL=0x40,	// Select Special Register Set (DLL, DLH)
    ERSSEL=0x80,	// Select enhanced register set (EFR, ...XOFF2)
    REGSEL_MASK=0xf0
};

typedef struct
{
    uint8_t state;
    uint8_t flags;
} regselstate_t;

typedef enum {
    RHR=0,	// Receive Holding Register (RHR)
    THR=0,	// Transmit Holding Register (THR)
    IER=1,	// Interrupt Enable Register (IER)
    IIR=2,	// Interrupt Identification Register (IIR)
    FCR=2,	// FIFO Control Register (FCR)
    LCR=3,	// Line Control Register (LCR)
    MCR=4,	// Modem Control Register (MCR)
    LSR=5,	// Line Status Register (LSR)
    MSR=6,	// Modem Status Register (MSR)
    SPR=7,	// Scratchpad Register (SPR)
    TCR=6	// Transmission Control Register (TCR)
	| TCRSEL,
    TLR=7	// Trigger Level Register (TLR)
	| TCRSEL,
    TXLVL=8,	// Transmit FIFO Level register
    RXLVL=9,	// Receive FIFO Level register
    IODir=10,	// I/O pin Direction register
    IOState=11,	// I/O pins State register
    IOIntEna=12, // I/O Interrupt Enable register
    IOControl=14, // I/O pins Control register
    EFCR=15,	// Extra Features Control Register Extra Features Control Register
    DLL=0	// Divisor Latch LSB (DLL)
	| SRSSEL,
    DLH=1	// Divisor Latch MSB (DLH)
	| SRSSEL,
    EFR=2	// Enhanced Features Register (EFR)
	| ERSSEL,
    XON1=4	// Xon1 word
	| ERSSEL,
    XON2=5	// Xon2 word
	| ERSSEL,
    XOFF1=6	// Xoff1 word
	| ERSSEL,
    XOFF2=7	// Xoff2 word
	| ERSSEL
} Sc16is752Regs_t;


#define DEV_MAX 2
#define CH_MAX  2

#define USART_DEVICE0_I2C_ADDR 0x48
#define USART_DEVICE1_I2C_ADDR 0x49

// Bit 3..6 is register number
// Bit 1..2 is channel number (only 00 and 01 are valid)
#define REGADDR(regsel, ch)  (((regsel)<<3)|(((ch)&1)<<1))

// XTAL Clock Frequency, base for baudrate calculation
#define XTAL 1843200

#define INIT_BAUDRATE 19200

#endif
