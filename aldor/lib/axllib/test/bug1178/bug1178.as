--* From hemmecke@risc.uni-linz.ac.at  Thu Nov  4 12:24:54 1999
--* Received: from admin.risc.uni-linz.ac.at (admin.risc.uni-linz.ac.at [193.170.33.111])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with SMTP id MAA07666
--* 	for <ax-bugs@nag.co.uk>; Thu, 4 Nov 1999 12:24:53 GMT
--* Received: from bianca.risc.uni-linz.ac.at by admin.risc.uni-linz.ac.at with SMTP id AA29752
--*   (5.67b8/IDA-1.5 for <ax-bugs@nag.co.uk>); Thu, 4 Nov 1999 13:24:14 +0100
--* Received: by risc.uni-linz.ac.at
--* 	via send-mail from stdin
--* 	id <m11jMqP-000DuBC@bianca.risc.uni-linz.ac.at> (Debian Smail3.2.0.101)
--* 	for ax-bugs@nag.co.uk; Thu, 4 Nov 1999 14:22:25 +0100 (CET) 
--* Message-Id: <m11jMqP-000DuBC@bianca.risc.uni-linz.ac.at>
--* Date: Thu, 4 Nov 1999 14:22:25 +0100 (CET)
--* From: Ralf.Hemmecke@risc.uni-linz.ac.at (Ralf Hemmecke)
--* To: ax-bugs@nag.co.uk
--* Subject: [9] Problem with domain constant

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -DCEqual xxx.as
-- Version: Aldor version 1.1.12p2 for SPARC [Solaris: GCC]
-- Original bug file name: xxx.as

-----------------------------------------------------------------------
-- 04-NOV-99
-----------------------------------------------------------------------
-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 04-NOV-99
-- Aldor version 1.1.12p2 for SPARC [Solaris: GCC] 
-- Subject: Problem with domain constant


-- Calling sequence:
-- Correct output:
--   axiomxl -grun -DCUnEqual xxx.as
--:Exec: ./xxx
--:DOM(1)
--:DOM(2)
--:(a,b,c)==list(1, 1, 2)
-- Program crashes:
--   axiomxl -grun -DCEqual xxx.as
--:Exec: ./xxx
--:Segmentation Fault - core dumped


#include "axllib"

macro {
	I == SingleInteger;
#if CEqual
	DOMX == DOMY;
#endif
}

import from List I;

DOM(k:I): with {
	init: I -> %;
	inc: () -> I;
} == add {
	print << "DOM(" << k <<")"<< newline;
	Rep ==> I;
	import from Rep;
	intern: I := 0;

	init(i:I):% == {
		free intern;
		intern := i;
		per i;
	}
	inc(): I == {
		free intern;
		intern := intern +1;
	}
}

main():() == {

	f(k:I):I == {
		DOMX == DOM(k);
		inc()$DOMX;
	}

	DOMY == DOM(1);
	B == DOM(2);
	a := inc()$DOMY;
	b := inc()$B;
	print << [a,b,f(1),f(2),f(3)] << newline;
}

main();
