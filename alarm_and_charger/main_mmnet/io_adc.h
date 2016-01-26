/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * Analogue inputs.
 *
**/


#include "defs.h"
#include "dev_map.h"

// Keeps current ADC channel value
extern unsigned volatile int adc_value[SERVANT_NADC];

extern dev_major io_adc;

float get_float_adc( uint8_t nAdc );

