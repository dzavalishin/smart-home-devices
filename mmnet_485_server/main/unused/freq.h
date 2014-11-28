#include "defs.h"

void send_changed_freq_data(void);
void freq_meter_init(void);
void freq_meter_05sec_timer(void);
void request_freq_data_send(unsigned char channel);

extern unsigned int freq_outs[SERVANT_NFREQ];



