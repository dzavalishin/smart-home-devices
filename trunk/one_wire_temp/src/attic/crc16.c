#include "crc16.h"

int crc16_calc( const unsigned char *byt, int size )
{
  /* Calculate CRC-16 value; uses The CCITT-16 Polynomial,
     expressed as X^16 + X^12 + X^5 + 1 */

  int crc = (int) 0xffff;
  int index;
  char b;

  for( index=0; index<size; ++index )
    {
      crc ^= (((int) byt[index]) << 8);
      for( b=0; b<8; ++b )
	{
	  if( crc & (int) 0x8000 )
	    crc = (crc << 1) ^ (int) 0x1021;
	  else
	    crc = (crc << 1);
	}
    }
  return crc;
}
