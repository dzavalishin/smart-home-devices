#define FLASH_BASE      ((unsigned int *)0x01000000)
#define FLASH_SIZE      (2*1024*1024)

/* 
 * EBI initialization.
 *
 * 0x01000000, 16MB, 2 tdf, 16 bits, 7 WS  
 */
#define EBI_CSR_0       ((unsigned int *)(FLASH_BASE | 0x2539)     
#define EBI_CSR_1       ((unsigned int *)0x10000000)
#define EBI_CSR_2       ((unsigned int *)0x20000000)
#define EBI_CSR_3       ((unsigned int *)0x30000000)
#define EBI_CSR_4       ((unsigned int *)0x40000000)
#define EBI_CSR_5       ((unsigned int *)0x50000000)
#define EBI_CSR_6       ((unsigned int *)0x60000000)
#define EBI_CSR_7       ((unsigned int *)0x70000000)

/*
 * LEDs
 */
#define LED1            _BV(16)
#define LED2            _BV(17)
#define LED3            _BV(18)
#define LED4            _BV(19)
#define LED5            _BV(3)
#define LED6            _BV(4)
#define LED7            _BV(5)
#define LED8            _BV(6)

#define LED_PIO_CTRL    1
#define LED_MASK        (LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8)

#define LED_ON          PIO_CLEAR_OUT
#define LED_OFF         PIO_SET_OUT

#define SW1_MASK        _BV(12)
#define SW2_MASK        _BV(9)
#define SW3_MASK        _BV(1)
#define SW4_MASK        _BV(2)
#define SW_MASK         (SW1_MASK|SW2_MASK|SW3_MASK|SW4_MASK)

/*
 * Push buttons.
 */
#define SW1             _BV(12)
#define SW2             _BV(9)
#define SW3             _BV(1)
#define SW4             _BV(2)

#define PIO_SW1         _BV(12)
#define PIO_SW2         _BV(9)
#define PIO_SW3         _BV(1)
#define PIO_SW4         _BV(2)

/*
 * Serial EEPROM
 */
#define SCL             _BV(8)
#define SDA             _BV(7)
#define PIO_SCL         _BV(8)
#define PIO_SDA         _BV(7)

/*
 * Master clock.
 */
#define MCK             66000000
#define MCKKHz          (MCK/1000)
