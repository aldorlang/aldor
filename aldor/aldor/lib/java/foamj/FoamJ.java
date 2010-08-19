
public class FoamJ {

	class Closure {
		Env env;
		Fn fn;

		Value call(Value[] vals) {
			return fn.ocall(env, vals);
		}
	}

	interface Fn {
		Value ocall(Env env, Value[] vals);
	}

	class EnvRecord extends Record {

		EnvRecord(Format fmt) {
			super(fmt);
		}};
	
	class Env {
		EnvRecord thisLvl;
		Env parent;
		
		Env(EnvRecord lvl, Env parent) {
			this.thisLvl = lvl;
		}
		
	}
	
	abstract class FnWxWWW implements Fn {
		abstract Word call(Env env, Word b, Word c);

		public Value ocall(Env env, Value[] vals) {
			Word r = call(env, vals[0].asWord(), vals[1].asWord());
			return r.toValue();
		}
	}

	/**
	 * Array type - rely on casting to retract to base type
	 * @author pab
	 *
	 */
	static class Array extends AbstractValue implements Value, Word {
		private Object arr;
		Array(Object arr) {
			this.arr = arr;
		}	

		public Object toArray() {
			return arr;
		}

		@Override
		public Value toValue() { return this; }
	}

	class Record extends AbstractValue implements Value {
		Value[] arr;
    
		Record(Format fmt) {
			arr = new Value[fmt.size()];
		}

		Word getWord(int idx, String name) {
			return arr[idx].asWord();
		}

		int getSInt(int idx, String name) {
			return arr[idx].toSInt();
		}
		
	}

	class Format {
		int size;
    
		int size() { return size; }
	}

	/**
	 * Generic type used where we have to pass random objects around
	 */
	interface Value {
		Word   asWord();
		int    toSInt();
		double toDouble();
		Object toArray();
		Record toRecord();
	}

	static abstract class AbstractValue implements Value {
		public Word asWord() { throw new CastException(); }
		public int  toSInt() { throw new CastException(); }
		public double toDouble() { throw new CastException(); }
		public Object  toArray() { throw new CastException(); }
		public Record toRecord() { throw new CastException(); }
	}

	@SuppressWarnings("serial")
	static class CastException extends RuntimeException {
	}
	
	/**
	 * Implementation of Foam Word type.
	 */
	interface Word {
		Word  asWord();
		Value toValue();
		int   toSInt();
		class U {
			Word fromSInt(int x) { return new SInt(x); }
			Word fromArray(Object x) { return new Array(x); }
		}
	}

	/**
	 * a.
	 * T1 = (Add (Cast SInt x) 1)
	 * -->
	 * t1 = x.toInteger() + 1;
	 * ..
	 * T2 = (Cast Word (Add (Cast SInt x) 1))
	 * -->
	 * t1 = Word.U.fromSInt(x.toSInt() + 1);
	 */
	static class SInt extends AbstractValue implements Value, Word {
		private int value;

		public SInt(int x) {
			this.value = x;
		}

		public int  toSInt() { return value; }

		@Override
		public Value toValue() {
			return this;
		}
	}

//x = ccall(a, b, c);
//x = ccallWxWWW(a, b, c);

	static Word callWxWWW(Closure a, Word b, Word c) {
		Value[] args = {b.toValue(),c.toValue()};
		Value r = a.call(args);
		return r.asWord();
	}

//clos c = new Closure(fn, env);
}
