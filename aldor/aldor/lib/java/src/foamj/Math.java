package foamj;

import java.math.BigInteger;

public class Math {
	private static final Format divideFormat = new Format(2);

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
		throw new RuntimeException();
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
		return b1.compareTo(b2) < 0;
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
	            
	
	public static int formatSInt(int v, char[] s, int c) {
		throw new RuntimeException();
	}
	
	public static BigInteger sIPower(BigInteger b1, BigInteger b2) {
		throw new RuntimeException();
	}
	public static BigInteger bIPower(BigInteger b1, BigInteger b2) {
		throw new RuntimeException();
	}
	public static BigInteger powerMod(BigInteger b1, BigInteger b2) {
		throw new RuntimeException();
	}

	public static BigInteger shiftUp(BigInteger b1, BigInteger b2)  {
		throw new RuntimeException();
	}
	public static BigInteger shiftDn(BigInteger b1, BigInteger b2)  {
		throw new RuntimeException();
	}
	public static BigInteger shiftRem(BigInteger b1, BigInteger b2) {
		throw new RuntimeException();
	}
}
