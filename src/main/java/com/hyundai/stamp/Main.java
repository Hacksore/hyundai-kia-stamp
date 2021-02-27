package com.hyundai.stamp;
import stationdm.euapi.header.RemoteHttpHeader;

public class Main {

	public static void main(String[] args) {
		System.out.println(System.getProperty("java.library.path"));

		if(args.length == 0) {
			System.out.println("Please give an input");
			return;
		}
 		
		System.out.println("Calling stamp for: " + args[0]);
		String stamp = RemoteHttpHeader.getStamp(args[0]);
		System.out.println(stamp);
	}
}