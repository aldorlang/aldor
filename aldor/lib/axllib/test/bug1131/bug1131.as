--* From peterb@nag.co.uk  Tue Aug 26 18:44:44 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA15921; Tue, 26 Aug 97 18:44:44 +0100
--* Received: from red.nag.co.uk (red.nag.co.uk [192.156.217.2])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with SMTP
--* 	  id SAA22476 for <ax-bugs@nag.co.uk>; Tue, 26 Aug 1997 18:47:37 +0100 (BST)
--* Received: from zorba.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for ax-bugs@nag.co.uk id AA15918; Tue, 26 Aug 97 18:44:40 +0100
--* Received: by zorba.nag.co.uk (8.8.5/920502.SGI)
--* 	for ax-bugs@nag.co.uk id SAA01226; Tue, 26 Aug 1997 18:44:01 +0100
--* Date: Tue, 26 Aug 1997 18:44:01 +0100
--* From: peterb@nag.co.uk (Peter Broadbery)
--* Message-Id: <199708261744.SAA01226@zorba.nag.co.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] diamond extensions

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.10
-- Original bug file name: iexp.as

#if A

#include "axllib"

Foo: BasicType with {
	op: Integer -> %;
	po: % -> Integer;
} == Integer add {
	Rep ==> Integer;
	op(x: Integer): % == per x;
	po(x: %): Integer == rep x;
}

#endif

#if B
#include "axllib"

#library A "iexp.ao"
import from A;
extend Foo: with {
	poop: Integer -> Integer
} == add {
	poop(x: Integer): Integer == po op x;
}

#endif

#if C
#include "axllib"

#library A "iexp.ao"
import from A;
extend Foo: with {
	oppo: % -> %
} == add {
	oppo(x: %): % == op po x;
}

#endif

#if D
#include "axllib"

#library A "iexp.ao"
#library B "iexpb.ao"
#library X "iexpc.ao"

import from B;
import from X;

-- NB: This is the easy case.
extend Foo: with == add;
-- x$ZZZ(Foo), where
-- ZZZ(X: BasicType): with {
--  	if X has poop and X has oppo then x: Integer
-- }
-- is as nasty as we need.
#endif

#if E
#include "axllib"

#library A "iexp.ao"
#library B "iexpb.ao"
#library X "iexpc.ao"

import from B;
import from X;

import from Integer;
import from Foo;
foo(x: Foo): () == {
	foo := op 10;
	print << (op 1) << newline;
--	print << poop 4 << newline;
--	print << oppo foo << newline;
}
#endif


