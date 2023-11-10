package foamj;


public class Record extends AbstractValue implements Value, Word {
    private Value[] arr;
    private int id;
    private static int count;

    public Record(Format fmt) {
        arr = new Value[fmt.size()];
        id = count++;
    }

    public Value getField(int idx, String name) {
        return arr[idx];
    }

    public Word getWord(int idx, String name) {
        return arr[idx].asWord();
    }

    public int getSInt(int idx, String name) {
        return arr[idx].toSInt();
    }

    public void setField(int idx, String name, Value val) {
        arr[idx] = val;
    }

    @Override
    public Value toValue() {
        return this;
    }

    @Override
    public Record toRecord() {
        return this;
    }

    public Word asWord() {
        return this;
    }

    public String toString() {
        return "[R: " + id + "]";
    }

    public Object toPtr() {
	return this;
    }
}
