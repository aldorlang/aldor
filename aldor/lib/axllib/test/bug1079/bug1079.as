--* Received: from igw2.watson.ibm.com by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA11686; Mon, 13 May 96 16:52:03 BST
--* Received: from mailhub1.watson.ibm.com (mailhub1.watson.ibm.com [9.2.249.31]) by igw2.watson.ibm.com (8.7.4/8.7.1) with ESMTP id LAA23141 for <ax-bugs@nag.co.uk>; Mon, 13 May 1996 11:46:19 -0400
--* Received: from matteo.watson.ibm.com (matteo.watson.ibm.com [9.2.221.114]) by mailhub1.watson.ibm.com (8.7.1/03-28-96) with SMTP id LAA33629 for <ax-bugs@nag.co.uk>; Mon, 13 May 1996 11:45:31 -0400
--* Received: by matteo.watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA18626; Mon, 13 May 1996 11:45:06 -0400
--* Date: Mon, 13 May 1996 11:45:06 -0400
--* From: dooley@watson.ibm.com (Sam Dooley)
--* Message-Id: <9605131545.AA18626@matteo.watson.ibm.com>
--* To: ax-bugs
--* Subject: [2] Additional init bug not present in bug1053.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: axiomxl -Fc -Cargs=-g -q2 -Fx b1053a.as
-- Version: v1.1.6
-- Original bug file name: b1053a.as

#include "axllib"

-- The bug here is that smallprimes is not being initialized at all
-- once primeIndex in inlined into foo.

macro Z	== SingleInteger;

SmallPrimes: with { primeIndex: Z -> Z } == add {
	import from Z, Array Z;

	smallprimes:Array(Z) == [3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59];

	primeIndex(prime:Z):Z == {
		f := (m:Z):Z +-> smallprimes.m;
		fa := f 1;
		fb := f 16;
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

