#ifndef MAIN_H
#define MAIN_H

void init_spi(void);
void init_pwm(void);
void init_usart(void);
void init_eeprom(void);

void eeprom_load(void);
void eeprom_save(void);



extern volatile uint8_t usart_pwm; 

extern volatile uint8_t main_pwm[4];

extern volatile uint8_t activity;



extern uint8_t	spi_data[SPI_BUF_SIZE];
extern uint8_t	spi_count; 	// SPI n bytes received
extern uint8_t	spi_rx_enable;	// SPI can receive bytes


#endif // MAIN_H
