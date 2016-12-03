/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
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
void rescan_temperature(void);
void temp_meter_measure(void); // Must be called once a 750 ms (actually once a sec)

extern uint8_t serialNumber [OW_ROMCODE_SIZE]; 

#if SERVANT_NTEMP > 0
extern uint8_t nTempSensors;

extern uint8_t  gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE];
extern uint16_t currTemperature[SERVANT_NTEMP];

#if !OW_ONE_BUS
extern uint8_t 	gTempSensorBus[SERVANT_NTEMP]; // Which bus this sensor lives on
#endif

#endif

extern uint16_t 	ow_error_cnt; // 1wire error counter


uint8_t search_sensors(uint8_t currBus);

const char *temptoa( uint16_t temperature, char *buf ); // NB! Uses buf to print to!
