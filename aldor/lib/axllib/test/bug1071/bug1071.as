--* Received: from nirvana.inria.fr by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA13381; Thu, 25 Apr 96 09:52:28 BST
--* Received: by nirvana.inria.fr (8.6.13/8.6.12) id KAA03199 for ax-bugs@nag.co.uk; Thu, 25 Apr 1996 10:47:06 +0200
--* Date: Thu, 25 Apr 1996 10:47:06 +0200
--* From: Stephen Watt <Stephen.Watt@sophia.inria.fr>
--* Message-Id: <199604250847.KAA03199@nirvana.inria.fr>
--* To: ax-bugs
--* Subject: [3][runtime] Export not found

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx essai.as
-- Version: AXIOM-XL version 1.1.5 for LINUX
-- Original bug file name: essai.as

-->>>> Output of this program is:
--> Looking in RingObserver(Integer) for coerce with code 728554000
--> Export not found
------------------------------------------------------------------------------
#include "axllib.as"

RingObserver(R: Ring): Ring == R add {

	----------- SETTINGS -------------
	Rep ==> R;
	import from Rep;

	------------ LOCAL FUNCTIONS ------------

	local has_been_called(s: String): () == 
		print << s << " called " << newline;

	------------ WRAP ------------

	wrap(s: String, f: (Rep,Rep)->Rep)(a: %, b:%): %  == {
		has_been_called(s);
		per f(rep a, rep b)
	}

	------------ TRACED FUNCTIONS ------------

	+: (%,%)->% == wrap("+", +$R);
	-: (%,%)->% == wrap("-", -$R);
	*: (%,%)->% == wrap("*", *$R);
}


test():() == {
	RO ==> RingObserver(Integer);
	import from RO, Integer;

	n := 20::RO;
	i := 1 + n * 1;


	print << "The value of i is " << i << newline;
}

test()
