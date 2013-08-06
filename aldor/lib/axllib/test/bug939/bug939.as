--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Thu Jan  5 11:04:19 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA22427; Thu, 5 Jan 1995 11:04:19 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 1937; Thu, 05 Jan 95 11:04:16 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.PETERB.NOTE.YKTVMV.3565.Jan.05.11:04:16.-0500>
--*           for asbugs@watson; Thu, 05 Jan 95 11:04:16 -0500
--* Received: from sun2.nsfnet-relay.ac.uk by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Thu, 05 Jan 95 11:04:16 EST
--* Via: uk.co.iec; Thu, 5 Jan 1995 15:24:39 +0000
--* Received: from nldi16.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1) id AA02184;
--*           Thu, 5 Jan 95 15:26:03 GMT
--* From: Peter Broadbery <peterb@num-alg-grp.co.uk>
--* Date: Thu, 5 Jan 95 15:22:28 GMT
--* Message-Id: <554.9501051522@nldi16.nag.co.uk>
--* Received: by nldi16.nag.co.uk (920330.SGI/NAg-1.0) id AA00554;
--*           Thu, 5 Jan 95 15:22:28 GMT
--* To: asbugs@watson.ibm.com
--* Subject: Substitution problem....

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


#include "axllib"

define C1(X: BasicType): Category == with { def: X -> Integer }


IOps: C1 Integer == add {
	def(x: Integer): Integer == x;
}

Wrap(X: BasicType, CC: C1 X): Join(C1 %, BasicType) with == add {

	Rep ==> Record(x: X);
	import from Rep, CC;

	sample: % == per [sample$X];

	(a: %) = (b: %): Boolean == rep a = rep b;
	(o: TextWriter) << (v: %): TextWriter == o << "[" << rep(v).x << "]";
	def(v: %): Integer == def(rep(v).x);
}


import from Integer;
import from IOps;
import from Wrap(Integer, IOps);
import from Wrap(Wrap(Integer, IOps), Wrap(Integer, IOps));

#if wannaSeeTheErrors
import from Wrap(Wrap(Integer, IOps), Wrap(Integer, IOps));
      ......................................^
[L5 C39] #1 (Error) Argument 2 of `Wrap' did not match any possible parameter type.
    The rejected type is
                Join(C1(%), BasicType) with
                ....
    Expected type C1(X).

#endif

