package ru.dz.binutil;

public class BitTools {

	public static int makeMask(int nBit)
	{
		if((nBit > 32) || (nBit < 0 ))
			throw new ArrayIndexOutOfBoundsException(nBit);
		
		if(nBit == 32)
			return 0xFFFFFFFF;
		
		int mask = 0x7FFFFFFF;
		
		mask >>= (31-nBit);
		return mask;
	}
	
}
