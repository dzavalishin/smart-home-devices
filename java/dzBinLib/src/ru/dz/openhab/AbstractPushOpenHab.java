package ru.dz.openhab;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.logging.Logger;

public class AbstractPushOpenHab implements IPushOpenHab 
{
	protected final static Logger log = Logger.getLogger(AbstractPushOpenHab.class.getName());

	private final String openHABHostName;
	
	public AbstractPushOpenHab( String openHABHostName ) {
		this.openHABHostName = openHABHostName;
	}
	

	public String getOpenHABHostName() {
		return openHABHostName;
	}
	
	
	/**
	 * Not supposed to be used from outside. Public for unit test. 
	 * @param name Item name
	 * @param value Item value to send
	 * @throws IOException 
	 */
	public void sendValue(String name, String value) throws IOException {
		try {
			URL url = makeUrl(name,value);
			callUrl(url);
		} catch(IOException e)
		{
			log.severe("IO Error: "+e.getMessage());
			throw e;
		}
	}


	private void callUrl(URL url) throws IOException 
	{
		URLConnection yc = url.openConnection();

		BufferedReader in = new BufferedReader(
				new InputStreamReader(yc.getInputStream()));

		String inputLine;

		while ((inputLine = in.readLine()) != null) 
		{
			log.finest("callUrl="+inputLine);
			System.out.println(inputLine);
		}
		
		in.close();
	}


	private URL makeUrl(String name, String value) throws MalformedURLException {
		return new URL("http", openHABHostName, 8080, String.format("/CMD?%s=%s ", name, value ) );
		//return new URL( String.format("http://%s:8080/CMD?%s=%s ", openHABHostName, name, value ) );
	}


	public void sendValue(String name, double d) throws IOException 
	{
		String ds = Double.toString( d );
		
		ds = ds.replace(',', '.');
		
		System.out.println(name + " = " + ds);
		
		sendValue(name, ds);		
	}
	
	
}
