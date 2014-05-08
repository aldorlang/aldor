package foamj;

import java.math.BigInteger;

import foamj.FoamJ.Array;
import foamj.FoamJ.SInt;
import foamj.FoamJ.Bool;
import foamj.FoamJ.Char;
import foamj.FoamJ.Byte;
import foamj.FoamJ.SFlo;
import foamj.FoamJ.DFlo;
import foamj.FoamJ.HInt;
import foamj.FoamJ.BInt;

/**
 * Implementation of Foam Word type.
 */
public interface Word {
    Word  asWord();
    Value toValue();
    int   toSInt();
    boolean toBool();
    Object toArray();
    char toChar();
    float toSFlo();
    double toDFlo();
    short toHInt();
    BigInteger toBInt();
    byte toByte();

	
    public class U {
	static public Object toArray(Word word) {
	    if (word == null)
		return null;
	    else
		return word.toArray();
	}

	public static Word fromSInt(int x) { return new SInt(x); }
	public static Word fromBInt(BigInteger x) { return new BInt(x); }
	public static Word fromHInt(short x) { return new HInt(x); }
	public static Word fromArray(Object x) { return new Array(x); }
	public static Word fromBool(boolean b) { return new Bool(b); }
	public static Word fromChar(char c) { return new Char(c); }
	public static Word fromByte(byte b) { return new Byte(b); }
	public static Word fromSFlo(float f) { return new SFlo(f); }
	public static Word fromDFlo(double d) { return new DFlo(d); }
	public static Word fromRec(Record r) { return r; }
	public static Word fromClos(Clos r) { return r; }
	public static Word fromValue(Value v) {
		if (v == null) return null;
		else return v.asWord();
	}
    }
}
