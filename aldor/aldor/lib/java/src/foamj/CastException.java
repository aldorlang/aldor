package foamj;

@SuppressWarnings("serial")
public class CastException extends RuntimeException {

	public CastException(AbstractValue abstractValue) {
		this("Casting: " + abstractValue.getClass().getName());
	}

	public CastException(String string) {
		super(string);
	}
}
