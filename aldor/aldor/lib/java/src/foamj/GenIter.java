package foamj;

public class GenIter extends AbstractValue implements Word, Value {
    private Env env;
    private Env env0;
    private GFn prog;
    private Record state;
    private Value value;
    private int step;

    public GenIter(Env env, GFn prog, Format fmt) {
	this.env = env;
	this.prog = prog;
	this.step = 0;
	this.state = new Record(fmt);
	if (prog.envFormat() != null) {
	    env0 = new Env(new EnvRecord(prog.envFormat()), env);
	}
    }

    public void step() {
	MultiRecord r = (MultiRecord) prog.ocall(env, env0, Value.U.fromSInt(step), state.toValue());
	this.step = r.getSInt(0, "step");
	this.value = r.getField(1, "retVal");
    }

    public boolean isDone() {
	return step == -1;
    }

    public Word value() {
	return Word.U.fromValue(value);
    }

    public static GenIter fromValue(Value v) {
        return (GenIter) v;
    }

    @Override
    public Value toValue() {
        return this;
    }

}
