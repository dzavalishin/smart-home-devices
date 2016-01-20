#ifndef MAIN_H
#define MAIN_H

void init_spi(void);
void init_pwm(void);
void init_usart(void);

extern uint8_t usart_pwm; 

extern uint8_t main_pwm[4];

extern volatile uint8_t activity;


#endif // MAIN_H
