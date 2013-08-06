--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Fri Jan 13 08:18:24 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA22569; Fri, 13 Jan 1995 08:18:24 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 5577; Fri, 13 Jan 95 08:18:22 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.PETERB.NOTE.YKTVMV.9097.Jan.13.08:18:21.-0500>
--*           for asbugs@watson; Fri, 13 Jan 95 08:18:22 -0500
--* Received: from sun2.nsfnet-relay.ac.uk by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Fri, 13 Jan 95 08:18:21 EST
--* Via: uk.co.iec; Fri, 13 Jan 1995 12:33:13 +0000
--* Received: from nldi16.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1) id AA17108;
--*           Fri, 13 Jan 95 12:34:33 GMT
--* From: Peter Broadbery <peterb@num-alg-grp.co.uk>
--* Date: Fri, 13 Jan 95 12:33:03 GMT
--* Message-Id: <5581.9501131233@nldi16.nag.co.uk>
--* Received: by nldi16.nag.co.uk (920330.SGI/NAg-1.0) id AA05581;
--*           Fri, 13 Jan 95 12:33:03 GMT
--* To: asbugs@watson.ibm.com
--* Subject: Confusion reigns over a parameterised domain.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


#include "axllib"

local Wrap(X: BasicType): BasicType with {
	wrap: X -> %;
	value: % -> X;
} == add {
	Rep ==> Record(x: X);
	import from Rep;

	wrap(x: X): % == per [x];
	value(a: %): X == rep(a).x;
	
	(a: %) = (b: %): Boolean == value a = value b;
	(print: TextWriter) << (a: %): TextWriter == print << "[" << value a << "]";
	sample: % == wrap sample;
}


f(): () == {
	import from Wrap Integer, Integer;
	print << wrap 12 << newline;
}

#if ResultsIn
"tt.as", line 21:         print << wrap 12 << newline;
                  ......................^
[L21 C23] #1 (Error) Argument 1 of `wrap' did not match any possible parameter type.
    The rejected type is Integer.
    Expected type X.
#endif

