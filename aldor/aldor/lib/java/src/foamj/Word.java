package foamj;

import foamj.FoamJ.Array;
import foamj.FoamJ.SInt;

/**
 * Implementation of Foam Word type.
 */
public interface Word {
	Word  asWord();
	Value toValue();
	int   toSInt();

	public class U {
		public static Word fromSInt(int x) { return new SInt(x); }
		public static Word fromArray(Object x) { return new Array(x); }
	}
}