package foamj;


import java.math.BigInteger;
import foamj.FoamJ.Array;
import foamj.FoamJ.SInt;
import foamj.FoamJ.BInt;
import foamj.FoamJ.Bool;
import foamj.FoamJ.Ptr;


/**
 * Generic type used where we have to pass random objects around
 */
public interface Value {
	Word   asWord();
	int    toSInt();
	double toDouble();
	Object toArray();
	Record toRecord();
	Clos   toClos();
	MultiRecord toMulti();
        boolean toBool();
    char toChar();
    Object toPtr();
    Env toEnv();
    
	public class U {
		public static Value fromBool(boolean b) { return new Bool(b); }
		public static Value fromSInt(int x) { return new SInt(x); }
		public static Value fromArray(Object x) { return new Array(x); }
		public static Value fromBInt(BigInteger x) { return new BInt(x); }
		public static Value fromPtr(Object o) { return new Ptr(o); }
	}
}