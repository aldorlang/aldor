-----------------------------------------------------------------------
-- 16-JUL-2003
-----------------------------------------------------------------------
-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 16-Jul-2003
-- Aldor version 1.0.0 for LINUX(gcc-2_96)
--   This is actually aldor-1.0.1-linux-i386-gmp4.bin (7684449 bytes)
--   (MD5: 29d37c14bbeb0029166ccdd6ed0cdba2)
-- Subject: [1]libaldor vs. libalgebra crash
-- Compile the program by
-- aldor -V -Y$ALGEBRAROOT/lib -I $ALGEBRAROOT/include -Y/home/hemmecke/local/lib -Q5 -Csmax=0 -Fx -lalgebra-gmp -laldor -DGMP -Cruntime=foam-gmp,gmp4,m -fc -DC1 -DALG xxx.as
-- (/home/hemmecke/local/lib is for libgmp4.a)
-- Then start xxx.
-- The executable crashes.
--: Copyright (C) 2003 Ralf Hemmecke
--: Before copyN
--: Entering copyN where n is
--: 3
--: copyN create y
--: copyN starting for loop
--: copyN for loop: i is
--: 0
--: Segmentation fault

-- If any of the directives C1 or ALG are left out, the program runs
-- properly.

-- Interestingly the following program runs also nicely:
-- Remove libalgebra.al libalgebra.a libalgebrad.al libalgebrad.a.
-- Then compile
-- aldor -V -Y$ALGEBRAROOT/lib -I $ALGEBRAROOT/include -Y/home/hemmecke/local/lib -Q5 -Csmax=0 -Fx -lalgebra-gmp -laldord -DDEBUG -DGMP -Cruntime=foam-gmp,gmp4,m -fc -DC1 -DALG xxx.as

-- The program compiles without complaining and gives the expected
-- result:

--: Copyright (C) 2003 Ralf Hemmecke
--: Before copyN
--: Entering copyN where n is
--: 3
--: copyN create y
--: copyN starting for loop
--: copyN for loop: i is
--: 0
--: copyN for loop end iteration i is
--: 0
--: copyN for loop: i is
--: 1
--: copyN for loop end iteration i is
--: 1
--: copyN for loop: i is
--: 2
--: copyN for loop end iteration i is
--: 2
--: leaving copyN!
--: After copyN
--: End Program

-- Looking at the generated .c file exibits the problem. With C1 and
-- ALG the compiler generates code which looks as if PrimitiveArray
-- contains size information (maybe this is why the 
--   -laldord -DDEBUG
-- works fine).

-- I produced libalgebra*.a by running 'make' in the extracted
-- algebra-1.0.1 directory. (BTW, in order to run the testsuite one is
-- required to take some effort to set the variables and include
-- directories correctly.)


#if ALG
#include "algebra"
#else
#include "aldor"
#endif

macro {
	SIZE == MachineInteger; -- Array size
	Z    == MachineInteger;
	T    == H4ti2Term;

	ZARR == PrimitiveArray Z;
	ZARRCAT == PrimitiveArrayType Z;

	PR(x) == {
		import from TextWriter, Character, String;
		stdout << x << newline;
		flush! stdout;
	}
}

H4ti2Term: ZARRCAT with {
	copyN: SIZE -> % -> %;
	gcdN!: SIZE -> (%, %) -> ();
} == ZARR add {
	Rep == ZARR;
	import from Rep;
	copyN(n: SIZE)(x: %): % == {
		PR "Entering copyN where n is";
		PR n;
		PR "copyN create y";
#if C1
		y := per new n;
#else
		y: Rep := new(n);
#endif
		PR "copyN starting for loop";
		for i in 0..n-1 repeat {
			PR "copyN for loop: i is";
			PR i;
			y.i := x.i;
			PR "copyN for loop end iteration i is";
			PR i;
		}
		PR "leaving copyN!";
#if C1
		y
#else
		per y;
#endif
	}
	gcdN!(n: SIZE)(x: %, y: %): () == {
		PR "Entering gcdN! where n is";
		PR n;
		for i in 0..n-1 repeat {
			PR "gcdN! for loop: i is";
			PR i;
			PR x.i;
			PR y.i;
			PR "gcdN! compute minimum";
			z: Z := min(x.i, y.i);
			PR "gcdN! assignment";
#if C1
			x.i := z;
#else
			rep(x).i := z;
#endif
			PR "gcdN! for loop end iteration: i is";
			PR i;
		}
		PR "leaving gcdN!"
	}
}



main(): () == {
	import from T, Z, List Z;
	numOfVars: Z == 3;
	PR "Copyright (C) 2003 Ralf Hemmecke";

	t1: T := [2,0,0];
	t2: T := [0,2,0];
	t3: T := [0,0,2];

	PR "Before copyN";

	t: T := copyN(numOfVars)(t1);

	PR "After copyN";
	PR "End Program";
}

main();

