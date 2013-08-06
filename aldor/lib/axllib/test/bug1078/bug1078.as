--* Received: from igw2.watson.ibm.com by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA11637; Mon, 13 May 96 16:50:12 BST
--* Received: from mailhub1.watson.ibm.com (mailhub1.watson.ibm.com [9.2.249.31]) by igw2.watson.ibm.com (8.7.4/8.7.1) with ESMTP id LAA09759 for <ax-bugs@nag.co.uk>; Mon, 13 May 1996 11:44:33 -0400
--* Received: from matteo.watson.ibm.com (matteo.watson.ibm.com [9.2.221.114]) by mailhub1.watson.ibm.com (8.7.1/03-28-96) with SMTP id LAA61467 for <ax-bugs@nag.co.uk>; Mon, 13 May 1996 11:44:15 -0400
--* Received: by matteo.watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA18615; Mon, 13 May 1996 11:43:20 -0400
--* Date: Mon, 13 May 1996 11:43:20 -0400
--* From: dooley@watson.ibm.com (Sam Dooley)
--* Message-Id: <9605131543.AA18615@matteo.watson.ibm.com>
--* To: ax-bugs
--* Subject: [2] Refinement on bug1053 from Manuel.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: axiomxl -Fc -Cargs=-g -q2 -Fx b1053.as
-- Version: v1.1.6
-- Original bug file name: b1053.as

#include "axllib"

-- Once primeIndex and f are inlined into foo,
-- the access into smallprimes which was inlined from f
-- looks in the wrong environment level to find smallprimes.

-- The inlined code looks for smallprimes in envLevel1.
-- smallprimes can be found in envLevel0.

-- The extra print statement ensures that smallprimes
-- is initialized correctly (there is a different bug
-- if that line is omitted).

-- This bug only appears on the Sun platform,
-- which produces a segmentation fault.

-- It also only seems to appear when using '-Fc -Cargs=-g'.

-- forland% axiomxl -Fc -Cargs=-g -q2 -Fx b1053.as
-- forland% b1053
-- foo(2) = smallprimes.1 = 3
-- Segmentation fault

macro Z	== SingleInteger;

SmallPrimes: with { primeIndex: Z -> Z } == add {
	import from Z, Array Z;

	smallprimes:Array(Z) == [3,5,7,11,13];

	primeIndex(prime:Z):Z == {
		-- The next line prevents an init bug so we can see
		-- the environment level bug.
		print << "smallprimes.1 = " << smallprimes.1 << newline;

		f := (m:Z):Z +-> smallprimes.m;
		fa := f 1;
		prime < fa => 0;
		0;
	}
}

Foo: with { foo: Z -> Z } == add {
	foo(n:Z):Z == {
		import from SmallPrimes;
		n < 2 or primeIndex(n) ~= 0 => 0;
		n;
	}
}

import from Z, Foo;
print << "foo(2) = " << foo 2 << newline;
