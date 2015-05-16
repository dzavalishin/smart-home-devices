#ifndef board_h
#define board_h

//{{{  Define

#define __inline static inline	// Definition for inline funtions

#ifndef NULL
#define NULL '\0'
#endif

#define TRUE	1
#define FALSE	0
#define true	1
#define false	0
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define ushort unsigned short
//}}}


#include <AT91SAM7S.H>			/* AT91SAMT7S definitions */      
#include <lib_AT91SAM7S.H>      /* Inline library of function */
#include <stdint.h>    			/* Exact-width integer types. WG14/N843 C99 Standard */

/* External Oscillator MAINCK [Hz] */
#define MAINCK               18432000

/* Master Clock [Hz] */
#define BOARD_MCK            47923200

/* Configure bootloader 0=US0 or 1=US1 2=DBGU */
#define BOOTLOADER_UART       0
#define BOOTLOADER_BAUDRATE   115200 
#define CODE_START 			  ((unsigned int)AT91C_IFLASH + 2048) // Must be multiple of page (128 or 256, check CPU)

/* interrupt locking policy */                                                
#define ARM_INT_KEY_TYPE         unsigned int
                                                
#if BOOTLOADER_UART == 0
#define BOOTLOADER_UART_BASE AT91C_BASE_US0
#endif
#if BOOTLOADER_UART == 1
#define BOOTLOADER_UART_BASE AT91C_BASE_US1
#endif
#if BOOTLOADER_UART == 2
#define BOOTLOADER_UART_BASE AT91C_BASE_DBGU
#endif

#endif                                                             /* bsp_h */

