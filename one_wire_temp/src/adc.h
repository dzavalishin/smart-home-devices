#include "defs.h"

void adc_init(void);

void adc_start();
//void adc_stop();

//unsigned char poll_an(unsigned char port_num);
//unsigned char get_an(unsigned char port_num);

void send_changed_adc_data(void);
void request_adc_data_send(unsigned char channel);


#if SERVANT_NADC > 0
    // Keeps current ADC channel value
    extern unsigned volatile int adc_value[SERVANT_NADC];
#endif
