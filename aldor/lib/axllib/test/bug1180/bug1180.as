--* From hemmecke@risc.uni-linz.ac.at  Thu Nov 18 11:09:20 1999
--* Received: from kernel.risc.uni-linz.ac.at (root@kernel.risc.uni-linz.ac.at [193.170.36.225])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id LAA12337
--* 	for <ax-bugs@nag.co.uk>; Thu, 18 Nov 1999 11:09:07 GMT
--* Received: from iapetus.risc.uni-linz.ac.at (root@iapetus.risc.uni-linz.ac.at [193.170.36.25])
--* 	by kernel.risc.uni-linz.ac.at (8.9.2/8.9.2/Debian/GNU) with ESMTP id MAA03642
--* 	for <ax-bugs@nag.co.uk>; Thu, 18 Nov 1999 12:07:45 +0100 (CET)
--* Received: by risc.uni-linz.ac.at
--* 	via send-mail from stdin
--* 	id <m11oPPl-0025TNC@iapetus.risc.uni-linz.ac.at> (Debian Smail3.2.0.102)
--* 	for ax-bugs@nag.co.uk; Thu, 18 Nov 1999 12:07:45 +0100 (CET) 
--* Message-Id: <m11oPPl-0025TNC@iapetus.risc.uni-linz.ac.at>
--* Date: Thu, 18 Nov 1999 12:07:45 +0100 (CET)
--* From: hemmecke@risc.uni-linz.ac.at (Ralf HEMMECKE)
--* To: ax-bugs@nag.co.uk
--* Subject: [2] semantic changing add statement

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -DC1 -grun xxx.as
-- Version: Aldor version 1.1.12p2 for LINUX(glibc)
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 18-NOV-99
-- Aldor version 1.1.12p2 for LINUX(glibc)
-- Subject: semantic changing add statement


-- Calling sequence:
-- Problem case:
--   axiomxl -V -DC1 -grun xxx.as
--The output will be

--:E1(1): x1	E1(2): x2
--:E2(1): x	E2(2): y

--while for 
--   axiomxl -V -DC1 -grun xxx.as
--the output is as wanted

--:E1(1): x	E1(2): y
--:E2(1): x	E2(2): y

-- I hope that this is also considered a bug by NAG. I had quite a hard
-- time to figure out this strange behaviour.
-- However, I guess that although I think that
--    CxDegLexPP(vars: LS): PPCat == CxTDegPP CxLexPP vars;
-- defines a constructor, it is actually considered 
-- by the compiler (or even by the language specification)
-- an ordinary function, maybe only a
-- bit special since it returns a domain, but who knows.
-- With this in mind, the question arises whether or not
--    CxPP(vars: LS,s: String): PPCat with == {add { ... } where {...}}
-- is considered a function or a domain constructor.

-- The intension of my original code (which I have shortened here) was
-- to provide a default definition in PPCat and to overload it by
-- new code from a derived category.

#include "axllib"

macro {
	I == SingleInteger;
	LS == List String;
}

define PPCat: Category == with {
	name: I -> String;
    default {
	name(i: I): String == {-- make x1,x2,x3,x4,...
		A ==> Array Character;
		import from TextWriter,A;
		buffer: A := new(1, char "x");
		wr := writer buffer;
		wr << i; 
		string buffer;
	}
    }
}

define PPCat(T: PPCat): Category == PPCat with {
	coerce: % -> T;
	coerce: T -> %;
    default {
	import from T;
	name(i: I): String == name(i)$T;
    }
}

-------------------------------------------------------------------
CxPP(vars: LS,s: String): PPCat with == {add { -- where clause follows
	name(i: I): String == {
		if i < 0 or i > numOfVars then {
			error "There is no variable with this index."
		} else {
			vars.i;
		}
	}
    } where {numOfVars: I == #vars}
}

CxTDegPP(E: PPCat): PPCat E with == add {
	Rep ==> Record(ex: E, tdeg: I);
	import from E, I, Rep;
	coerce(x: %): E  == rep(x).ex;
	coerce(e: E): % == per [e,  1];
}

CxLexPP(vars: LS): PPCat == CxPP(vars,"lex") add;
CxDegLexPP(vars: LS): PPCat == CxTDegPP CxLexPP vars 
#if C1
add --PROBLEM `add'
#endif
;

main():() == {
	import from I, LS, Character;
	vars: LS == ["x", "y", "z"];
	E1 == CxDegLexPP vars;
	E2 == CxTDegPP CxLexPP vars;
	print << "E1(1): " << name(1)$E1 << tab 
	      << "E1(2): " << name(2)$E1 << newline;
	print << "E2(1): " << name(1)$E2 << tab
	      << "E2(2): " << name(2)$E2 << newline;
}
main();
