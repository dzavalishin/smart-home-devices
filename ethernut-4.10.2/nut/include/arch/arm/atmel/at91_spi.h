#ifndef _ARCH_ARM_AT91_SPI_H_
#define _ARCH_ARM_AT91_SPI_H_

/*
 * Copyright (C)
 */

/*!
 * \file arch/arm/at91_spi.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2006/09/29 12:30:31  haraldkipp
 * Register offsets added.
 *
 * Revision 1.4  2006/08/31 19:11:46  haraldkipp
 * Bits per transfer definitions added.
 *
 * Revision 1.3  2006/08/05 11:54:45  haraldkipp
 * PDC registers added.
 *
 * Revision 1.2  2006/07/26 11:22:31  haraldkipp
 * Added missing bit definitions.
 *
 * Revision 1.1  2006/07/21 09:03:56  haraldkipp
 * Added SPI support, kindly contributed by Andras Albert.
 *
 * Revision 1.1
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Spi
 */
/*@{*/

/*! \name SPI Control Register */
/*@{*/
#define	SPI_CR_OFF	    0x00000000  /*!< \brief Control register offset. */

#define SPI_SPIEN       0x00000001  /*!< \brief SPI enable. */
#define SPI_SPIDIS      0x00000002  /*!< \brief SPI disable. */
#define SPI_SWRST       0x00000080  /*!< \brief Software reset. */
#define SPI_LASTXFER    0x01000000  /*!< \brief Last transfer. */
/*@}*/

/*! \name SPI Mode Register */
/*@{*/
#define SPI_MR_OFF		0x00000004  /*!< \brief Mode register offset. */

#define SPI_MSTR        0x00000001  /*!< \brief Master mode. */
#define SPI_PS          0x00000002  /*!< \brief Peripheral select. */
#define SPI_PCSDEC      0x00000004  /*!< \brief Chip select decode. */
#define SPI_FDIV        0x00000008  /*!< \brief Clock selection. */
#define SPI_MODFDIS     0x00000010  /*!< \brief Mode fault detection. */
#define SPI_LLB         0x00000080  /*!< \brief Local loopback enable. */
#define SPI_PCS         0x000F0000  /*!< \brief Peripheral chip select mask. */
#define SPI_PCS_0       0x000E0000  /*!< \brief Peripheral chip select 0. */
#define SPI_PCS_1       0x000D0000  /*!< \brief Peripheral chip select 1. */
#define SPI_PCS_2       0x000B0000  /*!< \brief Peripheral chip select 2. */
#define SPI_PCS_3       0x00070000  /*!< \brief Peripheral chip select 3. */
#define SPI_PCS_LSB             16  /*!< \brief Least significant bit of peripheral chip select. */
#define SPI_DLYBCS      0xFF000000  /*!< \brief Mask for delay between chip selects. */
#define SPI_DLYBCS_LSB          24  /*!< \brief Least significant bit of delay between chip selects. */
/*@}*/

/*! \name SPI Receive Data Register */
/*@{*/
#define SPI_RDR_OFF		0x00000008  /*!< \brief Receive data register offset. */

#define SPI_RD          0x0000FFFF  /*!< \brief Receive data mask. */
#define SPI_RD_LSB              0   /*!< \brief Least significant bit of receive data. */
/*@}*/

/*! \name SPI Transmit Data Register */
/*@{*/
#define SPI_TDR_OFF		0x0000000C  /*!< \brief Transmit data register offset. */

#define SPI_TD          0x0000FFFF  /*!< \brief Transmit data mask. */
#define SPI_TD_LSB              0   /*!< \brief Least significant bit of transmit data. */
/*@}*/

/*! \name SPI Status and Interrupt Register */
/*@{*/
#define SPI_SR_OFF		0x00000010  /*!< \brief Status register offset. */
#define SPI_IER_OFF		0x00000014  /*!< \brief Interrupt enable register offset. */
#define SPI_IDR_OFF		0x00000018  /*!< \brief Interrupt disable register offset. */
#define SPI_IMR_OFF		0x0000001C  /*!< \brief Interrupt mask register offset. */

#define SPI_RDRF        0x00000001  /*!< \brief Receive data register full. */
#define SPI_TDRE        0x00000002  /*!< \brief Transmit data register empty. */
#define SPI_MODF        0x00000004  /*!< \brief Mode fault error. */
#define SPI_OVRES       0x00000008  /*!< \brief Overrun error status. */
#define SPI_ENDRX       0x00000010  /*!< \brief End of RX buffer. */
#define SPI_ENDTX       0x00000020  /*!< \brief End of TX buffer. */
#define SPI_RXBUFF      0x00000040  /*!< \brief RX buffer full. */
#define SPI_TXBUFE      0x00000080  /*!< \brief TX buffer empty. */
#define SPI_NSSR        0x00000100  /*!< \brief NSS rising. */
#define SPI_TXEMPTY     0x00000200  /*!< \brief Transmission register empty. */
#define SPI_SPIENS      0x00010000  /*!< \brief SPI enable status. */
/*@}*/

/*! \name SPI Chip Select Registers */
/*@{*/
#define SPI_CSR0_OFF	0x00000030  /*!< \brief Chip select register 0 offset. */
#define SPI_CSR1_OFF	0x00000034  /*!< \brief Chip select register 1 offset. */
#define SPI_CSR2_OFF	0x00000038  /*!< \brief Chip select register 2 offset. */
#define SPI_CSR3_OFF    0x0000003C  /*!< \brief Chip select register 3 offset. */

#define SPI_CPOL        0x00000001  /*!< \brief Clock polarity. */
#define SPI_NCPHA       0x00000002  /*!< \brief Clock phase. */
#define SPI_CSAAT       0x00000008  /*!< \brief Chip select active after transfer. */
#define SPI_BITS        0x000000F0  /*!< \brief Bits per transfer mask. */
#define SPI_BITS_8      0x00000000  /*!< \brief 8 bits per transfer. */
#define SPI_BITS_9      0x00000010  /*!< \brief 9 bits per transfer. */
#define SPI_BITS_10     0x00000020  /*!< \brief 10 bits per transfer. */
#define SPI_BITS_11     0x00000030  /*!< \brief 11 bits per transfer. */
#define SPI_BITS_12     0x00000040  /*!< \brief 12 bits per transfer. */
#define SPI_BITS_13     0x00000050  /*!< \brief 13 bits per transfer. */
#define SPI_BITS_14     0x00000060  /*!< \brief 14 bits per transfer. */
#define SPI_BITS_15     0x00000070  /*!< \brief 15 bits per transfer. */
#define SPI_BITS_16     0x00000080  /*!< \brief 16 bits per transfer. */
#define SPI_BITS_LSB            4   /*!< \brief Least significant bit of bits per transfer. */
#define SPI_SCBR        0x0000FF00  /*!< \brief Serial clock baud rate mask. */
#define SPI_SCBR_LSB            8   /*!< \brief Least significant bit of serial clock baud rate. */
#define SPI_DLYBS       0x00FF0000  /*!< \brief Delay before SPCK mask. */
#define SPI_DLYBS_LSB           16  /*!< \brief Least significant bit of delay before SPCK. */
#define SPI_DLYBCT      0xFF000000  /*!< \brief Delay between consecutive transfers mask. */
#define SPI_DLYBCT_LSB          24  /*!< \brief Least significant bit of delay between consecutive transfers. */
/*@}*/

/*! \name Single SPI Register Addresses */
/*@{*/
#if defined(SPI_BASE)
#define SPI0_BASE   SPI_BASE
#define	SPI_CR		SPI0_CR     /*!< \brief SPI Control Register Write-only. */
#define SPI_MR		SPI0_MR     /*!< \brief SPI Mode Register Read/Write Reset=0x0. */
#define SPI_RDR		SPI0_RDR    /*!< \brief SPI Receive Data Register Read-only Reset=0x0. */
#define SPI_TDR		SPI0_TDR    /*!< \brief SPI Transmit Data Register Write-only . */
#define SPI_SR		SPI0_SR     /*!< \brief SPI Status Register Read-only Reset=0x000000F0. */
#define SPI_IER		SPI0_IER    /*!< \brief SPI Interrupt Enable Register Write-only. */
#define SPI_IDR		SPI0_IDR    /*!< \brief SPI Interrupt Disable Register Write-only. */
#define SPI_IMR		SPI0_IMR    /*!< \brief SPI Interrupt Mask Register Read-only Reset=0x0. */
#define SPI_CSR0	SPI0_CSR0   /*!< \brief SPI Chip Select Register 0 Read/Write Reset=0x0. */
#define SPI_CSR1	SPI0_CSR1   /*!< \brief SPI Chip Select Register 1 Read/Write Reset=0x0. */
#define SPI_CSR2	SPI0_CSR2   /*!< \brief SPI Chip Select Register 2 Read/Write Reset=0x0. */
#define SPI_CSR3	SPI0_CSR3   /*!< \brief SPI Chip Select Register 3 Read/Write Reset=0x0. */
#if defined(SPI_HAS_PDC)
#define SPI_RPR     SPI0_RPR    /*!< \brief PDC channel 0 receive pointer register. */
#define SPI_RCR     SPI0_RCR    /*!< \brief PDC channel 0 receive counter register. */
#define SPI_TPR     SPI0_TPR    /*!< \brief PDC channel 0 transmit pointer register. */
#define SPI_TCR     SPI0_TCR    /*!< \brief PDC channel 0 transmit counter register. */
#define SPI_RNPR    SPI0_RNPR   /*!< \brief PDC channel 0 receive next pointer register. */
#define SPI_RNCR    SPI0_RNCR   /*!< \brief PDC channel 0 receive next counter register. */
#define SPI_TNPR    SPI0_TNPR   /*!< \brief PDC channel 0 transmit next pointer register. */
#define SPI_TNCR    SPI0_TNCR   /*!< \brief PDC channel 0 transmit next counter register. */
#define SPI_PTCR    SPI0_PTCR   /*!< \brief PDC channel 0 transfer control register. */
#define SPI_PTSR    SPI0_PTSR   /*!< \brief PDC channel 0 transfer status register. */
#endif /* SPI_HAS_PDC */
#endif /* SPI_BASE */
/*@}*/

/*! \name SPI 0 Register Addresses */
/*@{*/
#if defined(SPI0_BASE)
#define	SPI0_CR		(SPI0_BASE + SPI_CR_OFF)        /*!< \brief SPI Control Register Write-only. */
#define SPI0_MR		(SPI0_BASE + SPI_MR_OFF)        /*!< \brief SPI Mode Register Read/Write Reset=0x0. */
#define SPI0_RDR	(SPI0_BASE + SPI_RDR_OFF)       /*!< \brief SPI Receive Data Register Read-only Reset=0x0. */
#define SPI0_TDR	(SPI0_BASE + SPI_TDR_OFF)       /*!< \brief SPI Transmit Data Register Write-only . */
#define SPI0_SR		(SPI0_BASE + SPI_SR_OFF)        /*!< \brief SPI Status Register Read-only Reset=0x000000F0. */
#define SPI0_IER	(SPI0_BASE + SPI_IER_OFF)       /*!< \brief SPI Interrupt Enable Register Write-only. */
#define SPI0_IDR	(SPI0_BASE + SPI_IDR_OFF)       /*!< \brief SPI Interrupt Disable Register Write-only. */
#define SPI0_IMR	(SPI0_BASE + SPI_IMR_OFF)       /*!< \brief SPI Interrupt Mask Register Read-only Reset=0x0. */
#define SPI0_CSR0	(SPI0_BASE + SPI_CSR0_OFF)      /*!< \brief SPI Chip Select Register 0 Read/Write Reset=0x0. */
#define SPI0_CSR1	(SPI0_BASE + SPI_CSR1_OFF)      /*!< \brief SPI Chip Select Register 1 Read/Write Reset=0x0. */
#define SPI0_CSR2	(SPI0_BASE + SPI_CSR2_OFF)      /*!< \brief SPI Chip Select Register 2 Read/Write Reset=0x0. */
#define SPI0_CSR3	(SPI0_BASE + SPI_CSR3_OFF)      /*!< \brief SPI Chip Select Register 3 Read/Write Reset=0x0. */
#if defined(SPI_HAS_PDC)
#define SPI0_RPR    (SPI0_BASE + PERIPH_RPR_OFF)    /*!< \brief PDC channel 0 receive pointer register. */
#define SPI0_RCR    (SPI0_BASE + PERIPH_RCR_OFF)    /*!< \brief PDC channel 0 receive counter register. */
#define SPI0_TPR    (SPI0_BASE + PERIPH_TPR_OFF)    /*!< \brief PDC channel 0 transmit pointer register. */
#define SPI0_TCR    (SPI0_BASE + PERIPH_TCR_OFF)    /*!< \brief PDC channel 0 transmit counter register. */
#define SPI0_RNPR   (SPI0_BASE + PERIPH_RNPR_OFF)   /*!< \brief PDC channel 0 receive next pointer register. */
#define SPI0_RNCR   (SPI0_BASE + PERIPH_RNCR_OFF)   /*!< \brief PDC channel 0 receive next counter register. */
#define SPI0_TNPR   (SPI0_BASE + PERIPH_TNPR_OFF)   /*!< \brief PDC channel 0 transmit next pointer register. */
#define SPI0_TNCR   (SPI0_BASE + PERIPH_TNCR_OFF)   /*!< \brief PDC channel 0 transmit next counter register. */
#define SPI0_PTCR   (SPI0_BASE + PERIPH_PTCR_OFF)   /*!< \brief PDC channel 0 transfer control register. */
#define SPI0_PTSR   (SPI0_BASE + PERIPH_PTSR_OFF)   /*!< \brief PDC channel 0 transfer status register. */
#endif /* SPI_HAS_PDC */
#endif /* SPI0_BASE */
/*@}*/

/*! \name SPI 1 Register Addresses */
/*@{*/
#if defined(SPI1_BASE)
#define	SPI1_CR		(SPI1_BASE + SPI_CR_OFF)        /*!< \brief SPI Control Register Write-only. */
#define SPI1_MR		(SPI1_BASE + SPI_MR_OFF)        /*!< \brief SPI Mode Register Read/Write Reset=0x0. */
#define SPI1_RDR	(SPI1_BASE + SPI_RDR_OFF)       /*!< \brief SPI Receive Data Register Read-only Reset=0x0. */
#define SPI1_TDR	(SPI1_BASE + SPI_TDR_OFF)       /*!< \brief SPI Transmit Data Register Write-only . */
#define SPI1_SR		(SPI1_BASE + SPI_SR_OFF)        /*!< \brief SPI Status Register Read-only Reset=0x000000F0. */
#define SPI1_IER	(SPI1_BASE + SPI_IER_OFF)       /*!< \brief SPI Interrupt Enable Register Write-only. */
#define SPI1_IDR	(SPI1_BASE + SPI_IDR_OFF)       /*!< \brief SPI Interrupt Disable Register Write-only. */
#define SPI1_IMR	(SPI1_BASE + SPI_IMR_OFF)       /*!< \brief SPI Interrupt Mask Register Read-only Reset=0x0. */
#define SPI1_CSR0	(SPI1_BASE + SPI_CSR0_OFF)      /*!< \brief SPI Chip Select Register 0 Read/Write Reset=0x0. */
#define SPI1_CSR1	(SPI1_BASE + SPI_CSR1_OFF)      /*!< \brief SPI Chip Select Register 1 Read/Write Reset=0x0. */
#define SPI1_CSR2	(SPI1_BASE + SPI_CSR2_OFF)      /*!< \brief SPI Chip Select Register 2 Read/Write Reset=0x0. */
#define SPI1_CSR3	(SPI1_BASE + SPI_CSR3_OFF)      /*!< \brief SPI Chip Select Register 3 Read/Write Reset=0x0. */
#if defined(SPI_HAS_PDC)
#define SPI1_RPR    (SPI1_BASE + PERIPH_RPR_OFF)    /*!< \brief PDC channel 1 receive pointer register. */
#define SPI1_RCR    (SPI1_BASE + PERIPH_RCR_OFF)    /*!< \brief PDC channel 1 receive counter register. */
#define SPI1_TPR    (SPI1_BASE + PERIPH_TPR_OFF)    /*!< \brief PDC channel 1 transmit pointer register. */
#define SPI1_TCR    (SPI1_BASE + PERIPH_TCR_OFF)    /*!< \brief PDC channel 1 transmit counter register. */
#define SPI1_RNPR   (SPI1_BASE + PERIPH_RNPR_OFF)   /*!< \brief PDC channel 1 receive next pointer register. */
#define SPI1_RNCR   (SPI1_BASE + PERIPH_RNCR_OFF)   /*!< \brief PDC channel 1 receive next counter register. */
#define SPI1_TNPR   (SPI1_BASE + PERIPH_TNPR_OFF)   /*!< \brief PDC channel 1 transmit next pointer register. */
#define SPI1_TNCR   (SPI1_BASE + PERIPH_TNCR_OFF)   /*!< \brief PDC channel 1 transmit next counter register. */
#define SPI1_PTCR   (SPI1_BASE + PERIPH_PTCR_OFF)   /*!< \brief PDC channel 1 transfer control register. */
#define SPI1_PTSR   (SPI1_BASE + PERIPH_PTSR_OFF)   /*!< \brief PDC channel 1 transfer status register. */
#endif /* SPI_HAS_PDC */
#endif /* SPI1_BASE */
/*@}*/

/*@} xgNutArchArmAt91Spi */

#endif  /* _ARCH_ARM_AT91_SPI_H_ */
