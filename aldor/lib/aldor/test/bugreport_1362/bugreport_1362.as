-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 26-Apr-2004
-- Aldor version 1.0.2 for LINUX(gcc-2_96)
-- Subject: inline-limit

-- Compilation with
--   aldor -DBUG -q5 -q inline-limit=9 xxx.as
-- works fine.
-- Setting inline-limit=10 results in 
--: #1 (Fatal Error) Looking for `first' with code `14819079' in `'. Export of `(null)' not found.
-- during the COMPILATION process.
-- If one removes the -DBUG switch, the same error occurs at
-- inline-limit=11.

#include "aldor"

macro LA == List A;

CalixCriticalPairs0(
    A: with {
	neutral: %;
	neutral?: % -> Boolean;
	sPolynomial: (%, %) -> %;
    },
    B: with {
	create: LA -> %;
    }
): with {} == add {
	import from TextWriter, String;
	stderr << name(%)$Trace;
	macro Pair == Record(a1: A, a2: A);
	Rep == List Pair;
	import from Pair, Rep;

	choose!(x: %): (A, %) == {
		c: Pair := [neutral, neutral];		
		(sPolynomial(c.a1, c.a2), per rest rep x);
	}

	updateCriticalPairs!(x: %): % == {
#if BUG
		per append!([], rep x);
#else
		per append!([]$Rep, rep x);
#endif
	}

	create(basis: LA): (%, B) == {
		import from A;
		b: B := create basis;
		if empty? basis then return (per empty, b);
		newBasis: LA := empty;
		for a in basis repeat {
			newBasis := cons(a, newBasis);
		}
		(per empty, b);
	}
}
