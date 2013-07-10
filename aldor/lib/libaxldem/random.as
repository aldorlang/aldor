-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

#include "axllib.as"

--% RandomNumberSource
+++  All random numbers used in the system should originate from
+++  the same generator.  This package is intended to be the source.
--
--  Possible improvements:
--  1) Start where the user left off
--  2) Be able to switch between methods in the random number source.
 
RandomNumberSource: with {
        size: Integer;
		++ size is the base of the random number generator
 
        random:  () -> Integer;
		++ random() is a random number in 0..size-1.

        random: Integer -> Integer;
		++ random(n) is a random number in 0..n-1.

        reseed: Integer -> ();
		++ reseed(n) restarts the random number generator with seed n.
}
== add {
        -- This random number generator passes the spectral test
        -- with flying colours. [Knuth vol2, 2nd ed, p105]
        ranbase: Integer := 2^31-1;
        x0:      Integer := 1231231231;
        x1:      Integer := 3243232987;
 
        random(): Integer == {
		free x0, x1;
		t := (271828183 * x1 - 314159269 * x0) rem ranbase;
		if t < 0 then t := t + ranbase;
		x0:= x1;
		x1:= t;
	}
 
        size:Integer == ranbase;

        reseed(n: Integer): () == {
		free x0, x1;
		x0 := n rem ranbase;
		x1 := (n quo ranbase) rem ranbase;
	}
 
        -- Compute an integer in 0..n-1.
        random(n: Integer): Integer ==
		(n * random()) quo ranbase;
}
