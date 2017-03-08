package foamj;

/**
 * Represents a foam function.
 * The value is generally a hash code; we only ever use PRef 0.
 *
 * @author pab
 */
public abstract class Fn {
    private int val;
    private String name;

    public Fn(String name) {
        this.name = name;
    }

    abstract public Value ocall(Env env, Value... vals);

    public void setInfo(int idx, int val) {
        this.val = val;
    }

    public int getInfo(int idx) {
        return val;
    }

    public String getName() {
        return name;
    }

    public String toString() {
        return "Fn-" + name;
    }
}