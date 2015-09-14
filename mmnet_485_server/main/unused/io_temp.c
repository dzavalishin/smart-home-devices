/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * 1-Wire temperature sensors.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"

#include <inttypes.h>
#include <stdio.h>

#include "io_temp.h"
#include <dev/owibus.h>

// for avrportg
#include <dev/gpio.h>
//#include <cfg/arch.h>


uint8_t serialNumber [OW_ROMCODE_SIZE];
uint8_t gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE]; // Used by MAC generation
uint16_t oldTemperature[SERVANT_NTEMP];

static NUTOWIBUS bus_container;
static NUTOWIBUS *bus = &bus_container;


static int io_temp_search_1w_devices(void);


void init_temperature(void)
{
    // TODO hardcoded port!
    int rc;
    rc = NutRegisterOwiBus_BB(bus, AVRPORTG, OW_PIN, 0, 0);

    rc = io_temp_search_1w_devices();

}

void temp_meter_measure(void) // Must be called once a 750 ms (actually once a sec)
{
}










static int io_temp_search_1w_devices(void)
{
    uint8_t diff;
    uint64_t hid  = 0;
    int rc;

    diff = OWI_SEARCH_FIRST;
    rc = OwiRomSearch(bus, &diff, &hid);

    if(rc)
    {
        printf("OwiRomSearch failed\n");
        return rc;
    }

    onewire_available = 1;

    fprintf(stdout, "Hardware ID of first device %08lx%08lx\n",
            (uint32_t)(hid>>32),
            (uint32_t)(hid &0xffffffff));


    return 0;
}








