package foamj;


public class Clos extends AbstractValue {
    private Env env;
    private Fn fn;
    private Word info;
    
    public Clos(Env env, Fn fn) {
    	this.env = env;
    	this.fn = fn;
    }

    public Value call(Value ... vals) {
		return fn.ocall(env, vals);
    }
    
    public Fn getProg() {
    	return fn;
    }

    public Env getEnv() {
    	return env;
    }
    
    public void setProg(Fn fn) {
    	this.fn = fn;
    }
    
    public void setEnv(Env env) {
    	this.env = env;
    }

	public void setInfo(Word info) {
		this.info = info;
	}

	public Word getInfo() {
		return info;
	}
}