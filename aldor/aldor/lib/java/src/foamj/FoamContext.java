package foamj;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

public class FoamContext {
	
	public void startFoam(FoamClass c) {
		c.run();
	}
	
	public FoamClass loadClass(String name) {
		return null;
	}
	
	@SuppressWarnings("unchecked")
	public Clos createLoadFn(final String name) {
		Fn loader = new Fn("constructor-"+name) {
			public Value ocall(Env env, Value... vals) {
				Class<FoamClass> c;
				try {
					c = (Class<FoamClass>) ClassLoader.getSystemClassLoader().loadClass(name);
					Constructor<FoamClass> cons = c.getConstructor(FoamContext.class);
					FoamClass fc = cons.newInstance(FoamContext.this);
					fc.run();
				} catch (ClassNotFoundException e) {
					throw new RuntimeException(e);
				} catch (SecurityException e) {
					throw new RuntimeException(e);
				} catch (NoSuchMethodException e) {
					throw new RuntimeException(e);
				} catch (IllegalArgumentException e) {
					throw new RuntimeException(e);
				} catch (InstantiationException e) {
					throw new RuntimeException(e);
				} catch (IllegalAccessException e) {
					throw new RuntimeException(e);
				} catch (InvocationTargetException e) {
					throw new RuntimeException(e);
				}
				return null;
			}
			
		};
		return new Clos(null, loader);
	}
	
	
}
