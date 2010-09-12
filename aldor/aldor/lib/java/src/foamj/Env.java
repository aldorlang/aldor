package foamj;

public class Env {
	private EnvRecord thisLvl;
	private Env parent;
	
	public Env(EnvRecord lvl, Env parent) {
		this.thisLvl = lvl;
	}
	
    public Env nthParent(int idx) {
    	Env e = this;
    	while (idx != 0) {
    		e = e.parent;
    		idx--;
    	}
    	return e;
    }

    public EnvRecord level() {
    	return thisLvl;
    }
    
    public Env parent() {
    	return parent;
    }
    
}