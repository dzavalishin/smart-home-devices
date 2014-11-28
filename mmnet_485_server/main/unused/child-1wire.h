#include "defs.h"

#if ENABLE_CH1W

#include <stdint.h>
#include "../child-1wire/twi_slave_cmd.h"

void child_1wire_init(void);

uint8_t ch1w_reset(void);
uint16_t ch1w_read_temperature(uint8_t channel_number );


#endif




