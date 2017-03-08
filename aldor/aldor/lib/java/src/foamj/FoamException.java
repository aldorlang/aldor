package foamj;

@SuppressWarnings("serial")
public class FoamException extends RuntimeException {
    private int status;

    public FoamException(int status) {
        this.status = status;
    }

    public int getStatus() {
        return status;
    }
}
