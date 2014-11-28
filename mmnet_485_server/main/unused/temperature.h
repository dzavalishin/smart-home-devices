#include "defs.h"
#include "onewire.h"

void send_changed_temperature_data(void);
void init_temperature(void);
void request_temp_data_send(unsigned char channel);
void temp_meter_sec_timer(void);

extern uint8_t serialNumber [OW_ROMCODE_SIZE]; 
extern uint8_t gTempSensorIDs[N_TEMPERATURE_IN][OW_ROMCODE_SIZE]; // Used by MAC generation

extern uint16_t oldTemperature[N_TEMPERATURE_IN];


