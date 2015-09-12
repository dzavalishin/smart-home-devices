
// TODO remove?
#define OW_ROMCODE_SIZE 8

void init_temperature(void);
void temp_meter_measure(void);

extern uint8_t serialNumber [OW_ROMCODE_SIZE]; 
extern uint8_t gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE]; // Used by MAC generation
extern uint16_t oldTemperature[SERVANT_NTEMP];
