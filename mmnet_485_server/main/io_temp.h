/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * 1-Wire temperature sensors.
 *
**/

#include "defs.h"

#include <inttypes.h>
#include "onewire.h"


// TODO remove?
//#define OW_ROMCODE_SIZE 8

void init_temperature(void);
void temp_meter_measure(void); // Must be called once a 750 ms (actually once a sec)

extern uint8_t serialNumber [OW_ROMCODE_SIZE]; 

#if SERVANT_NTEMP > 0
extern uint8_t nTempSensors;

extern uint8_t  gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE];
extern uint16_t currTemperature[SERVANT_NTEMP];
#endif


uint8_t search_sensors(uint8_t currBus);

const char *temptoa( uint16_t temperature, char *buf ); // NB! Uses buf to print to!
