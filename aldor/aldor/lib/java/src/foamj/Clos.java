package foamj;


public class Clos extends AbstractValue {
    public Env env;
    public Fn fn;

    public Clos(Env env, Fn fn) {
	this.env = env;
	this.fn = fn;
    }

    public Value call(Value ... vals) {
		return fn.ocall(env, vals);
    }
    
}