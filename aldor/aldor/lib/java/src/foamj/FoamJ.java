package foamj;



public class FoamJ {

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
	static public class SInt extends AbstractValue implements Value, Word {
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

	static public class Bool extends AbstractValue implements Value, Word {
		private boolean value;

		public Bool(boolean b) {
			this.value = b;
		}

		public boolean toBool() { return value; }

		@Override
		public Value toValue() {
			return this;
		}
	}

}
