package ru.dz.binutil;

public class BitPrint {

	private BitRange[] format;

	public BitPrint(BitRange [] format) {
		this.format = format;
	}
	
	public String print(byte [] data)
	{
		StringBuilder sb = new StringBuilder();
		
		boolean first = true;
		for(BitRange br : format)
		{
			String val = br.printBit(data);
			if( val == null )
				continue;
			
			if(!first)
				sb.append(", ");
			
			first = false;
			
			sb.append(val);
		}
			
		return sb.toString();
	}
	
}
