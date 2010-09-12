package foamj;


/**
 * Generic type used where we have to pass random objects around
 */
public interface Value {
	Word   asWord();
	int    toSInt();
	double toDouble();
	Object toArray();
	Record toRecord();
	Clos   toClos();
	MultiRecord toMulti();
}