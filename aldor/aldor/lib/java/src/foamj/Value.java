package foamj;


import foamj.FoamJ.Array;
import foamj.FoamJ.SInt;
import foamj.FoamJ.Bool;


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
	
	public class U {
		public static Value fromBool(boolean b) { return new Bool(b); }
		public static Value fromSInt(int x) { return new SInt(x); }
		public static Value fromArray(Object x) { return new Array(x); }
	}
}