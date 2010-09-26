package foamj;

import java.math.BigInteger;

public class Foam {
	public static void fputc(Word cw, Word w) {
		//w.append(cw.toChar());
		
	}

	public static void fputs(Word s, Word w) {
		//w.append(new String((char[]) s.toArray()));
	}

	public static Word fputss(Word w1, Word w2, Word w3, Word w4) {
		throw new RuntimeException();
	}

	public static Word stdoutFile() {
		//return System.out;
		throw new RuntimeException();
	}
	
	public static Word stderrFile() {
		//return System.out;
		throw new RuntimeException();
	}

	public static Word stdinFile() {
		//return System.out;
		throw new RuntimeException();
	}

	public static Word fopen(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word formatSInt(int a) {
		return Word.U.fromArray(("" + a).toCharArray());
	}

	public static void halt(int r) {
		throw new HaltException(r);
	}

    public static int ptrToSInt(Object o) {
		throw new RuntimeException();
	}

    public static int sintToPtr(int o) {
		throw new RuntimeException("wheee");
	}


	public static Word fiStrHash(Word w) {
		return Word.U.fromSInt(w.hashCode());
	}

	public static Word osFnameUnparse(Word w1, Word w2, Word w3) {
		throw new RuntimeException();
	}

	public static Word osFnameUnparseSize(Word w1, Word w2) {
		throw new RuntimeException();
	}
	
	public static Word osFnameParseSize(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word osFnameParse(Word w1, Word w2, Word w3, Word w4) {
		throw new RuntimeException();
	}

	public static void fiRaiseException(Word w) {
		throw new RuntimeException(w.toString());
	}
	
	public static float arrToSFlo(Object o) { throw new RuntimeException(); }
	public static double arrToDFlo(Object o) { throw new RuntimeException(); }
	public static int arrToSInt(Object o) { throw new RuntimeException(); }
	public static BigInteger arrToBInt(Object o) { throw new RuntimeException(); }
}