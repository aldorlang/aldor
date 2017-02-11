package foamj;

import java.io.PrintStream;
import java.lang.Math;
import java.math.BigInteger;

public class Foam {
    public final static int RTE = 1;
    public final static int PlatformOS = 1;

    public static void fputc(Word cw, Word w) {
        PrintStream ps = (PrintStream) Word.U.toArray(w);
        char c = (char) cw.toSInt();
        ps.write(c);
    }

    public static Word fgetss(Word w1, Word w2, Word w3, Word w4) {
        throw new RuntimeException();
    }

    public static Word fgetc(Word cw) {
        throw new RuntimeException();
    }

    public static void fputs(Word s, Word w) {
        PrintStream ps = (PrintStream) Word.U.toArray(s);
        char[] arr = (char[]) w.toArray();
        for (int i = 0; i < arr.length - 1; i++) {
            ps.write(arr[i]);
        }
    }

    // string, int, int, stream -> int
    public static Word fputss(Word w1, Word w2, Word w3, Word w4) {
        char[] arr = (char[]) w1.toArray();
        int start = w2.toSInt();
        int limit = w3.toSInt();
        if (limit == -1) {
            System.out.print(new String(arr).substring(start));
            return Word.U.fromSInt(arr.length - 1 - start);
        } else {
            System.out.print(new String(arr, start, limit - start));
            return Word.U.fromSInt(limit - start);
        }
    }

    public static Word stdoutFile() {
        return Word.U.fromArray(System.out);
    }

    public static Word stderrFile() {
        return Word.U.fromArray(System.err);
    }

    public static Word stdinFile() {
        return Word.U.fromArray(System.in);
    }

    public static Word fopen(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word fflush(Word w1) {
        PrintStream ps = (PrintStream) Word.U.toArray(w1);
        ps.flush();
        return w1;
    }

    public static void lungetc(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word fclose(Word w1) {
        throw new RuntimeException();
    }

    public static Word formatBInt(BigInteger a) {
        return Word.U.fromArray(("" + a + '\0').toCharArray());
    }

    public static Word formatSInt(int a) {
        return Word.U.fromArray(("" + a + '\0').toCharArray());
    }

    public static Word formatSFlo(float a) {
        return Word.U.fromArray(("" + a + '\0').toCharArray());
    }

    public static void halt(int r) {
        throw new HaltException(r);
    }

    public static Object throwException(RuntimeException e) {
        throw e;
    }

    public static int ptrToSInt(Object o) {
        return System.identityHashCode(o);
    }

    public static Word sintToPtr(int o) {
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

    public static float arrToSFlo(Object o) {
        char[] arr = (char[]) o;
        return new Float(new String(arr, 0, arr.length - 1));

    }

    public static double arrToDFlo(Object o) {
        char[] arr = (char[]) o;
        return new Double(new String(arr, 0, arr.length - 1));
    }

    public static int arrToSInt(Object o) {
        char[] arr = (char[]) o;
        return Integer.parseInt(new String(arr, 0, arr.length - 1));
    }

    public static BigInteger arrToBInt(Object o) {
        char[] arr = (char[]) o;
        return new BigInteger(new String(arr, 0, arr.length - 1));
    }


    public static Word powf(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word randomSeed() {
        return Word.U.fromSInt(1000);
    }

    public static void stoGc() {
        throw new RuntimeException();
    }

    public static void stoShow() {
        throw new RuntimeException();
    }

    public static Word gcTimer() {
        throw new RuntimeException();
    }

    public static void fiSetDebugVar(Word word) {
        throw new RuntimeException();
    }

    public static void agatSendLong(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static void agatSendChar(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static void agatSendFloat(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static void agatSendDouble(Word w1, double w2) {
        throw new RuntimeException();
    }

    public static Word cerrno() {
        throw new RuntimeException();
    }

    public static Word ftell(Word w) {
        throw new RuntimeException();
    }

    public static Word fseekset(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word fseekend(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word fseekcur(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word mkstemp(Word w1) {
        throw new RuntimeException();
    }

    public static Word lfputc(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word unlink(Word w) {
        throw new RuntimeException();
    }

    public static void system(Object obj) {
        throw new RuntimeException();
    }


    public static double sqrt(double d) {
        return Math.sqrt(d);
    }

    public static double pow(double a, double b) {
        return Math.pow(a, b);
    }

    public static double log(double d) {
        return Math.log(d);
    }

    public static double exp(double d) {
        return Math.exp(d);
    }

    public static double sin(double d) {
        return Math.sin(d);
    }

    public static double cos(double d) {
        return Math.cos(d);
    }

    public static double tan(double d) {
        return Math.tan(d);
    }

    public static double sinh(double d) {
        return Math.sinh(d);
    }

    public static double cosh(double d) {
        return Math.cosh(d);
    }

    public static double tanh(double d) {
        return Math.tanh(d);
    }

    public static double asin(double d) {
        return Math.asin(d);
    }

    public static double acos(double d) {
        return Math.acos(d);
    }

    public static double atan(double d) {
        return Math.atan(d);
    }

    public static double atan2(double a, double b) {
        return Math.atan2(a, b);
    }

}
