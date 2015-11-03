/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Analogue inputs.
 *
**/


#include "defs.h"
#include "map.h"

void adc_init(void);

void adc_start(void);
//void adc_stop();

//unsigned char poll_an(unsigned char port_num);
//unsigned char get_an(unsigned char port_num);

void send_changed_adc_data(void);
void request_adc_data_send(unsigned char channel);


// Keeps current ADC channel value
extern unsigned volatile int adc_value[SERVANT_NADC];

extern dev_major io_adc;

