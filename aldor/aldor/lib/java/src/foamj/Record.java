package foamj;


public class Record extends AbstractValue implements Value {
	private Value[] arr;

	public Record(Format fmt) {
		arr = new Value[fmt.size()];
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
	
}