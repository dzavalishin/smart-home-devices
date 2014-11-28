#ifndef ALARM_H_
#define ALARM_H_

extern unsigned char warn_max[], warn_min[], warn_byte[], warn_mask[];
extern unsigned char al_max[], al_min[], al_byte[], al_mask[];

void init_alarm(void);

#endif /*ALARM_H_*/
