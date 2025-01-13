package foamj;

public class Gener extends AbstractValue implements Word, Value {
    private Env env;
    private GFn prog;
    private Format fmt;

    public Gener(Env env, GFn prog, Format fmt) {
	this.env = env;
	this.prog = prog;
	this.fmt = fmt;
    }

    public static Gener fromValue(Value v) {
        return (Gener) v;
    }

    @Override
    public Value toValue() {
        return this;
    }

    @Override
    public Word asWord() {
	return this;
    }

    public GenIter start() {
	return new GenIter(env, prog, fmt);
    }

}
