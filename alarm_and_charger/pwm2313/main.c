#include "defs.h"

#include <avr/io.h>
#include <stdlib.h>

#include "main.h"

int
main(void)
{
	init_spi();
	init_pwm();
	init_usart();

	for(;;)
	{
	}

}

