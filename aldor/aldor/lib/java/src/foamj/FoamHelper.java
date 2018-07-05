package foamj;

public class FoamHelper {
    static private final ThreadLocal<FoamContext> contextForThread = new ThreadLocal();

    static public void setContext(FoamContext context) {
	contextForThread.set(context);
    }

    static public <T extends FoamClass> T instanceForClass(Class<T> clss) {
	return contextForThread.get().instanceForClass(clss);
    }
}
