package foamj;

import java.math.BigInteger;

public class Foam {
    public final static int RTE = 1;
    public final static int PlatformOS = 1;

	public static void fputc(Word cw, Word w) {
		System.out.println("FPuts: " + cw + " " + w);
	}

	public static void fputs(Word s, Word w) {
		System.out.println("FPuts: " + s + " " + w);
		//w.append(new String((char[]) s.toArray()));
	}
    
    // string, int, int, stream -> int
	public static Word fputss(Word w1, Word w2, Word w3, Word w4) {
	    char[] arr = (char[]) w1.toArray();
	    int start = w2.toSInt();
	    int limit = w3.toSInt();
	    if (limit == -1) {
		System.out.println(new String(arr).substring(start));
		return Word.U.fromSInt(arr.length - start);
	    }
	    else {
		System.out.println(new String(arr, start, limit-start));
		return Word.U.fromSInt(limit-start);
	    }
	}

	public static Word stdoutFile() {
		//return System.out;
	    return Word.U.fromSInt(999);
	}
	
	public static Word stderrFile() {
		//return System.out;
	    return Word.U.fromSInt(999);
	}

	public static Word stdinFile() {
		//return System.out;
	    return Word.U.fromSInt(999);
	}

	public static Word fopen(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word formatSInt(int a) {
		return Word.U.fromArray(("" + a+'\0').toCharArray());
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
	public static int arrToSInt(Object o) { 
	    char[] arr  = (char[]) o;
	    return Integer.parseInt(new String(arr));
	}
	public static BigInteger arrToBInt(Object o) { throw new RuntimeException(); }
}