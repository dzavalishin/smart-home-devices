#include "defs.h"

#include "onewire.h"

void send_changed_temperature_data(void);
void init_temperature(void);
void request_temp_data_send(unsigned char channel);
void temp_meter_05sec_timer(void);


#if N_TEMPERATURE_IN > 0
extern uint8_t gTempSensorIDs[N_TEMPERATURE_IN][OW_ROMCODE_SIZE];

extern uint8_t gTempSensorBus[N_TEMPERATURE_IN]; 			// Which bus this sensor lives on

extern uint8_t gTempSensorLogicalNumber[N_TEMPERATURE_IN]; 	// Report sensor with this logical number

extern uint16_t currTemperature[N_TEMPERATURE_IN];

extern uint8_t serialNumber [OW_ROMCODE_SIZE]; 

extern uint8_t nTempSensors;


extern uint16_t ow_error_cnt; // 1wire error counter

#endif

// unmap by logical number
void remove_mapping_by_id( uint8_t id );
void remove_mapping_by_rom( uint8_t rom[] );
void add_mapping( uint8_t rom[], uint8_t id );
int8_t find_pos_by_persistent_id( uint8_t id );

