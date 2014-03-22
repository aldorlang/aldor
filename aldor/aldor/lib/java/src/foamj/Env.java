package foamj;

public class Env extends AbstractValue implements Value {
	private EnvRecord thisLvl;
	private Env parent;
	private Word info;
	private int id;
	private static int count = 0;

	public Env(EnvRecord lvl, Env parent) {
		this.thisLvl = lvl;
		this.parent = parent;
		this.id = count++;
	}
	
    public Env nthParent(int idx) {
    	Env e = this;
    	while (idx != 0) {
    		e = e.parent;
    		idx--;
    	}
    	return e;
    }
    public Env toEnv() { return this; }

    public EnvRecord level() {
    	return thisLvl;
    }
    
    public Env parent() {
    	return parent;
    }

    public void ensure() {
		if (info != null) {
			((Clos) info).call();
		}
    }
	public void setInfo(Word info) {
		this.info = info;
	}

	public Word getInfo() {
		return info;
	}

	public String getId() {
		return ""+id;
	}

}
