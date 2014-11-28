#include "alarm.h"
#include "defs.h"

unsigned char warn_max[SERVANT_NADC], warn_min[SERVANT_NADC], warn_byte[SERVANT_NDIG], warn_mask[SERVANT_NDIG];
unsigned char al_max[SERVANT_NADC], al_min[SERVANT_NADC], al_byte[SERVANT_NDIG], al_mask[SERVANT_NDIG];

void init_alarm(void)
{
    unsigned char i;

    for( i=0; i<SERVANT_NADC; i++ ) {
        al_min[i]=0;
        al_max[i]=0xff;

        warn_min[i]=0;
        warn_max[i]=0xff;
    }

    for( i=0; i<SERVANT_NDIG; i++ ) {
        al_byte[i]=0;
        al_mask[i]=0;

        warn_byte[i]=0;
        warn_mask[i]=0;
    }
}

// TODO check warning and alarm conditions here
