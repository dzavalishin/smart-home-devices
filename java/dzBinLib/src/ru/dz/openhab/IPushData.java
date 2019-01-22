package ru.dz.openhab;

import java.io.IOException;


public interface IPushData {


	public void sendValue(String name, String value) throws IOException;
	public void sendValue(String name, double d) throws IOException; 

	
	public void setInputItemName( int input, String itemName );

	public void setDefaultItemNames();

	
}
