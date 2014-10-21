#if 0
//ATmega16
#define dXTAL 16000000

#include <avr/io.h>

//#include "ioavr.h"
#include "ina90.h"
//#include "stdbool.h"

#include "modbus.h"


//размер буфера принимаемых по UART данных
#define MaxLenghtRecBuf 25
//размер буфера передаваемых по UART данных
#define MaxLenghtTrBuf 25


#define SetBit(Port,bit) Port|=(1<<bit)
#define ClrBit(Port,bit) Port&=~(1<<bit)
#define InvBit(Port,bit) Port^=(1<<bit)
#define Hi(Int) (char) (Int>>8)
#define Low(Int) (char) (Int)

//ModBus
char ModBus(char NumByte);
bool bModBus;  //флаг обработки посылки
unsigned char cNumRcByte0; //передает в обработчик кол-во принятых байт
unsigned char cNumTrByte0;  //кол-во передаваемых данных

//UART
void StartUART0(void);
void StartTrans0(void);
unsigned char cmRcBuf0[MaxLenghtRecBuf] ; //буфер принимаемых данных
unsigned char cmTrBuf0[MaxLenghtTrBuf] ; //буфер передаваемых данных

//начальные настройки
void Setup(void){
  DDRB=0xFF;  //порт на выход
  PORTB=0xFF; //на выходе 1


//  SetBit(DDRD, 4);  //настройка управления мультиплесором
//  ClrBit(PORTD,4);  //COM-порт
//  SetBit(DDRD, 5);  //разрешение работы мультиплексора
//  ClrBit(PORTD,5);  //
}//end Setup()

int main( void ){
  Setup();  //настройка регистров МК
  _SEI(); //разрешение прерываний
  StartUART0();

  while(1){
    if (bModBus){
      cNumTrByte0=ModBus(cNumRcByte0); //обработка принятого соообщения ModBus
      if (cNumTrByte0!=0) StartTrans0();
      bModBus=false;
    }//end if (bModBus)
  }//end while(1)
}//end main()


//формирование ответа об ошибке
char ErrorMessage(char Error){
  char TempI;
  cmTrBuf0[1]=cmRcBuf0[1]+0x80;;//команда с ошибкой
  cmTrBuf0[2]=Error;
  TempI=modbus_CRC16(cmTrBuf0,3);//подсчет КС посылки
  cmTrBuf0[3]=Low(TempI);
  cmTrBuf0[4]=Hi(TempI);
  return 5;
}//end ErrorMessage()

int CRC16;
char ModBus(char NumByte){
  int TempI;
   //вывод посылки на экран
  cmRcBuf0[NumByte]=0x00;

  //обработка посылки
  if (cmRcBuf0[0]!=0x20) return 0x00; //адрес устройства  //ответ не нужен
  CRC16=modbus_CRC16(cmRcBuf0,NumByte-2);//подсчет CRC в принятой посылке
  TempI=(int) (cmRcBuf0[NumByte-1]<<8) + cmRcBuf0[NumByte-2];
  if (CRC16!=TempI) return 0x00;  //контрольная сумма //ответ не нужен
  cmTrBuf0[0]=0x20;//адрес устройства
  //код команды
  switch(cmRcBuf0[1]){
    case 0x03:{//чтение регистров
      TempI=(int) (cmRcBuf0[2]<<8) + cmRcBuf0[3];
      if (TempI!=1){ //првоерка номера регистра, есть только 1 регистр
        return ErrorMessage(0x02); //данный адрес не может быть обработан
      }
      TempI=(int) (cmRcBuf0[4]<<8) + cmRcBuf0[5];
      if (TempI!=1){//проверка кол-ва запрашиваемых регистров, есть только 1 регистр
         return ErrorMessage(0x02); //данный адрес не может быть обработан
      }
      cmTrBuf0[1]=0x03;//команда
      cmTrBuf0[2]=0x02;//кол-во байт данных
      cmTrBuf0[3]=0x00;//старший байт
      TempI=PINB;
      cmTrBuf0[4]=Low(TempI);//уровни порта F
      TempI=modbus_CRC16(cmTrBuf0,5);//подсчет КС посылки
      cmTrBuf0[5]=Low(TempI);
      cmTrBuf0[6]=Hi(TempI);
      return 7;
    }
    case 0x06:{//запись в единичный регистр
      TempI=(int) (cmRcBuf0[2]<<8) + cmRcBuf0[3];
      if (TempI!=1){ //првоерка номера регистра, есть только 1 регистр
         return ErrorMessage(0x02); //данный адрес не может быть обработан
      }
      TempI=(int) (cmRcBuf0[4]<<8) + cmRcBuf0[5];
      if (TempI>0xFF){  //проверка числа, которое надо записать в порт
         return ErrorMessage(0x03); //недопустимые данные в запросе
      }
      PORTB=Low(TempI);
      cmTrBuf0[1]=cmRcBuf0[1];//команда
      cmTrBuf0[2]=cmRcBuf0[2];//адрес
      cmTrBuf0[3]=cmRcBuf0[3];//
      cmTrBuf0[4]=cmRcBuf0[4];//данные
      cmTrBuf0[5]=cmRcBuf0[5];//
      cmTrBuf0[6]=cmRcBuf0[6];//КС
      cmTrBuf0[7]=cmRcBuf0[7];//
      return 8;
    }
    default:{
       return ErrorMessage(0x01); //недопустимая команда
    }
  }
}//end ModBus()

//запретить прерывание приема
#define DisableReceive0 ClrBit(UCSRB,RXEN); ClrBit(UCSRB,RXCIE)
//разрешить прерывание приема
#define EnableReceive0 SetBit(UCSRB,RXEN); SetBit(UCSRB,RXCIE)
//проверка на разрешение прерывания приема
#define TestReceive0 TestBit(UCSRB,RXCIE)
//разрешить прерывание по освобождению буфера передачи, начать передачу
#define GoTrans0 SetBit(UCSRB,TXEN); SetBit(UCSRB,UDRIE)
//запретить прерывание по освобождению буфера передачи, остановка передачи
#define StopTrans0 ClrBit(UCSRB,TXEN); ClrBit(UCSRB,UDRIE)
//проверка на разрешение прерывания передачи
#define TestTrans0 TestBit(UCSRB,UDRIE)


#define StartTimer0 TCNT0=dTCNT0;TCCR0=0x03; //запуск таймера 0
#define InitTimer0 TIFR&=0xFD;TIMSK|=(1<<TOIE0); //инициализация таймера0

//MaxPause = 1.5*(8+1+2)/bod = 0.85мс -> TCNT0~40
#define dTCNT0 40

unsigned char RcCount, TrCount;  //счетчик принятых/переданных данных дданных
bool StartRec=false;// false/true начало/прием посылки
unsigned char DataPause;  //пауза между байтами

extern bool bModBus;  //флаг обработки посылки
extern unsigned char cNumRcByte0; //передает в обработчик кол-во принятых байт
extern unsigned char cNumTrByte0;

//настройка UART
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
#pragma vector=USART_RXC_vect //адрес прерывания приема UART
__interrupt void UART_RX_interrupt(void){
  cTempUART=UDR;

  if  (UCSRA&(1<<FE)) return;   //FE-ошибка кадра, OVR - переполнение данных (флаги)

  if (!StartRec){ //если это первый байт, то начинаем прием
      StartRec=true;
      RcCount=0;
      DataPause=0;
      cmRcBuf0[RcCount++]=cTempUART;
      StartTimer0;
  }else{// end if (StartRec==0) //продолжаем прием
    if (RcCount<MaxLenghtRecBuf){//если еще не конец буфера
      cmRcBuf0[RcCount++]=cTempUART;
    }else{//буфер переполнен
      cmRcBuf0[MaxLenghtRecBuf-1]=cTempUART;
    }
    DataPause=0;
    TCNT0=dTCNT0;//перезапуск таймера
  }//end else if (StartRec==0)
}//end  __interrupt UART0_RX_interrupt()

#pragma vector=USART_UDRE_vect //адрес прерывания передачи в буфер UART
__interrupt void UART_UDRE_interrupt(void){
  if (TrCount<cNumTrByte0){
    UDR=cmTrBuf0[TrCount];
    TrCount++;
  }else{
    StopTrans0;
    TrCount=0;
  }
}//end __interrupt UART0_UDRE_interrupt()

//разрешение передачи по UART, с указанием ко-ва передаваемых байтов
void StartTrans0(void){
  TrCount=0;
  GoTrans0;
}//end  void StartTrans1()

#pragma vector=TIMER0_OVF_vect  //адрес прерывания таймера/счетчика 0 по переполнению
__interrupt void Timer0_overflowed_interrupt(void){
  if (StartRec){
    StartRec=false; //посылка принята
    cNumRcByte0=RcCount;  //кол-во принятых байт
    bModBus=true;//
    TCCR0=0;//остановим таймер
  }
}//end __interrupt void Timer0_overflowed_interrupt()

#endif

