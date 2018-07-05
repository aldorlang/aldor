package foamj;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.concurrent.ConcurrentHashMap;

public class FoamContext {
    ConcurrentHashMap<String, Clos> loadFns = new ConcurrentHashMap<>();
    ConcurrentHashMap<Class<? extends FoamClass>, FoamClass> classInstances = new ConcurrentHashMap<>();

    public void startFoam(FoamClass c, String[] args) {
        Word[] mainArgv = new Word[1];
        mainArgv[0] = Word.U.fromArray(literalCharArray(c.getClass().getName()));
        Globals.setGlobal("mainArgc", Word.U.fromSInt(1).toValue());
        Globals.setGlobal("mainArgv", Word.U.fromArray(mainArgv).toValue());
        c.run();
    }

    private static char[] literalCharArray(String s) {
        char[] arr = new char[s.length() + 1];
        for (int i = 0; i < s.length(); i++)
            arr[i] = s.charAt(i);
        arr[s.length()] = '\0';

        return arr;
    }

    public <T extends FoamClass> T instanceForClass(Class<T> clss) {
	return (T) classInstances.get(clss);
    }

    @SuppressWarnings("unchecked")
    public Clos createLoadFn(final String name) {
        if (loadFns.get(name) != null)
            return loadFns.get(name);
        Fn loader = new Fn("constructor-" + name) {
            boolean called = false;

            public Value ocall(Env env, Value... vals) {
                if (called)
                    return null;
                called = true;
                Class<FoamClass> c;
                try {
		    c = (Class<FoamClass>) getClass().getClassLoader().loadClass("aldorcode." + name);
                    Constructor<FoamClass> cons = c.getConstructor(FoamContext.class);
                    FoamClass fc = cons.newInstance(FoamContext.this);
		    classInstances.put(c, fc);
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
        Clos clos = new Clos(null, loader);
        loadFns.put(name, clos);
        return clos;
    }
}
