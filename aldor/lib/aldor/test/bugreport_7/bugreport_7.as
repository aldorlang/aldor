-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 15-Jun-2005
-- Aldor version 1.0.2 for LINUX(glibc2.3)
-- Subject: Modifying constant 1

-- Compile with
-- aldor -grun -mno-mactext -laldor xxx.as
-- The output is
--: 1 = [1, 1]
--: z = [8, 100]
--: 1 = [8, 100]

-- The problem is that the implementation of BinaryPowering contains 
-- the lines
-- binaryExponentiation!(a:T, b:Z):T	== binPow!(1, a, b);
-- if T has CopyableType and Z has CopyableType then {
--	binaryExponentiation(a:T, b:Z):T == binPow!(1, copy a, copy b);
-- }
-- and binPow! builds on the function times!$T. So if times!$T 
-- destroys its first argument, then the constant 1 is modified.

-- A bugfix would be to replace "1" by "copy 1" in the second case. 
-- For the first appearance of "1" this would not be possible since
-- T might not be of CopyableType.

#include "aldor"
#include "aldorio"

macro {
	I == MachineInteger;
	X == rep x;
	Y == rep y;
}
MyInt: IntegerType == add {
	Rep == Record(eins: I, zwei: I);
	import from Rep, I;

	-- let's have a really destructive times! function
	times!(x: %, y: %): % == {
		X.eins := times!(X.eins, Y.eins);
		X.zwei := 100;
		x;
	}

	-- make some good definitions for the necessary functions
	0: % == per [0, 0];
	1: % == per [1, 1];
	(x: %) = (y: %): Boolean == X.eins = Y.eins;
	(x: %) < (y: %): Boolean == X.eins < Y.eins;
	(x: %) + (y: %): % == per [X.eins + Y.eins, 2];
	(x: %) * (y: %): % == per [X.eins * Y.eins, 3];
	(x: %) quo (y: %): % == per [X.eins quo Y.eins, 4];
	(x: %) rem (y: %): % == per [X.eins rem Y.eins, 5];
	- (x: %): % == per [- X.eins, 14];
	~ (x: %): % == - x;
	(x: %) \/ (y: %): % == x + y;
	(x: %) /\ (y: %): % == x * y;
	(x: %) ^ (i: I): % == {
		z := x;
		for j in 2..i repeat z := z*x;
		z;
	}
	<<(tr: TextReader): %   == {i: I := <<tr; per [i, 15];}
	<<(br: BinaryReader): % == {i: I := <<br; per [i, 16];}
	(tw: TextWriter)   << (x: %): TextWriter   == {
		tw << "[" << X.eins << ", " << X.zwei << "]";
	}
	(bw: BinaryWriter) << (x: %): BinaryWriter == bw << X.eins;
	bit?(x: %, i: I): Boolean == bit?(X.eins, i);
	coerce(i: I): % == per [i, 7];
	machine(x: %): I == X.eins;
	gcd(x: %, y: %): % == per [gcd(X.eins, Y.eins), 6];
	divide(x: %, y: %): (%, %) == {
		(a, b) := divide(X.eins, Y.eins);
		(per [a, 1], per [b, 2]);
	}
	integer(l: Literal): % == per [integer l, 7];
	length(x: %): I == length(X.eins);
	nthRoot(x: %, y: %): (Boolean, %) == {
		(b, i) := nthRoot(X.eins, Y.eins);
		(b, per [i, 8]);
	}
	random(): % == per [random(), 9];
	random(i: I): % == per [random i, 10];
	shift(x: %, i: I): % == per [shift(X.eins, i), 11];
	
}

main(): () == {
	import from MyInt, I;
	import from BinaryPowering(MyInt, I);

	x: MyInt := 2;
	stdout << "1 = " << 1@MyInt << newline;
	z := binaryExponentiation!(2, 3);
	stdout << "z = " << z << newline;
	stdout << "1 = " << 1@MyInt << newline;
}

main();