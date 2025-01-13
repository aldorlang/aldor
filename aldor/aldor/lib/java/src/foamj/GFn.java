package foamj;

/**
 * Represents a foam coroutine.
 * @author pab
 */
public abstract class GFn {
    private String name;
    private Format envFormat;

    public GFn(String name) {
        this.name = name;
	this.envFormat = null;
    }

    public GFn(String name, Format envFormat ) {
        this.name = name;
	this.envFormat = envFormat;
    }

    public String getName() {
        return name;
    }

    public Format envFormat() {
	return envFormat;
    }

    public String toString() {
        return "GFn-" + name;
    }

    abstract public Value ocall(Env env, Env env0, Value... vals);

}
