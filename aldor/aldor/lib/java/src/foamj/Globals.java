package foamj;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class Globals {

	/*
	 * :: Globals 
	 */
	static private final Map<String, Value> globals = new ConcurrentHashMap<String, Value>();
	
	static public Value getGlobal(String s) {
		return globals.get(s);
	}

	static public void setGlobal(String s, Value v) {
		globals.put(s, v);
	}

}
