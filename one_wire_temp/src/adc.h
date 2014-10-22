#include "defs.h"

void adc_init(void);

void adc_start();
//void adc_stop();

#if SERVANT_NADC > 0
    // Keeps current ADC channel value
    extern unsigned volatile int adc_value[SERVANT_NADC];
#endif
