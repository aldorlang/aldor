package foamj;

import foamj.FoamJ.*;
import foamj.FoamJ.Byte;

import java.math.BigInteger;

/**
 * Implementation of Foam Word type.
 */
public interface Word {
    Word asWord();

    Value toValue();

    int toSInt();

    boolean toBool();

    Object toArray();

    <T> T toJavaObj();

    char toChar();

    float toSFlo();

    double toDFlo();

    short toHInt();

    BigInteger toBInt();

    byte toByte();

    Object toPtr();

    public class U {
        static public Object toArray(Word word) {
            if (word == null)
                return null;
            else
                return word.toArray();
        }

        static public Object toPtr(Word word) {
            if (word == null)
                return null;
            else
                return word.toPtr();
        }

	static public <T> T toJavaObj(Word word) {
            if (word == null)
                return null;
            else
                return word.toJavaObj();
        }

        public static Word fromSInt(int x) {
            return new SInt(x);
        }

        public static Word fromBInt(BigInteger x) {
            return new BInt(x);
        }

        public static Word fromHInt(short x) {
            return new HInt(x);
        }

        public static Word fromArray(Object x) {
            return new Array(x);
        }

        public static <T> Word fromJavaObj(T t) {
            return new JavaObj<T>(t);
        }

        public static Word fromBool(boolean b) {
            return new Bool(b);
        }

        public static Word fromChar(char c) {
            return new Char(c);
        }

        public static Word fromByte(byte b) {
            return new Byte(b);
        }

        public static Word fromSFlo(float f) {
            return new SFlo(f);
        }

        public static Word fromDFlo(double d) {
            return new DFlo(d);
        }

        public static Word fromRec(Record r) {
            return r;
        }

        public static Word fromClos(Clos r) {
            return r;
        }

        public static Word fromValue(Value v) {
            if (v == null) return null;
            else return v.asWord();
        }
    }
}
