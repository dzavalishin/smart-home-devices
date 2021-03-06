/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Software PWM.
 *
**/


#include "defs.h"
#include "map.h"

void timer1_init(void);
void timer1_start(void);

extern unsigned char pwm_mask_byte;
extern unsigned char pwm[SERVANT_NPWM];

extern unsigned char pwm_count;


void 	set_an(unsigned char port_num, unsigned char data);

void    pwm_set_default_output_state(void);

extern dev_major io_pwm;


