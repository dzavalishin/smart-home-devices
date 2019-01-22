package ru.dz.openhab;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public abstract class AbstractPushData implements IPushData {

	protected static final Map<Integer,String> items = new HashMap<Integer,String>();
	protected String chargeItemName;

	/**
	 * Map CCU825 input to named OpenHAB item. 
	 * @param input CCU825 input number, 0-15
	 * @param itemName OpehNAB item to translate data to
	 */
	public void setInputItemName( int input, String itemName )
	{
		items.put(input, itemName);
	}

	public String getChargeItemName() {		return chargeItemName;	}

	public void setChargeItemName(String chargeItemName) {		this.chargeItemName = chargeItemName;	}

	public void sendValue(String name, double d) throws IOException 
	{
		String ds = Double.toString( d );
		
		ds = ds.replace(',', '.');
		
		System.out.println(name + " = " + ds);
		
		sendValue(name, ds);		
	}
	

}
