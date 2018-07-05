package foamj;

public class JavaException extends RuntimeException
{
    public JavaException(Exception e) {
	super(e);
    }
}
