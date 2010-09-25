package foamj;


public abstract class AbstractValue implements Value {
	public Word asWord() { throw new CastException(); }
	public int  toSInt() { throw new CastException(); }
	public double toDouble() { throw new CastException(); }
	public Object  toArray() { throw new CastException(); }
	public Record toRecord() { throw new CastException(); }
	public Clos   toClos() { throw new CastException(); }
	public boolean toBool() { throw new CastException(); }
	public MultiRecord toMulti() { throw new CastException(); }
    public char toChar() { throw new CastException(); }
    public Object toPtr() { throw new CastException(); }
    public Env toEnv() { throw new CastException(); }
}