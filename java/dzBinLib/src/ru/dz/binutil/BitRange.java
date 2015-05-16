package ru.dz.binutil;

public class BitRange implements BitFormatter {

	private int byteOffset;
	private int bitOffset;
	private int nBit;
	private String name;
	private String nameOff = null;

	/**
	 * Define named bit range to print.
	 * <p>
	 * Used to extract and print bit or bit range.
	 * 
	 * @param byteOffset Number of byte this bit is in, counting from 0.
	 * @param bitOffset Starting bit of bit range, 0-7.
	 * @param name Text to print bit as
	 * @param nBit Number of bits in range. If not 1, range will be printed as integer.
	 */
	public BitRange(int byteOffset, int bitOffset, String name, int nBit ) {
		this.byteOffset = byteOffset;
		this.bitOffset = bitOffset;
		this.nBit = nBit;
		this.name = name;
	}

	/**
	 * Define named bit range to print.
	 * <p>
	 * Used to extract and print bit or bit range. 
	 * 
	 * <li>byteOffset supposed to be 0.
	 * 
	 * @param bitOffset Starting bit of bit range, 0-7.
	 * @param name Text to print bit as
	 * @param nBit Number of bits in range. If not 1, range will be printed as integer.
	 */
	public BitRange(int bitOffset, String name, int nBit) {
		this.byteOffset = 0;
		this.bitOffset = bitOffset;
		this.nBit = nBit;
		this.name = name;
	}
	
	/**
	 * Define named bit range to print.
	 * <p>
	 * Used to extract and print bit or bit range.
	 * 
	 * <li>byteOffset supposed to be 0.
	 * <li>nBit supposed to be 1.
	 * 
	 * @param bitOffset Starting bit of bit range, 0-7.
	 * @param name Text to print bit as
	 */
	public BitRange(int bitOffset, String name) {
		this.byteOffset = 0;
		this.bitOffset = bitOffset;
		this.nBit = 1;
		this.name = name;
	}

	/**
	 * Define named bit range to print.
	 * <p>
	 * Used to extract and print bit or bit range.
	 * 
	 * <li>nBit supposed to be 1.
	 * 
	 * @param byteOffset Number of byte this bit is in, counting from 0.
	 * @param bitOffset Starting bit of bit range, 0-7.
	 * @param name Text to print bit as
	 */
	public BitRange(int byteOffset, int bitOffset, String name ) {
		this.byteOffset = byteOffset;
		this.bitOffset = bitOffset;
		this.nBit = 1;
		this.name = name;
	}

	/**
	 * Define named bit range to print.
	 * <p>
	 * Used to extract and print bit or bit range.
	 * 
	 * <li>nBit supposed to be 1.
	 * 
	 * @param byteOffset Number of byte this bit is in, counting from 0.
	 * @param bitOffset Starting bit of bit range, 0-7.
	 * @param name Text to print bit as
	 */
	public BitRange(int byteOffset, int bitOffset, String nameOn, String nameOff ) {
		this.byteOffset = byteOffset;
		this.bitOffset = bitOffset;
		this.nBit = 1;
		this.name = nameOn;
		this.nameOff = nameOff;
	}
	
	// TODO enum as value set for field
	
	public String printBit(byte[] data)
	{
		byte b = data[byteOffset];
		
		b >>= bitOffset;
		
		if(nBit ==1)
		{
			return ((b&1) != 0) ? name : nameOff;
		}
		else
		{
			int val = BitTools.makeMask(nBit) & b;
			return String.format("%s=%d", name, val);
		}
	}
	
}
