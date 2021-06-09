package com.hyundai.stamp;
import stationdm.euapi.header.RemoteHttpHeader;

public class Main {

	public static int MAX_STAMPS = 1000;
	public static void main(String[] args) {
		if(args.length == 0) {
			System.out.println("Please give an input");
			return;
		}

		if(args.length != 2) {
			return;
		}

		String command = args[0];
		String clientId = args[1];

		if (command.equalsIgnoreCase("single")) {
			String stamp = RemoteHttpHeader.getStamp(clientId);
			System.out.println(stamp);
		}

		if (command.equalsIgnoreCase("list")) {
			System.out.println("Generating 100k stamps...");
			for(int i=0; i < MAX_STAMPS; i++) {
				int unixTime = (int) (System.currentTimeMillis() / 1000L) + i;
				String stamp = RemoteHttpHeader.getStamp(clientId + ":" + unixTime);
				System.out.println(stamp);
			}
		}
	}
}