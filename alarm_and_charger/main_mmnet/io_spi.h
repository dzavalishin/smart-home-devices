#include "defs.h"
#include "dev_map.h"
//void spi_init(void);

extern dev_major io_spi;

//
// ss   - slave select number
//
// cmd1. cmd2 - bytes to send
//
//
//

unsigned char  spi_send( unsigned char ss, unsigned char cmd1, unsigned char cmd2 );

