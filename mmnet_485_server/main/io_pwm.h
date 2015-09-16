
/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Software PWM.
 *
**/


#include "defs.h"

void timer1_init(void);
void timer1_start(void);

extern unsigned char pwm_mask_byte;
extern unsigned char pwm[SERVANT_NPWM];

extern unsigned char pwm_count;


void set_an(unsigned char port_num, unsigned char data);


