#ifndef _DEV_REG_AX88796_H_
#define _DEV_REG_AX88796_H_

/*!
 * \brief Asix Ax88796L register definitions.
 */
/*@{*/

/*
 * Register offset applicable to all register pages.
 */
#define CR 0x00        /*!< \brief Command register */
#define DATAPORT 0x10  /*!< \brief Data Port  */
#define IFGS1 0x12     /*!< \brief Inter-frame Gap Segment 1 */
#define IFGS2 0x13     /*!< \brief Inter-frame Gap Segment 2 */
#define MII_EEP 0x14   /*!< \brief MII/EEPROM Access */
#define TR 0x15        /*!< \brief Test Register */
#define IFG 0x16       /*!< \brief Inter-frame Gap */
#define GPI 0x17       /*!< \brief GPI */
#define GPOC 0x17      /*!< \brief GPOC */
#define SPP1 0x18      /*!< \brief Standard Printer Port 1 */
#define SPP2 0x19      /*!< \brief Standard Printer Port 2 */
#define SPP3 0x1a      /*!< \brief Standard Printer Port 3 */
#define RESET 0x1f     /*!< \brief Reset port */

/*
 * Page 0 register offsets.
 */
#define PG0_PSTART 0x01   /*!< \brief Page start register */
#define PG0_PSTOP 0x02    /*!< \brief Page stop register */
#define PG0_BNRY 0x03     /*!< \brief Boundary pointer */
#define PG0_TSR 0x04      /*!< \brief Transmit status register */
#define PG0_TPSR 0x04     /*!< \brief Transmit page start address */
#define PG0_NCR 0x05      /*!< \brief Number of collisions register */
#define PG0_TBCR0 0x05    /*!< \brief Transmit byte count register 0 */
#define PG0_CPR 0x06      /*!< \brief Current Page Register */
#define PG0_TBCR1 0x06    /*!< \brief Transmit Byte Count Register 1 */
#define PG0_ISR 0x07      /*!< \brief Interrupt status register */
#define PG0_CRDA0 0x08    /*!< \brief Current remote DMA address 0 */
#define PG0_RSAR0 0x08    /*!< \brief Remote start address register 0
                                   Low byte address to read from the buffer. */
#define PG0_CRDA1 0x09    /*!< \brief Current remote DMA address 1 */
#define PG0_RSAR1 0x09    /*!< \brief Remote start address register 1
                                   High byte address to read from the buffer. */
#define PG0_RBCR0 0x0a    /*!< \brief Remote byte count register 0
                                   Low byte of the number of bytes to read
                                   from the buffer. */
#define PG0_RBCR1 0x0b    /*!< \brief Remote byte count register 1
                                   High byte of the number of bytes to read
                                   from the buffer. */
#define PG0_RSR 0x0c      /*!< \brief Receive status register */
#define PG0_RCR 0x0c      /*!< \brief Receive configuration register */
#define PG0_CNTR0 0x0d    /*!< \brief Tally counter 0 (frame alignment errors) */
#define PG0_TCR 0x0d      /*!< \brief Transmit configuration register */
#define PG0_DCR 0x0e      /*!< \brief Data configuration register */
#define PG0_IMR 0x0f      /*!< \brief Interrupt mask register */

/*
 * Page 1 register offsets.
 */
#define PG1_PAR0 0x01      /*!< \brief Physical Address Register 0 */
#define PG1_PAR1 0x02      /*!< \brief Physical Address Register 1 */
#define PG1_PAR2 0x03      /*!< \brief Physical Address Register 2 */
#define PG1_PAR3 0x04      /*!< \brief Physical Address Register 3 */
#define PG1_PAR4 0x05      /*!< \brief Physical Address Register 4 */
#define PG1_PAR5 0x06      /*!< \brief Physical Address Register 5 */
#define PG1_CPR 0x07       /*!< \brief Current Page Register */
#define PG1_MAR0 0x08      /*!< \brief Multicast Address Register 0 */
#define PG1_MAR1 0x09      /*!< \brief Multicast Address Register 1 */
#define PG1_MAR2 0x0a      /*!< \brief Multicast Address Register 2 */
#define PG1_MAR3 0x0b      /*!< \brief Multicast Address Register 3 */
#define PG1_MAR4 0x0c      /*!< \brief Multicast Address Register 4 */
#define PG1_MAR5 0x0d      /*!< \brief Multicast Address Register 5 */
#define PG1_MAR6 0x0e      /*!< \brief Multicast Address Register 6 */
#define PG1_MAR7 0x0f      /*!< \brief Multicast Address Register 7 */

/*
 * Command register bits. (0x00, Read/Write)
 */
#define CR_STOP 0x01   /*!< \brief Stop */
#define CR_START 0x02  /*!< \brief Start */
#define CR_TXP 0x04    /*!< \brief Transmit packet */
#define CR_RD0 0x08    /*!< \brief Remote DMA command bit 0 */
#define CR_RD1 0x10    /*!< \brief Remote DMA command bit 1 */
#define CR_RD2 0x20    /*!< \brief Remote DMA command bit 2 */
#define CR_PS0 0x40    /*!< \brief Page select bit 0 */
#define CR_PS1 0x80    /*!< \brief Page select bit 1 */

/*
 * Interrupt status register bits. (0x07, Read/Write)
 */
#define ISR_PRX 0x01      /*!< \brief Packet received */
#define ISR_PTX 0x02      /*!< \brief Packet transmitted */
#define ISR_RXE 0x04      /*!< \brief Receive error */
#define ISR_TXE 0x08      /*!< \brief Transmit error */
#define ISR_OVW 0x10      /*!< \brief Overwrite warning */
#define ISR_CNT 0x20      /*!< \brief Counter overflow */
#define ISR_RDC 0x40      /*!< \brief Remote DMA complete */
#define ISR_RST 0x80      /*!< \brief Reset status */

/*
 * Interrupt mask register bits. (0x0F, Write)
 */
#define IMR_PRXE 0x01     /*!< \brief Packet received interrupt enable */
#define IMR_PTXE 0x02     /*!< \brief Packet transmitted interrupt enable */
#define IMR_RXEE 0x04     /*!< \brief Receive error interrupt enable */
#define IMR_TXEE 0x08     /*!< \brief Transmit error interrupt enable */
#define IMR_OVWE 0x10     /*!< \brief Overwrite warning interrupt enable */
#define IMR_CNTE 0x20     /*!< \brief Counter overflow interrupt enable */
#define IMR_RCDE 0x40     /*!< \brief Remote DMA complete interrupt enable */


/*
 * Data configuration register bits. (0x0E, Write)
 */
#define DCR_WTS  0x01     /*!< \brief Word transfer select */
#define DCR_RDCR 0x80     /*!< \brief Remote DMA always completed */

/*
 * Transmit configuration register bits. (0x0D, Write)
 */
#define TCR_CRC 0x01      /*!< \brief Inhibit CRC */
#define TCR_LB0 0x02      /*!< \brief Encoded loopback control bit 0 */
#define TCR_LB1 0x04      /*!< \brief Encoded loopback control bit 1 */
#define TCR_RLO 0x20      /*!< \brief Full Duplex */
#define TCR_PD  0x40      /*!< \brief Pad Disable */
#define TCR_FDU 0x80      /*!< \brief Retry of late collision */

/*
 * Transmit status register bits. (0x04, Read)
 */
#define TSR_PTX 0x01      /*!< \brief Packet transmitted */
#define TSR_COL 0x04      /*!< \brief Transmit collided */
#define TSR_ABT 0x08      /*!< \brief Transmit aborted */
#define TSR_OWC 0x80      /*!< \brief Out of window collision */

/*
 * Receive configuration register bits. (0x0C, Write)
 */
#define RCR_SEP 0x01      /*!< \brief Save errored packets */
#define RCR_AR 0x02       /*!< \brief Accept runt packets */
#define RCR_AB 0x04       /*!< \brief Accept broadcast */
#define RCR_AM 0x08       /*!< \brief Accept multicast */
#define RCR_PRO 0x10      /*!< \brief Promiscuous physical */
#define RCR_MON 0x20      /*!< \brief Monitor mode */
#define RCR_INTT 0x40     /*!< \brief Interrupt Trigger Mode */

/*
 * Receive status register bits. (0x0C, Read)
 */
#define RSR_PRX 0x01      /*!< \brief Packet received intact */
#define RSR_CR 0x02       /*!< \brief CRC error */
#define RSR_FAE 0x04      /*!< \brief Frame alignment error */
#define RSR_FO 0x08       /*!< \brief FIFO overrun */
#define RSR_MPA 0x10      /*!< \brief Missed packet */
#define RSR_PHY 0x20      /*!< \brief Physical/multicast address */
#define RSR_DIS 0x40      /*!< \brief Receiver disabled */

/*
 * MII/EEPROM access regiser bits. (0x14, Read/Write)
 */
#define MII_EEP_MDC 0x01   /*!< \brief MII clock */
#define MII_EEP_MDIR 0x02  /*!< \brief MII MDIO directions */
#define MII_EEP_MDI 0x04   /*!< \brief MII data In */
#define MII_EEP_MDO 0x08   /*!< \brief MII data Out */
#define MII_EEP_EECS 0x10  /*!< \brief EEPROM Chip select */
#define MII_EEP_EEI 0x20   /*!< \brief EEPROM data in */
#define MII_EEP_EEO 0x40   /*!< \brief EEPROM data out */
#define MII_EEP_EECLK 0x80 /*!< \brief EEPROM clock*/

/*
 * TR access regiser bits. (0x15, Read/Write)
 */
#define TR_RST_B 0x02 /*!< \brief Reset busy */


/*
 * The Asix Embedded PHY Registers
 */

#define PHY_MR0  0x00	/*!< \brief Control*/
#define PHY_MR1  0x01	/*!< \brief Status*/
#define PHY_MR2  0x02	/*!< \brief PHY Identifier 1*/
#define PHY_MR3  0x03	/*!< \brief PHY Identifier 2*/
#define PHY_MR4  0x04	/*!< \brief Autonegotiation Advertisement*/
#define PHY_MR5  0x05	/*!< \brief Autonegotiation Link Partner Ability*/
#define PHY_MR6  0x06	/*!< \brief Autonegotiation Expansion*/
#define PHY_MR7  0x07	/*!< \brief Next Page Transmit*/
#define PHY_MR16 0x10	/*!< \brief PCS Control Register*/
#define PHY_MR17 0x11	/*!< \brief Autonegotiation (read register A)*/
#define PHY_MR18 0x12	/*!< \brief Autonegotiation (read register B)*/
#define PHY_MR19 0x13	/*!< \brief Analog Test Register*/
#define PHY_MR20 0x14	/*!< \brief User-defined Register*/
#define PHY_MR21 0x15	/*!< \brief RXER Counter*/
#define PHY_MR22 0x16	/*!< \brief Analog Test Registers*/
#define PHY_MR23 0x17	/*!< \brief Analog Test Registers*/
#define PHY_MR24 0x18	/*!< \brief Analog Test Registers*/
#define PHY_MR25 0x19	/*!< \brief Analog Test (tuner) Registers*/
#define PHY_MR26 0x1a	/*!< \brief Analog Test (tuner) Registers*/
#define PHY_MR27 0x1b	/*!< \brief Analog Test (tuner) Registers*/
#define PHY_MR28 0x1c	/*!< \brief Device Specific 1*/
#define PHY_MR29 0x1d	/*!< \brief Device Specific 2*/
#define PHY_MR30 0x1e	/*!< \brief Device Specific 3*/
#define PHY_MR31 0x1f	/*!< \brief Quick Status Register*/

/*
 * PHY MR0 (Control) access regiser bits.
 */
#define MR0_SW_RESET	0x8000	/* R/W Reset, all PHY registers will be set to default */
#define MR0_LOOPBACK	0x4000	/* R/W Loopback */
#define MR0_SPEED100	0x2000	/* R/W Speed selection, 1 = 100Mbits/s, 0 = 10Mbits/s */
#define MR0_NWAY_ENA	0x1000	/* R/W Autonegotiation Enable */
#define MR0_PWRDN		0x0800	/* R/W Powerdown */
#define MR0_ISOLATE		0x0400	/* R/W Isolate */
#define MR0_REDONWAY	0x0200	/* R/W Restart autonegotiation */
#define MR0_FULL_DUP	0x0100	/* R/W Duplex mode selection, 1 = FULL, 0 = HALF */
#define MR0_COLTST		0x0080	/* R/W Collision Test */

/*
 * PHY MR1 (Status) access regiser bits.
 */
#define MR1_T4ABLE		0x8000	/* R 100Base-T4 Ability. This bit will always be a 0 */
#define MR1_TXFULDUP	0x4000	/* R 100Base-TX Full-Duplex Ability. This bit will always be a 1 */
#define MR1_TXHAFDUP	0x2000	/* R 100Base-TX Half-Duplex Ability. This bit will always be a 1 */
#define MR1_ENFULDUP	0x1000	/* R 10Base-T Full-Duplex Ability. This bit will always be a 1 */
#define MR1_ENHAFDUP	0x0800	/* R 10Base-T Half-Duplex Ability. This bit will always be a 1 */
#define MR1_NO_PA_OK	0x0040	/* R Suppress preamble */
#define MR1_NWAYDONE	0x0020	/* R Autonegotiation complete */
#define MR1_REM_FLT		0x0010	/* R Remote fault */
#define MR1_NWAYABLE	0x0008	/* R Autonegotiation ability */
#define MR1_LSTAT_OK	0x0004	/* R Link status */
#define MR1_JABBER		0x0002	/* R Jabber detect */
#define MR1_EXT_ABLE	0x0001	/* R Extended capability */


/*
 * PHY MR31 (Quick Status) access regiser bits.
 */
#define MR31_LSTAT_OK	0x0800	/* R Link status */
#define MR31_SPEED100	0x0200	/* R Link Speed, 0=10Mbits/s, 1=100Mbits/s */
#define MR31_FULL_DUP	0x0100	/* R Duplex Mode, 0=Half, 1=Full */


/*
0x8000	15
0x4000	14
0x2000	13
0x1000	12
0x0800	11
0x0400	10
0x0200	9
0x0100	8
0x0080	7
0x0040	6
0x0020	5
0x0010	4
0x0008	3
0x0004	2
0x0002	1
0x0001	0

*/

/*@}*/

#endif

