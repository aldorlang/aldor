package foamj;

import java.math.BigInteger;

public class Math {
    private static final Format divideFormat = new Format(2);
    public static final int ROUND_ZERO = 0;
    public static final int ROUND_NEAREST = 1;
    public static final int ROUND_UP = 2;
    public static final int ROUND_DOWN = 3;
    public static final int ROUND_DONTCARE = 4;

	public static boolean bit(int n, int idx) {
		return (n & (1<<idx)) != 0; 
	}
	
	public static boolean isEven(int n) {
		return (n & 1) == 0; 
	}

	public static boolean isOdd(int n) {
		return (n & 1) == 1; 
	}

	public static int gcd(int i1, int i2) {
		// FIXME: Not even slightly efficient.
	    return BigInteger.valueOf(i1).gcd(BigInteger.valueOf(i2)).intValue();
	}
	
	public static int length(int i1) {
		throw new RuntimeException();
	}

	public static MultiRecord divide(int i1, int i2) {
		MultiRecord result = new MultiRecord(divideFormat);
		result.setField(0, "quo", Value.U.fromSInt(i1 / i2));
		result.setField(1, "rem", Value.U.fromSInt(i1 % i2));
		return result;
	}

    public static int hashCombine(int i1, int i2) {
	long z1  = 0x419ac241;
	long z2  = 0x5577f8e1;
	long zzh = 0x440badfc;
	long zzl = 0x05072367;
	long zz = (zzh << 32) + zzl;
	long h1 = i1 & ((1L<<32)-1);
	long h2 = i2 & ((1L<<32)-1);

	int tmp = (int)(((z1*h1 + z2*h2) * zz) >> 32);
	return tmp & 0x3FFFFFFF;

    }

	public static boolean isZero(BigInteger b)  {
		return b.compareTo(BigInteger.ZERO) == 0;
	}
	public static boolean isNeg(BigInteger b)   {
		return b.compareTo(BigInteger.ZERO) < 0;
	}
	public static boolean isPos(BigInteger b)   {
		return b.compareTo(BigInteger.ZERO) > 0;
	}
	public static boolean isEven(BigInteger b)  {
		return !b.testBit(0);
	}
	public static boolean isOdd(BigInteger b)   {
		return b.testBit(0);
	}

	public static boolean isSingle(BigInteger b) {
		return b.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) <= 0
			&& b.compareTo(BigInteger.valueOf(Integer.MIN_VALUE)) >= 0;
	}

	public static boolean eq(BigInteger b1, BigInteger b2)  {
		return b1.compareTo(b2) == 0;
	}

	public static boolean ne(BigInteger b1, BigInteger b2)  {
		return b1.compareTo(b2) != 0;
	}

	public static boolean lt(BigInteger b1, BigInteger b2) {
		return b1.compareTo(b2) < 0;
	}
	public static boolean le(BigInteger b1, BigInteger b2) {
		return b1.compareTo(b2) <= 0;
	}

	public static BigInteger prev(BigInteger b) {
		return b.subtract(BigInteger.ONE);
	}
	public static BigInteger next(BigInteger b)  {
		return b.add(BigInteger.ONE);
	}
	public static BigInteger plus(BigInteger b1, BigInteger b2)  {
		return b1.add(b2);
	}
	public static BigInteger minus(BigInteger b1, BigInteger b2)  {
		return b1.subtract(b2);
	}
	public static BigInteger times(BigInteger b1, BigInteger b2) {
		return b1.multiply(b2);
	}
	public static BigInteger timesPlus(BigInteger b1, BigInteger b2, BigInteger b3) {
		return b1.multiply(b2).add(b3);
	}
	public static BigInteger mod(BigInteger b1, BigInteger b2) {
		return b1.mod(b2);
	}
	public static BigInteger quo(BigInteger b1, BigInteger b2) {
		return b1.divide(b2);
	}
	public static BigInteger rem(BigInteger b1, BigInteger b2) {
		return b1.remainder(b2);
	}
	public static MultiRecord divide(BigInteger b1, BigInteger b2)  {
		BigInteger[] qr = b1.divideAndRemainder(b2);
		MultiRecord result = new MultiRecord(divideFormat);
		result.setField(0, "quo", Value.U.fromBInt(qr[0]));
		result.setField(1, "rem", Value.U.fromBInt(qr[1]));
		return result;
	}
	            
	
	public static int formatSInt(int v, Object s, int c) {
		throw new RuntimeException();
	}
	
	public static BigInteger sIPower(BigInteger b1, int b2) {
		throw new RuntimeException();
	}
	public static BigInteger bIPower(BigInteger b1, BigInteger b2) {
		throw new RuntimeException();
	}
    public static BigInteger powerMod(BigInteger b1, BigInteger b2, BigInteger b3) {
		throw new RuntimeException();
	}

	public static BigInteger shiftUp(BigInteger b1, int b2)  {
		throw new RuntimeException();
	}
	public static BigInteger shiftDn(BigInteger b1, int b2)  {
		throw new RuntimeException();
	}
	public static BigInteger shiftRem(BigInteger b1, BigInteger b2) {
		throw new RuntimeException();
	}

    public static int timesModInv(int a, int b, int c, double d) {
		throw new RuntimeException();
    }


    public static float sfloAssemble(boolean sign, int i, Word w) {
	throw new RuntimeException();
    }

    public static MultiRecord sfloDissemble(float f) {
	throw new RuntimeException();
    }

    public static float sfloNext(float f) {
	throw new RuntimeException();
    }

    public static float sfloPrev(float f) {
	throw new RuntimeException();
    }

    public static float sfloRTimes(float a, float b, int r) {
	throw new RuntimeException();
    }

    public static float sfloRMinus(float a, float b, int r) {
	throw new RuntimeException();
    }

    public static float sfloRPlus(float a, float b, int r) {
	throw new RuntimeException();
    }

    public static float sfloRDivide(float a, float b, int r) {
	throw new RuntimeException();
    }

    public static float sfloRTimesPlus(float a, float b, float c, int r) {
	throw new RuntimeException();
    }

    public static BigInteger sfloTruncate(float a) {
	throw new RuntimeException();
    }
    public static float sfloFraction(float a) {
	throw new RuntimeException();
    }
    public static BigInteger sfloRound(float a, int b) {
	throw new RuntimeException();
    }



    public static double dfloAssemble(boolean sign, int i, Word w1, Word w2) {
	throw new RuntimeException();
    }

    public static MultiRecord dfloDissemble(double f) {
	throw new RuntimeException();
    }
    public static float dfloNext(double f) {
	throw new RuntimeException();
    }

    public static float dfloPrev(double f) {
	throw new RuntimeException();
    }

    public static double dfloRTimes(double a, double b, int r) {
	throw new RuntimeException();
    }

    public static double dfloRMinus(double a, double b, int r) {
	throw new RuntimeException();
    }

    public static double dfloRPlus(double a, double b, int r) {
	throw new RuntimeException();
    }

    public static double dfloRDivide(double a, double b, int r) {
	throw new RuntimeException();
    }

    public static double dfloRTimesPlus(double a, double b, double c, int r) {
	throw new RuntimeException();
    }

    public static BigInteger dfloTruncate(double a) {
	throw new RuntimeException();
    }
    public static double dfloFraction(double a) {
	throw new RuntimeException();
    }
    public static BigInteger dfloRound(double a, int b) {
	throw new RuntimeException();
    }


    public static int formatBInt(BigInteger i, Object o, int n) {
	throw new RuntimeException();
    }
    public static int formatSFlo(float i, Object o, int n) {
	throw new RuntimeException();
    }
    public static int formatDFlo(double i, Object o, int n) {
	throw new RuntimeException();
    }

    public static MultiRecord scanSFlo(Object o, int n) {
	throw new RuntimeException();
    }
    public static MultiRecord scanDFlo(Object o, int n) {
	throw new RuntimeException();
    }
    public static MultiRecord scanSInt(Object o, int n) {
	throw new RuntimeException();
    }
    public static MultiRecord scanBInt(Object o, int n) {
	throw new RuntimeException();
    }
}
