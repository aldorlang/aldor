package foamj;

@SuppressWarnings("serial")
public class HaltException extends FoamException {

    public HaltException(int status) {
        super(status);
    }

}
