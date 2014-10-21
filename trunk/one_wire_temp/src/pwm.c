#include "freq_defs.h"
#include "pwm.h"
#include "defs.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>



#if SERVANT_NPWM > 0

unsigned char pwm_count;
unsigned char pwm[SERVANT_NPWM];
unsigned char pwm_sort[SERVANT_NPWM], pwm_order[SERVANT_NPWM];
unsigned char pwm_mask_byte;
unsigned int pwm_time[SERVANT_NPWM];


//TIMER1 initialize - prescale:1 !!! ��� ���ᨬ��쭮� ᪮��� PWM-��
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1000uSec
// actual value: 1000,000uSec (0,0%)
void timer1_init(void)
{
    TCCR1B = 0x00; //stop
    TCNT1H = 0xF0; //setup
    TCNT1L = 0x60;
    OCR1AH = 0x01;
    OCR1AL = 0x90;
    OCR1BH = 0x01;
    OCR1BL = 0x90;
    ICR1H  = 0x01;
    ICR1L  = 0x90;
    TCCR1A = 0x00;
    // TCCR1B = 0x01; //start Timer �����

    pwm_count=0;
    pwm_mask_byte=0b00000000;
}

void timer1_start(void)
{
    TIMSK |= _BV(TOIE1);

    TCCR1B = 0x01; //start Timer1
}

//#pragma interrupt_handler timer1_ovf_isr:15
//void timer1_ovf_isr(void)
ISR(TIMER1_OVF_vect)
{
    // ॠ������ ��᪮�쪨� PWM-�� �� 1 ⠩���
    unsigned char j, mask;
    cli();   // �������� ���뢠���
    // TIMER1 has overflowed

    if (pwm_mask_byte>0) {

        pwm_count++; // ��� ��� PWM

        if (pwm_count==SERVANT_NPWM+1) {
            pwm_count=0;	// ��砫� 横�� PWM
            mask=pwm_mask_byte&0x0f; // �� ������� ����⨢�� PWM-�
            PORTC|=mask;	// ������� �� ��室�, PC0-3; ��������, �᫨ ��������� �뢮�� ���������� ��室��
        }

        j=pwm_order[pwm_count-1];
        // �몫���� ����� PWM ��᫥����⥫쭮 �� �����⠭�� ���祭��
        if (pwm_count!=0)
        {
            if ((pwm_mask_byte & _BV(j))!=0)
            {
                PORTC &= ~_BV(j);
            }
        }

        TCNT1H=pwm_time[pwm_count]>>8;
        TCNT1L=pwm_time[pwm_count];
    }
    else {
        // PWM-� �� �������, ����� 100 ���.
        TCNT1H = 0xF0; //setup
        TCNT1L = 0x60;
    }
    sei();   // ࠧ���� ���뢠���
}


void set_an(unsigned char port_num, unsigned char data) {
    // ����� �ᯮ�짮���� ���஥��� PWM, �� �� ����
    // � ��� �������� ⠩����
    // ��� ����襣� �᫠ ���������� �뢮���
    // PWM ����� ॠ�������� �� ����� ⠩���
    unsigned char i, j, min, s;

    pwm[port_num]=data;

    // ���஢�� ���祭�� PWM-��, ���������� ���冷�
    for (i=0; i<SERVANT_NPWM; i++) {
        pwm_sort[i]=pwm[i];
        pwm_order[i]=i;
    }

    for (i=0; i<SERVANT_NPWM; i++) {
        min=i;
        for (j=i+1; j<SERVANT_NPWM; j++) {
            if (pwm_sort[j]<pwm_sort[min]) {
                min=j;
            }
        }
        s=pwm_sort[i];
        pwm_sort[i]=pwm_sort[min];
        pwm_sort[min]=s;

        if (min!=i) {
            pwm_order[min]=pwm_order[i]; // ���������� ���冷�
            pwm_order[i]=min;
        }
    }

    // �� ��ਮ�� �����樨 ��᫥����⥫쭮 ����᪠�� ⠩���
    // ����� ���祭�ﬨ ������� ��室� PWM
    pwm_time[0]=~(pwm_sort[0]*10);
    for (i=1; i<SERVANT_NPWM; i++) {
        pwm_time[i]=~((pwm_sort[i]-pwm_sort[i-1])*SERVANT_PWM_SPEED);
        // ��ਮ� �����樨 ࠧ���塞 �� 256*(PWM_SPEED=10) ��設��� 横���
    }
    // ��᫥ �몤�祭�� ��� PWM ��������� �� ����砭�� ��ਮ��
    pwm_time[SERVANT_NPWM]=~((0xff-pwm_sort[SERVANT_NPWM-1])*SERVANT_PWM_SPEED); // �� ��᫥����� ������ �� ���� 横��
}


#endif // SERVANT_NPWM > 0

