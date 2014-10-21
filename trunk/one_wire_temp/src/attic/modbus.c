#if 0
//ATmega16
#define dXTAL 16000000

#include <avr/io.h>

//#include "ioavr.h"
#include "ina90.h"
//#include "stdbool.h"

#include "modbus.h"


//������ ������ ����������� �� UART ������
#define MaxLenghtRecBuf 25
//������ ������ ������������ �� UART ������
#define MaxLenghtTrBuf 25


#define SetBit(Port,bit) Port|=(1<<bit)
#define ClrBit(Port,bit) Port&=~(1<<bit)
#define InvBit(Port,bit) Port^=(1<<bit)
#define Hi(Int) (char) (Int>>8)
#define Low(Int) (char) (Int)

//ModBus
char ModBus(char NumByte);
bool bModBus;  //���� ��������� �������
unsigned char cNumRcByte0; //�������� � ���������� ���-�� �������� ����
unsigned char cNumTrByte0;  //���-�� ������������ ������

//UART
void StartUART0(void);
void StartTrans0(void);
unsigned char cmRcBuf0[MaxLenghtRecBuf] ; //����� ����������� ������
unsigned char cmTrBuf0[MaxLenghtTrBuf] ; //����� ������������ ������

//��������� ���������
void Setup(void){
  DDRB=0xFF;  //���� �� �����
  PORTB=0xFF; //�� ������ 1


//  SetBit(DDRD, 4);  //��������� ���������� ��������������
//  ClrBit(PORTD,4);  //COM-����
//  SetBit(DDRD, 5);  //���������� ������ ��������������
//  ClrBit(PORTD,5);  //
}//end Setup()

int main( void ){
  Setup();  //��������� ��������� ��
  _SEI(); //���������� ����������
  StartUART0();

  while(1){
    if (bModBus){
      cNumTrByte0=ModBus(cNumRcByte0); //��������� ��������� ���������� ModBus
      if (cNumTrByte0!=0) StartTrans0();
      bModBus=false;
    }//end if (bModBus)
  }//end while(1)
}//end main()


//������������ ������ �� ������
char ErrorMessage(char Error){
  char TempI;
  cmTrBuf0[1]=cmRcBuf0[1]+0x80;;//������� � �������
  cmTrBuf0[2]=Error;
  TempI=modbus_CRC16(cmTrBuf0,3);//������� �� �������
  cmTrBuf0[3]=Low(TempI);
  cmTrBuf0[4]=Hi(TempI);
  return 5;
}//end ErrorMessage()

int CRC16;
char ModBus(char NumByte){
  int TempI;
   //����� ������� �� �����
  cmRcBuf0[NumByte]=0x00;

  //��������� �������
  if (cmRcBuf0[0]!=0x20) return 0x00; //����� ����������  //����� �� �����
  CRC16=modbus_CRC16(cmRcBuf0,NumByte-2);//������� CRC � �������� �������
  TempI=(int) (cmRcBuf0[NumByte-1]<<8) + cmRcBuf0[NumByte-2];
  if (CRC16!=TempI) return 0x00;  //����������� ����� //����� �� �����
  cmTrBuf0[0]=0x20;//����� ����������
  //��� �������
  switch(cmRcBuf0[1]){
    case 0x03:{//������ ���������
      TempI=(int) (cmRcBuf0[2]<<8) + cmRcBuf0[3];
      if (TempI!=1){ //�������� ������ ��������, ���� ������ 1 �������
        return ErrorMessage(0x02); //������ ����� �� ����� ���� ���������
      }
      TempI=(int) (cmRcBuf0[4]<<8) + cmRcBuf0[5];
      if (TempI!=1){//�������� ���-�� ������������� ���������, ���� ������ 1 �������
         return ErrorMessage(0x02); //������ ����� �� ����� ���� ���������
      }
      cmTrBuf0[1]=0x03;//�������
      cmTrBuf0[2]=0x02;//���-�� ���� ������
      cmTrBuf0[3]=0x00;//������� ����
      TempI=PINB;
      cmTrBuf0[4]=Low(TempI);//������ ����� F
      TempI=modbus_CRC16(cmTrBuf0,5);//������� �� �������
      cmTrBuf0[5]=Low(TempI);
      cmTrBuf0[6]=Hi(TempI);
      return 7;
    }
    case 0x06:{//������ � ��������� �������
      TempI=(int) (cmRcBuf0[2]<<8) + cmRcBuf0[3];
      if (TempI!=1){ //�������� ������ ��������, ���� ������ 1 �������
         return ErrorMessage(0x02); //������ ����� �� ����� ���� ���������
      }
      TempI=(int) (cmRcBuf0[4]<<8) + cmRcBuf0[5];
      if (TempI>0xFF){  //�������� �����, ������� ���� �������� � ����
         return ErrorMessage(0x03); //������������ ������ � �������
      }
      PORTB=Low(TempI);
      cmTrBuf0[1]=cmRcBuf0[1];//�������
      cmTrBuf0[2]=cmRcBuf0[2];//�����
      cmTrBuf0[3]=cmRcBuf0[3];//
      cmTrBuf0[4]=cmRcBuf0[4];//������
      cmTrBuf0[5]=cmRcBuf0[5];//
      cmTrBuf0[6]=cmRcBuf0[6];//��
      cmTrBuf0[7]=cmRcBuf0[7];//
      return 8;
    }
    default:{
       return ErrorMessage(0x01); //������������ �������
    }
  }
}//end ModBus()

//��������� ���������� ������
#define DisableReceive0 ClrBit(UCSRB,RXEN); ClrBit(UCSRB,RXCIE)
//��������� ���������� ������
#define EnableReceive0 SetBit(UCSRB,RXEN); SetBit(UCSRB,RXCIE)
//�������� �� ���������� ���������� ������
#define TestReceive0 TestBit(UCSRB,RXCIE)
//��������� ���������� �� ������������ ������ ��������, ������ ��������
#define GoTrans0 SetBit(UCSRB,TXEN); SetBit(UCSRB,UDRIE)
//��������� ���������� �� ������������ ������ ��������, ��������� ��������
#define StopTrans0 ClrBit(UCSRB,TXEN); ClrBit(UCSRB,UDRIE)
//�������� �� ���������� ���������� ��������
#define TestTrans0 TestBit(UCSRB,UDRIE)


#define StartTimer0 TCNT0=dTCNT0;TCCR0=0x03; //������ ������� 0
#define InitTimer0 TIFR&=0xFD;TIMSK|=(1<<TOIE0); //������������� �������0

//MaxPause = 1.5*(8+1+2)/bod = 0.85�� -> TCNT0~40
#define dTCNT0 40

unsigned char RcCount, TrCount;  //������� ��������/���������� ������ �������
bool StartRec=false;// false/true ������/����� �������
unsigned char DataPause;  //����� ����� �������

extern bool bModBus;  //���� ��������� �������
extern unsigned char cNumRcByte0; //�������� � ���������� ���-�� �������� ����
extern unsigned char cNumTrByte0;

//��������� UART
void StartUART0(void){
  UBRRH=Hi(((dXTAL/16)/19200)-1);
  UBRRL=Low(((dXTAL/16)/19200)-1);

UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;

UBRRH=0x00;
UBRRL=0x33;

  EnableReceive0;
  InitTimer0;
  StartTimer0;
}//end void StartUART0()

char cTempUART;
#pragma vector=USART_RXC_vect //����� ���������� ������ UART
__interrupt void UART_RX_interrupt(void){
  cTempUART=UDR;

  if  (UCSRA&(1<<FE)) return;   //FE-������ �����, OVR - ������������ ������ (�����)

  if (!StartRec){ //���� ��� ������ ����, �� �������� �����
      StartRec=true;
      RcCount=0;
      DataPause=0;
      cmRcBuf0[RcCount++]=cTempUART;
      StartTimer0;
  }else{// end if (StartRec==0) //���������� �����
    if (RcCount<MaxLenghtRecBuf){//���� ��� �� ����� ������
      cmRcBuf0[RcCount++]=cTempUART;
    }else{//����� ����������
      cmRcBuf0[MaxLenghtRecBuf-1]=cTempUART;
    }
    DataPause=0;
    TCNT0=dTCNT0;//���������� �������
  }//end else if (StartRec==0)
}//end  __interrupt UART0_RX_interrupt()

#pragma vector=USART_UDRE_vect //����� ���������� �������� � ����� UART
__interrupt void UART_UDRE_interrupt(void){
  if (TrCount<cNumTrByte0){
    UDR=cmTrBuf0[TrCount];
    TrCount++;
  }else{
    StopTrans0;
    TrCount=0;
  }
}//end __interrupt UART0_UDRE_interrupt()

//���������� �������� �� UART, � ��������� ��-�� ������������ ������
void StartTrans0(void){
  TrCount=0;
  GoTrans0;
}//end  void StartTrans1()

#pragma vector=TIMER0_OVF_vect  //����� ���������� �������/�������� 0 �� ������������
__interrupt void Timer0_overflowed_interrupt(void){
  if (StartRec){
    StartRec=false; //������� �������
    cNumRcByte0=RcCount;  //���-�� �������� ����
    bModBus=true;//
    TCCR0=0;//��������� ������
  }
}//end __interrupt void Timer0_overflowed_interrupt()

#endif

