--* From DOOLEY%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Fri Feb 10 09:42:56 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA17229; Fri, 10 Feb 1995 09:42:56 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 9613; Fri, 10 Feb 95 09:42:33 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.DOOLEY.NOTE.VAGENT2.7237.Feb.10.09:42:32.-0500>
--*           for asbugs@watson; Fri, 10 Feb 95 09:42:33 -0500
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 7229; Fri, 10 Feb 1995 09:42:32 EST
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Fri, 10 Feb 95 09:42:31 EST
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA14815; Fri, 10 Feb 1995 09:38:14 -0500
--* Date: Fri, 10 Feb 1995 09:38:14 -0500
--* From: dooley@watson.ibm.com (Sam Dooley)
--* Message-Id: <9502101438.AA14815@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [3] Code generation of multi-valued defines.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp -Fx object1a.as; object1a.as
-- Version: r1-0-6
-- Original bug file name: object1a.as

--> testcomp
--> testrun

#include "axllib"

f(): () == {
	import from SingleInteger, SingleFloat, DoubleFloat, Ratio Integer;
	import from Character;

	x: List Object Order := [
		object(Character, char "g"),
		object(Integer, -13),
		object(SingleInteger, 2),
		object(SingleFloat, 1.25),
		object(DoubleFloat, -0.4),
		object(Ratio Integer, 2/3)
	];

	-- Test code generation of multi-valued defines.

	print << "Test: ";
	for ob in x repeat {
		(S: Order, s: S) == avail ob;
		if S has OrderedAbelianMonoid then
			if s > 0 then print << "[" << s << "]";
	}
	print << newline;
}

f();
