package foamj;


public interface Fn {
	Value ocall(Env env, Value... vals);
}