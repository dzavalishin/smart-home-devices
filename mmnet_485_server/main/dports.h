#include "defs.h"

void port_init(void);

//unsigned char get_dig(unsigned char port_num); //
unsigned char get_dig_out(unsigned char port_num);

void set_dig(unsigned char port_num, unsigned char data);
void set_ddr(unsigned char port_num, unsigned char data);


void check_digital_inputs(void);
void request_dig_data_send(unsigned char channel);
void send_changed_dig_data(void);


extern unsigned char dig_value[SERVANT_NDIG];

