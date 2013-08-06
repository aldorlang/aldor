--* Received: from nirvana.inria.fr by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA06626; Thu, 29 Aug 96 17:25:56 BST
--* Received: by nirvana.inria.fr (8.7.5/8.6.12) id SAA20351 for ax-bugs@nag.co.uk; Thu, 29 Aug 1996 18:19:29 +0200
--* Date: Thu, 29 Aug 1996 18:19:29 +0200
--* From: Stephen Watt <Stephen.Watt@sophia.inria.fr>
--* Message-Id: <199608291619.SAA20351@nirvana.inria.fr>
--* To: ax-bugs
--* Subject: [3] Over-riding implementations ignored

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx ex1.as
-- Version: AXIOM-XL version 1.1.5 for LINUX
-- Original bug file name: ex1.as

#include "axllib"

define Cat1: Category == with {
	op1: Integer -> Integer;
	op2: Integer -> Integer;

	default op1(n: Integer): Integer == {
		print << "The default op1 for " << n << newline;
		n*2
	}
}


Package1: Cat1 == add {
	op2(n: Integer): Integer == op1 op1 op1 n;
}



Package2: Cat1 == Package1 add {
	op1(n: Integer): Integer == { print << "The overriding op1 for " << n << newline; n }
}



main():() == {

	import from Integer;

	-- This should use the default op1, and does.
	print << "From package 1: " << newline << op2(3)$Package1 << newline;

	-- ************* This should use op1$Package2, but doesn't. **********
	print << "From package 2: " << newline << op2(3)$Package2 << newline;
}

main()
