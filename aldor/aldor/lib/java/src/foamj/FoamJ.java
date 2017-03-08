package foamj;

import java.math.BigInteger;

public class FoamJ {

    /**
     * Array type - rely on casting to retract to base type
     *
     * @author pab
     */
    static class Array extends AbstractValue implements Value, Word {
        private Object arr;

        Array(Object arr) {
            this.arr = arr;
        }

        public Object toArray() {
            return arr;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }

        public String toString() {
            return "A" + arr.toString() + "]";
        }

    }

    /**
     * a.
     * T1 = (Add (Cast SInt x) 1)
     * -->
     * t1 = x.toInteger() + 1;
     * ..
     * T2 = (Cast Word (Add (Cast SInt x) 1))
     * -->
     * t1 = Word.U.fromSInt(x.toSInt() + 1);
     */
    static public class SInt extends AbstractValue implements Value, Word {
        private int value;

        public SInt(int x) {
            this.value = x;
        }

        public int toSInt() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }

        public String toString() {
            return "" + value;
        }
    }

    static public class HInt extends AbstractValue implements Value, Word {
        private short value;

        public HInt(short x) {
            this.value = x;
        }

        public short toHInt() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }

        public String toString() {
            return "" + value;
        }
    }

    static public class SFlo extends AbstractValue implements Value, Word {
        private float value;

        public SFlo(float x) {
            this.value = x;
        }

        public float toSFlo() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }

        public String toString() {
            return "" + value;
        }
    }

    static public class DFlo extends AbstractValue implements Value, Word {
        private double value;

        public DFlo(double x) {
            this.value = x;
        }

        public double toDFlo() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }

        public String toString() {
            return "" + value;
        }
    }

    static public class BInt extends AbstractValue implements Value, Word {
        private BigInteger value;

        public BInt(BigInteger x) {
            this.value = x;
        }

        public BigInteger toBInt() {
            return value;
        }

        public Word asWord() {
            return this;
        }

        @Override
        public Value toValue() {
            return this;
        }
    }

    static public class Bool extends AbstractValue implements Value, Word {
        private boolean value;

        public Bool(boolean b) {
            this.value = b;
        }

        public boolean toBool() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }

        public String toString() {
            return "" + value;
        }
    }

    static public class Char extends AbstractValue implements Value, Word {
        private char value;

        public Char(char b) {
            this.value = b;
        }

        public char toChar() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }
    }


    static public class Ptr extends AbstractValue implements Value, Word {
        private Object value;

        public Ptr(Object b) {
            this.value = b;
        }

        public Object toPtr() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }
    }

    static public class Byte extends AbstractValue implements Value, Word {
        private byte value;

        public Byte(byte b) {
            this.value = b;
        }

        public byte toByte() {
            return value;
        }

        @Override
        public Value toValue() {
            return this;
        }

        public Word asWord() {
            return this;
        }
    }

}
