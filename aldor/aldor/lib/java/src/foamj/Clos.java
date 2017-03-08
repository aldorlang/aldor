package foamj;

public class Clos extends AbstractValue implements Word, Value {
    private Env env;
    private Fn fn;
    private Word info;

    public Clos(Env env, Fn fn) {
        this.env = env;
        this.fn = fn;
    }

    public Value call(Value... vals) {
        //System.out.println("(Calling: " + fn.getName() + " " + Arrays.asList(vals));
        Value v = fn.ocall(env, vals);
        //System.out.println(" Return: " + v + ")");

        return v;
    }

    public static Clos fromValue(Value v) {
        return (Clos) v;
    }

    public String toString() {
        return "{Clos: " + fn + ", " + env.getId() + "}";
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

    public Clos toClos() {
        return this;
    }

    public Word asWord() {
        return this;
    }

    @Override
    public Value toValue() {
        return this;
    }
}
