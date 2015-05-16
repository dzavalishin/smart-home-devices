package ru.dz.openhab;

import java.io.IOException;

public interface IPushOpenHab {

	public String getOpenHABHostName();
	public void sendValue(String name, String value) throws IOException;
	
	
}
