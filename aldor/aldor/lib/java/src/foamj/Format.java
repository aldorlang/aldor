package foamj;

/**
 * Represents a Foam DDecl.
 * <p>
 * Should really have some type information, but
 * at the moment is just a size.
 *
 * @author pab
 */
public class Format {
    private int size;

    public Format(int sz) {
        this.size = sz;
    }

    int size() {
        return size;
    }
}