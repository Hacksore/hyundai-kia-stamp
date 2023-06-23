package stationdm.euapi.header;
import java.util.Base64;

public class RemoteHttpHeader {
	static {
		System.loadLibrary("native-lib");
	}

	public static String getStamp(String payload) {
		return Base64.getEncoder().encodeToString(stringFromJNI(payload));
	}

	public static native byte[] stringFromJNI(String str);
}
