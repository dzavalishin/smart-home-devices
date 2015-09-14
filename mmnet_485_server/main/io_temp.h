/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * 1-Wire temperature sensors.
 *
**/

#include "defs.h"

#include <inttypes.h>


// TODO remove?
#define OW_ROMCODE_SIZE 8

void init_temperature(void);
void temp_meter_measure(void); // Must be called once a 750 ms (actually once a sec)

extern uint8_t serialNumber [OW_ROMCODE_SIZE]; 
extern uint8_t gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE]; // Used by MAC generation
extern uint16_t oldTemperature[SERVANT_NTEMP];

