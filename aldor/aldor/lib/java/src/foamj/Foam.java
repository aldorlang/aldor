package foamj;

import java.math.BigInteger;

public class Foam {
    public final static int RTE = 1;
    public final static int PlatformOS = 1;

	public static void fputc(Word cw, Word w) {
		System.out.println("FPuts: " + cw + " " + w);
	}

	public static Word fgetss(Word w1, Word w2, Word w3, Word w4) {
		throw new RuntimeException();
	}

	public static Word fgetc(Word cw) {
		throw new RuntimeException();
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

	public static Word fclose(Word w1) {
		throw new RuntimeException();
	}

	public static Word formatBInt(BigInteger a) {
		return Word.U.fromArray(("" + a+'\0').toCharArray());
	}

	public static Word formatSInt(int a) {
		return Word.U.fromArray(("" + a+'\0').toCharArray());
	}

	public static Word formatSFlo(float a) {
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


	public static BigInteger fiSFloMantissa(float f) {
		throw new RuntimeException();
	}

	public static BigInteger fiSFloExponent(float f) {
		throw new RuntimeException();
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

	public static Word osFnameDirEqual(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word osRun(Word w1) {
		throw new RuntimeException();
	}

	public static Word osRunQuoteArg(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word osRunConcurrent(Word w1, Record r1, Record r2, Record r3) {
		throw new RuntimeException();
	}

	public static Word osCpuTime() {
		throw new RuntimeException();
	}

	public static Word osDate() {
		throw new RuntimeException();
	}

	public static Word osGetEnv(Word w1) {
		throw new RuntimeException();
	}

	public static Word osPutEnv(Word w1) {
		throw new RuntimeException();
	}

	public static Word osPutEnvIsKept() {
		throw new RuntimeException();
	}

	public static Word osCurDirName() {
		throw new RuntimeException();
	}

	public static Word osTmpDirName() {
		throw new RuntimeException();
	}

	public static Word osSubdir(Word w1, Word w2, Word w3) {
		throw new RuntimeException();
	}

	public static Word osSubdirLength(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word osFnameTempSeed() {
		throw new RuntimeException();
	}

	public static Word osFnameTempDir(Word w1) {
		throw new RuntimeException();
	}

	public static Word osIsInteractive(Word w1) {
		throw new RuntimeException();
	}

	public static Word osFileRemove(Word w1) {
		throw new RuntimeException();
	}

	public static Word osFileRename(Word w1, Word w2) {
		throw new RuntimeException();
	}

	public static Word osFileIsThere(Word w1) {
		throw new RuntimeException();
	}

	public static Word osFileHash(Word w1) {
		throw new RuntimeException();
	}

	public static Word osFileSize(Word w1) {
		throw new RuntimeException();
	}

	public static Word osDirIsThere(Word w1) {
		throw new RuntimeException();
	}

	public static Word osDirSwap(Word w1, Word w2, Word w3) {
		throw new RuntimeException();
	}

	public static Word osIncludePath() {
		throw new RuntimeException();
	}

	public static Word osLibraryPath() {
		throw new RuntimeException();
	}

	public static Word osExecutePath() {
		throw new RuntimeException();
	}

	public static Word osPathLength(Word w1) {
		throw new RuntimeException();
	}

	public static Word osPathParse(Word w1, Word w2, Word w3) {
		throw new RuntimeException();
	}

	public static Word osSetFaultHandler(Word w1) {
		throw new RuntimeException();
	}

	public static Word osSetLimitHandler(Word w1) {
		throw new RuntimeException();
	}

	public static Word osSetBreakHandler(Word w1) {
		throw new RuntimeException();
	}

	public static Word osSetDangerHandler(Word w1) {
		throw new RuntimeException();
	}

	public static Word osAlloc(Word w1) {
		throw new RuntimeException();
	}

	public static Word osFree(Word w1) {
		throw new RuntimeException();
	}

	public static Word osAllocAlignHint(Word w1) {
		throw new RuntimeException();
	}

	public static Word osAllocShow() {
		throw new RuntimeException();
	}

	public static Word osMemMap(Word w1) {
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
