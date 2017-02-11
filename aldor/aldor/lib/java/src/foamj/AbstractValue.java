package foamj;

import java.math.BigInteger;

public abstract class AbstractValue implements Value {
    public Word asWord() {
        throw new CastException(this);
    }

    public int toSInt() {
        throw new CastException(this);
    }

    public short toHInt() {
        throw new CastException(this);
    }

    public BigInteger toBInt() {
        throw new CastException(this);
    }

    public double toDFlo() {
        throw new CastException(this);
    }

    public float toSFlo() {
        throw new CastException(this);
    }

    public Object toArray() {
        throw new CastException(this);
    }

    public Record toRecord() {
        throw new CastException(this);
    }

    public Clos toClos() {
        throw new CastException(this);
    }

    public boolean toBool() {
        throw new CastException(this);
    }

    public MultiRecord toMulti() {
        throw new CastException(this);
    }

    public char toChar() {
        throw new CastException(this);
    }

    public Object toPtr() {
        throw new CastException(this);
    }

    public Env toEnv() {
        throw new CastException(this);
    }

    public byte toByte() {
        throw new CastException(this);
    }
}