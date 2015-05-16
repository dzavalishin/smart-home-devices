/*****************************************************************************
 * XMODEM BOOTLOADER FOR ARM USING CODE SOURCERY
*****************************************************************************/

#include "board.h"   	/* Board Support Package */        
#include "AT91SAM7S.h"
#include "lib_AT91SAM7S.h"

//{{{  Definition

#define RAMFUNC __attribute__ ((long_call, section (".ramfunc")))
#define AT91C_MC_FSR_MVM 		((unsigned int) 0xFF << 8)		// (MC) Status Register GPNVMx: General-purpose NVM Bit Status
#define AT91C_MC_FSR_LOCK 		((unsigned int) 0xFFFF << 16)	// (MC) Status Register LOCKSx: Lock Region x Lock Status
#define	ERASE_VALUE 		0xFFFFFFFF

typedef void (*funct)(void);

#define UART_RXBUFSIZE 1500

// X/YMODEM header def
#define CRC_TABLE_SIZE   (256)
#define CRC16_POLYNOMIAL (0x1021)
#define PACKET_HEADER		(3)	/* start, block, block-complement */
#define PACKET_TRAILER_CRC	(2)	/* CRC bytes */
#define PACKET_OVERHEAD_CRC (PACKET_HEADER + PACKET_TRAILER_CRC)
#define PACKET_SIZE     	(128)
#define PACKET_1K_SIZE  	(1024)

// X/YMODEM header byte
#define SOH (0x01)	/* start of 128-byte data packet */
#define STX (0x02)	/* start of 1024-byte data packet */
#define EOT (0x04)	/* end of transmission */
#define ACK (0x06)	/* receive OK */
#define NAK (0x15)	/* receiver error; retry */
#define CAN (0x18)	/* two of these in succession aborts transfer */
#define CRC (0x43)	/* use in place of first NAK for CRC mode */
#define GRC (0x47)	/* use in place of first NAK for -g *///}}}


//{{{  poll_UART

static unsigned char uart_rxbuf[UART_RXBUFSIZE];  
static unsigned short rx_index=0,tx_index=0;

__attribute__ ((section (".text.fastcode"))) void poll_UART(void) {
    if(((AT91PS_USART)BOOTLOADER_UART_BASE)->US_CSR & AT91C_US_RXRDY) {
    	uart_rxbuf[tx_index++] = ((AT91PS_USART)BOOTLOADER_UART_BASE)->US_RHR & 0x1FF;
		if(tx_index==UART_RXBUFSIZE) tx_index=0;
	}
}
//}}}

//{{{  getc_UART

unsigned char getc_UART() {  
	unsigned char c;

	while(rx_index==tx_index) poll_UART();
   	c = uart_rxbuf[rx_index++];
	if(rx_index==UART_RXBUFSIZE) rx_index=0;

	return c;
}
//}}}

//{{{  kbhit_UART

int kbhit_UART() {
	poll_UART();
    return (rx_index!=tx_index);
}

//}}}

//{{{  putc_UART

void putc_UART(unsigned char c) {
	while (!AT91F_US_TxReady((AT91PS_USART)BOOTLOADER_UART_BASE));
	AT91F_US_PutChar((AT91PS_USART)BOOTLOADER_UART_BASE, c);
}
//}}}

//{{{  puts_UART

void puts_UART(const char *c) {
    while((*c)!=0) { putc_UART(*c); c++; }
}
//}}}




//{{{  AT91F_Flash_Ready


//----------------------------------------------------------------------------
// \fn    AT91F_Flash_Ready
// \brief Wait the flash ready
//----------------------------------------------------------------------------
__attribute__ ((section (".text.fastcode"))) int AT91F_Flash_Ready (void) {
    unsigned int status = 0;

    // Wait the end of command
    while ((status & AT91C_MC_FRDY) != AT91C_MC_FRDY) {
		status = AT91C_BASE_MC->MC_FSR;
		poll_UART();
	}

    return status;
}
//}}}



//{{{  AT91F_Flash_Write


//----------------------------------------------------------------------------
// \fn    AT91F_Flash_Write
// \brief Write in one Flash page located in AT91C_IFLASH,  size in 32 bits
// \input Flash_Address: start at 0x0010 0000 size: in byte
//----------------------------------------------------------------------------
__attribute__ ((section (".text.fastcode"))) int AT91F_Flash_Write(unsigned int page,unsigned int *buff) {

	if(page>=AT91C_IFLASH_NB_OF_PAGES) return false;

    // set the Flash controller base address
    AT91PS_MC ptMC = AT91C_BASE_MC;
    unsigned int i, status;
    unsigned int * Flash;

    // init flash pointer
    Flash = (unsigned int *)AT91C_IFLASH + (page * AT91C_IFLASH_PAGE_SIZE);

    // copy the new value
	for (i=0; i<AT91C_IFLASH_PAGE_SIZE; i+=4, Flash++, buff++) { *Flash=*buff; poll_UART(); }
	
    // Write the write page command
    ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_START_PROG | (AT91C_MC_PAGEN & (page <<8)) ;

    // Wait the end of command
    status = AT91F_Flash_Ready();

    // Check the result
    if ( (status & ( AT91C_MC_PROGE | AT91C_MC_LOCKE ))!=0) return false;

  	return true;
}
//}}}



//{{{  crc16_init

static unsigned short crc16_table[CRC_TABLE_SIZE * sizeof(unsigned short)];

/* Generate the table of constants used in executing the CRC32 algorithm: */
void crc16_init(void) {
  	int i, j;
  	unsigned short crc;

   	for(i = 0; i < CRC_TABLE_SIZE; ++i) {
   		crc = i << 8;
		for(j = 8; j > 0; --j) {
			if(crc & 0x8000)
				crc = (crc << 1) ^ CRC16_POLYNOMIAL;
			else
				crc <<= 1;
       		}
    	crc16_table[i] = crc;
	}
}
//}}}

//{{{  crc_16_buf

/* Perform a CRC16 computation over `buf'. This method was derived from
 * an algorithm (C) 1986 by Gary S. Brown, and was checked against an
 * implementation (C) 2000 by Compaq Computer Corporation, authored by
 * George France.
 */
unsigned short crc16_buf(unsigned char *buf, unsigned int length) {
  	unsigned short crc = 0;

	while(length-- > 0) crc = crc16_table[(crc >> 8) & 0xff] ^ (crc << 8) ^ *buf++;
 	return crc;
}
//}}}


//{{{  wait - delay loop

//--------------------------------------------------------------------------------------
// Function Name       : wait
// Object              : Software waiting loop
// Input Parameters    : none. Waiting time is defined by the global variable LedSpeed.
// Output Parameters   : none
//--------------------------------------------------------------------------------------
void wait(unsigned long time) {
    unsigned int waiting_time;

    for(waiting_time = 0; waiting_time < time; waiting_time++) if (kbhit_UART()) return;
}
//}}}

//{{{  error

void error(unsigned char code) {
	while(1) {
		putc_UART(CAN);
    	switch(code) { 
			case 0: puts_UART("Error writing last page\r\n"); break;
			case 1: puts_UART("CAN received\r\n"); break;
			case 2: puts_UART("Error writting flash page\r\n"); break;
		}
	    while(kbhit_UART()) getc_UART();
		wait(500000*3);
	}
}
//}}}

//{{{  main

static unsigned char xmodem_buf[PACKET_1K_SIZE+PACKET_OVERHEAD_CRC];
static unsigned int page_buffer[AT91C_IFLASH_PAGE_SIZE/4];

int main (void) {
    int i,p;
    unsigned char c=0,ymodem;
    unsigned int page_index,page;
	int index,pkt_count,waitloop;		// Buffer pointer
	unsigned char xmodem_pktid;         // Next packet ID to receive (begin with 1)
    AT91PS_PMC pPMC;
	
	/* BOARD INIT (SWITCH CLOCK TO 48 MHZ) */
	//{{{  Start main clock oscillator at 18.432 MHz

    /* Enable the Main Oscillator:
    * set OSCOUNT to 6, which gives Start up time = 8 * 6 / SCK = 1.4ms
    * (SCK = 32768Hz)
    */
    pPMC = AT91C_BASE_PMC;
    pPMC->PMC_MOR = ((6 << 8) & AT91C_CKGR_OSCOUNT) | AT91C_CKGR_MOSCEN;
    while ((pPMC->PMC_SR & AT91C_PMC_MOSCS) == 0); /* Wait the startup time */

	//}}}

	//{{{  Select 1 wait state for flash

    /* Set flash wait sate FWS and FMCN at 72(1.5uS) for 48Mhz operation */
    AT91C_BASE_MC->MC_FMR = (72 << 16) | AT91C_MC_FWS_1FWS;
//}}}
	
	//{{{  Configure PLL for 48Mhz operation

    /* Set the PLL and Divider:
    * - div by 5 Fin = 3,6864 =(18,432 / 5)
    * - Mul 25+1: Fout = 95,8464 =(3,6864 *26)
    * for 96 MHz the error is 0.16%
    * Field out NOT USED = 0
    * PLLCOUNT pll startup time estimate at : 0.844 ms
    * PLLCOUNT 28 = 0.000844 /(1/32768)
    */
    pPMC->PMC_PLLR = ((AT91C_CKGR_DIV & 0x05)
                      | (AT91C_CKGR_PLLCOUNT & (28 << 8))
                      | (AT91C_CKGR_MUL & (25 << 16)));
    while ((pPMC->PMC_SR & AT91C_PMC_LOCK) == 0);   

/* Wait the startup time */
    while ((pPMC->PMC_SR & AT91C_PMC_MCKRDY) == 0); 
//}}}

	//{{{  Switch ARM core clock and master clock to PLL


    /* Select Master Clock and CPU Clock select the PLL clock / 2 */
    pPMC->PMC_MCKR =  AT91C_PMC_PRES_CLK_2;  while ((pPMC->PMC_SR & AT91C_PMC_MCKRDY) == 0);
    pPMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK; while ((pPMC->PMC_SR & AT91C_PMC_MCKRDY) == 0);

    //}}}


    /* INITIALIZE SERIAL PORT */
	//{{{  Configure UART


#if BOOTLOADER_UART == 0
	AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC,((unsigned int) 1 << AT91C_ID_US0));
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, ((unsigned int) AT91C_PA5_RXD0)|((unsigned int) AT91C_PA6_TXD0), 0);
#endif
#if BOOTLOADER_UART == 1
	AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC,((unsigned int) 1 << AT91C_ID_US1));
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,((unsigned int) AT91C_PA22_TXD1)|((unsigned int) AT91C_PA21_RXD1),0);
#endif
#if BOOTLOADER_UART == 2
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,((unsigned int) AT91C_PA9_DRXD)|((unsigned int) AT91C_PA10_DTXD), 0); 
#endif

    // Configure UART
    //-------------------
	AT91F_US_Configure (
		(AT91PS_USART) BOOTLOADER_UART_BASE,       // UART base address
		BOARD_MCK,
		AT91C_US_ASYNC_MODE ,                 // Mode Register to be programmed
		BOOTLOADER_BAUDRATE,                  // Baudrate to be programmed
		0);                                   // Timeguard to be programmed

    // Enable Transmitter & receiver
    //-----------------------------------
    ((AT91PS_USART)BOOTLOADER_UART_BASE)->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;    
       //}}}

	
	/* Check for bootloader key */
	wait(200000);
    puts_UART("\r\nXLoader 7.0"); 
	wait(500000*3);
	if (kbhit_UART()) c = getc_UART();
	if(c != '\r') { 
		puts_UART(" - run"); 
     	goto StartCode;
	} else {
		puts_UART(" - x/ymodem 1k/CRC\r\n");
    }	

    /* Init xmodem variable */
    index = 0;
    xmodem_pktid = 1;
	pkt_count = 0; 			// Only use is to detect ymodem header packet ID=0  
	ymodem = 0;				// Set to 1 if detect YMODEM transfer
    page = (CODE_START-(unsigned int)AT91C_IFLASH) / AT91C_IFLASH_PAGE_SIZE;
    page_index = 0;
	crc16_init();

	/* HANDSHACKING FOR XMODEM CRC/1K OR YMODEM */
    while(1) {   

        // Search for SOH or STX start flag
	    if (kbhit_UART()) {
		    c = getc_UART();
		    if(c==SOH || c==STX) break;
	    }

        // Send CRC 
        putc_UART(CRC);

        // Wait loop
        wait(240000*3);
        AT91F_WDTRestart(AT91C_BASE_WDTC);
    }

	/* STORE FIRST PACKET TYPE */
    xmodem_buf[index++] = c;

	/* MAIN LOOP X/YMODEM PROTOCOL */
    while(1) {

		/* GET CARACTER FROM SERIAL PORT */
        AT91F_WDTRestart(AT91C_BASE_WDTC);
		waitloop=0;
	    while(kbhit_UART()==0) { 
			waitloop++;
			if(waitloop==500000) { waitloop=0; putc_UART(NAK); }   // Timeout
		}
		c = getc_UART();
        
		/* FIRST CARACTER: CHECK PACKET TYPE */
        if(index==0) {   		 
            if(c==EOT) {                                    // End of transfer
				
				/* DO NOT FLUSH FLASH BUFFER ON 2ND YMODEM SEQUENCE */
                if(ymodem<2 && page_index!=0) {           
                    if(AT91F_Flash_Write(page, page_buffer)==0) error(0);
                }   

                if(ymodem) {
					if(ymodem==1) { 
						putc_UART(NAK);   	// 1st YMODEM end sequence 
						ymodem++; 
				     } else {
						 putc_UART(ACK);    // 2nd YMODEM end sequence (reset receiver for next batch)
						 putc_UART(CRC); 
   						 xmodem_pktid = 1;
					 	 pkt_count = 0; 
					 }
				} else { 
					putc_UART(ACK);         // XMODEM end sequence
					break;
                }
            }
            if(c==CAN) error(1);	        // Cancel send
            if(c==SOH || c==STX) xmodem_buf[index++] = c;   // Start new data packet

		/* RECEIVE PACKET, CHECK FOR END OF BLOCK */
        } else {
            xmodem_buf[index++] = c;
            if((xmodem_buf[0]==SOH && index==(PACKET_SIZE+PACKET_OVERHEAD_CRC)) || 
               (xmodem_buf[0]==STX && index==(PACKET_1K_SIZE+PACKET_OVERHEAD_CRC)) ) {

				// Detect YMODEM transfer, end batch if filename is NULL
				if(xmodem_buf[1]==0 && pkt_count==0) { 
					ymodem=1; 
					index=0; 
					putc_UART(ACK); 
					if(xmodem_buf[3]!=0) {
						putc_UART(CRC);				// Start new YMODEM transfer 
						continue; 
					} else {
						break;                      // Pathname is NULL, end batch transfer
					}
				}
				pkt_count++;

                // Verify CRC and packet ID
                if(xmodem_buf[1]!=xmodem_pktid) { putc_UART(NAK); continue; }
   				if(crc16_buf(xmodem_buf + PACKET_HEADER, (xmodem_buf[0]==SOH?PACKET_SIZE:PACKET_1K_SIZE) + PACKET_TRAILER_CRC) != 0) { putc_UART(NAK); continue; }

				// ack packet before write to FLASH (UART buffer fill when wait for flash)
                putc_UART(ACK);

                // Transfer packet in flash page buffer
                p = 3; 
                i=(xmodem_buf[0]==SOH?PACKET_SIZE:PACKET_1K_SIZE);
				while(i--) {
                    poll_UART();
					((unsigned char*)page_buffer)[page_index++] = xmodem_buf[p++];

	                // Write flash if page is full
	                if(page_index==AT91C_IFLASH_PAGE_SIZE) {
	   	                if(AT91F_Flash_Write(page,page_buffer)==0) error(2);
        	            page_index=0;
            	        page++;  
                	}
                }

                xmodem_pktid++;
                index=0;
            }
        }
    }
     
	 puts_UART("complete\r\n  ");

StartCode:
    AT91F_PMC_DisablePeriphClock (AT91C_BASE_PMC, 0xFFFFFFFF );     // All peripheral clocks de-activated.
    AT91F_PMC_CfgMCKReg (AT91C_BASE_PMC,AT91C_PMC_CSS_SLOW_CLK | AT91C_PMC_PRES_CLK); // Slow Clock
    while(!(AT91F_PMC_GetStatus(AT91C_BASE_PMC) & AT91C_PMC_MCKRDY));
    AT91F_CKGR_CfgPLLReg(AT91C_BASE_CKGR,0);                        // Shutdown PLL
    AT91F_CKGR_DisableMainOscillator(AT91C_BASE_CKGR);              // Disable Main Oscillator
    ((funct)(CODE_START-(unsigned int)AT91C_IFLASH))();
	return 0;
}
//}}}

