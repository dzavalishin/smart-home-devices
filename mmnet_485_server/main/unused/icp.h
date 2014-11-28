// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* - File              : icp.h
* - Compiler          : IAR EWAAVR 3.20
*
* - Support mail      : avr@atmel.com
*
* - Supported devices : All devices with an Input Capture Unit can be used.
*                       The example is written for ATmega64
*
* - AppNote           : AVR135 - Pulse-Width Demodulation
*
* - Description       : Header file for Pulse-Width Demodulation
*
* Originally authored by Bruce McKenny
* $Revision: 1.5 $
* $Date: Wednesday, November 02, 2005 13:20:02 UTC $
*****************************************************************************/

#if	!defined(_AVR135_ICP_H_)
#define _AVR135_ICP_H_	1

#define	ICP_ANALOG		1					/* assume analog data		*/

#define	ICP_BUFSIZE		1					/* queue only 1 element		*/
#if		ICP_ANALOG
#define	ICP_RX_QSIZE	ICP_BUFSIZE			/* same as BUFSIZE			*/
#else	/* ICP_DIGITAL */
#define	ICP_RX_QSIZE	(ICP_BUFSIZE+1)		/* 1 extra for queue management */
#endif

//#define	ICP_SCALE	256U
#define	ICP_SCALE	(0x0800U)

#if		ICP_SCALE <= 256
typedef	unsigned char icp_sample_t;
typedef	unsigned int  icp_total_t;
#else
typedef	unsigned int icp_sample_t;
typedef	unsigned long  icp_total_t;
#endif

void	icp_init(void);
unsigned int icp_get_duty(void);
unsigned int icp_get_freq(void);

// Uset when we are doing temperature measure via 1-wire

void disable_icp( unsigned char d );


#endif	/* AVR135_ICP_H_ */
