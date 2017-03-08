package foamj;

public class MultiRecord extends Record {

    public MultiRecord(Format fmt) {
        super(fmt);
    }

    public MultiRecord toMulti() {
        return this;
    }

}