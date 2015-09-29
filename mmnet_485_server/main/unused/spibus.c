/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * SPI bus
 *
**/

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"


#include <dev/spibus_avr.h>
#include <dev/spi_at45d.h>


void
spi_bus_init(void)
{
    int rc;

    rc = NutRegisterSpiDevice(&devSpiAt45d0, &spiBus0Avr, 0); // Os CS 0

    rc = NutRegisterDevice(&devNplMmc0, 0, 0);



}

