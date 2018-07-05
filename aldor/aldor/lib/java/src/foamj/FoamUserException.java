package foamj;

public class FoamUserException extends RuntimeException
{
    Word theException;
    private int tag;

    public FoamUserException(int tag, Word w) {
	this.tag = tag;
	this.theException = w;
    }
}
