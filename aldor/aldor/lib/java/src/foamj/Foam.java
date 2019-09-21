package foamj;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.Math;
import java.math.BigInteger;

public class Foam {
    public final static int RTE = 1;
    public final static int PlatformOS = 1;

    public static void fputc(Word cw, Word w) {
        try {
            fputc0(cw, w);
        }
        catch (IOException e) {
            throw new JavaException(e);
        }
    }

    public static void fputc0(Word cw, Word w) throws IOException {
        OutputStream ps = (OutputStream) Word.U.toJavaObj(w);
        char c = (char) cw.toSInt();
        ps.write(c);
    }

    public static Word fgetss(Word w1, Word w2, Word w3, Word w4) {
        throw new RuntimeException();
    }

    public static Word fgetc(Word cw) {
        InputStream instream = Word.U.<InputStream>toJavaObj(cw);
        try {
            return Word.U.fromSInt(instream.read());
        }
        catch (IOException e) {
            throw new JavaException(e);
        }
    }

    public static void fputs(Word s, Word w) {
        try {
            fputs0(s, w);
        }
        catch (IOException e) {
            throw new JavaException(e);
        }
    }

    public static void fputs0(Word s, Word w) throws IOException{
        OutputStream ps = (OutputStream) Word.U.toJavaObj(s);
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
            System.out.print(new String(arr, start, arr.length-1));
            return Word.U.fromSInt(arr.length - 1 - start);
        } else {
            System.out.print(new String(arr, start, limit - start));
            return Word.U.fromSInt(limit - start);
        }
    }

    public static Word stdoutFile() {
        return Word.U.fromJavaObj(System.out);
    }

    public static Word stderrFile() {
        return Word.U.fromJavaObj(System.err);
    }

    public static Word stdinFile() {
        return Word.U.fromJavaObj(System.in);
    }

    public static Word fopen(Word w1, Word w2) {
        try {
            return fopen0(w1, w2);
        }
        catch (IOException e) {
            throw new JavaException(e);
        }
    }

    private static Word fopen0(Word w1, Word w2) throws IOException {
        char[] aname = (char[]) w1.toArray();
        char[] aopts = (char[]) w2.toArray();
        String name = new String(aname, 0, aname.length -1);
        String opts = new String(aopts, 0, aopts.length -1);

        if ("r".equals(opts)) {
            InputStream instream = new FileInputStream(new File(name));
            return Word.U.fromJavaObj(instream);
        }
        else if ("w".equals(opts)) {
            OutputStream outstream = new BufferedOutputStream(new FileOutputStream(new File(name)));
            return Word.U.fromJavaObj(outstream);
        }
        throw new RuntimeException();
    }

    public static Word fflush(Word w1) {
        OutputStream ps = (OutputStream) Word.U.toJavaObj(w1);
        try {
            ps.flush();
        }
        catch (IOException e) {
            throw new JavaException(e);
        }
        return w1;
    }

    public static void lungetc(Word w1, Word w2) {
        throw new RuntimeException();
    }

    public static Word fclose(Word w1) {
        try {
            return fclose0(w1);
        }
        catch (IOException e) {
            throw new JavaException(e);
        }

    }

    public static Word fclose0(Word w1) throws IOException {
        Object o = Word.U.toJavaObj(w1);
        if (o instanceof InputStream) {
            ((InputStream) o).close();
        }
        if (o instanceof OutputStream) {
            ((OutputStream) o).close();
        }
        return Word.U.fromSInt(0);
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
        throw new RuntimeException(stringToJavaString(w));
    }

    public static float arrToSFlo(Object o) {
        char[] arr = (char[]) o;
        return new Float(arrToString(arr));

    }

    public static double arrToDFlo(Object o) {
        char[] arr = (char[]) o;
        return new Double(arrToString(arr));
    }

    public static int arrToSInt(Object o) {
        char[] arr = (char[]) o;
        return Integer.parseInt(arrToString(arr));
    }

    public static BigInteger arrToBInt(Object o) {
        char[] arr = (char[]) o;
        return new BigInteger(arrToString(arr));
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
        try {
            return fseekset0(w1, w2);
        }
        catch (IOException e) {
            throw new JavaException(e);
        }
    }

    private static Word fseekset0(Word w1, Word w2) throws IOException {
        InputStream instream = Word.U.<InputStream>toJavaObj(w1);
        int posn = w2.toSInt();

        instream.read(new byte[posn]);

        return Word.U.fromSInt(0);
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
        fputc(w1, w2);
        return w1;
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

    public static String stringToJavaString(Word w) {
        char[] arr = (char[]) w.toArray();
        return arrToString(arr);
    }

    public static Word javaStringToString(String s) {
        Word arr = Word.U.fromArray(("" + s + "\0").toCharArray());
        return  arr;
    }

    public static String arrToString(char[] arr) {
        String s = new String(arr);
        int idx = s.indexOf("\0");

        if (idx == -1) {
            return s;
        }

        return s.substring(0, idx);
    }
}
