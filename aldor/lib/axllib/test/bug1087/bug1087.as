--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA12767; Fri, 7 Jun 96 16:00:11 BST
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id QAA21223 for <ax-bugs@nag.co.uk>; Fri, 7 Jun 1996 16:54:28 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id QAA25123 for ax-bugs@nag.co.uk; Fri, 7 Jun 1996 16:54:27 +0200
--* Date: Fri, 7 Jun 1996 16:54:27 +0200
--* Message-Id: <199606071454.QAA25123@ru7.inf.ethz.ch>
--* To: ax-bugs
--* Subject: [9] export not found with bad constant ordering

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fx notfound.as
-- Version: 1.1.6
-- Original bug file name: notfound.as

------------------------------- notfound.as --------------------------
--
-- This is connected with older bugs regarding defining constants
-- before using them. The following bug disappears if the definition
-- of # is moved before the one of size, so it's only a very minor problem.
--
-- % axiomxl -fx notfound.as
-- % notfound
-- Looking in PowerProduct(??, ??) for # with code 484208045
-- Export not found
--

#include "axllib"

macro I	== SingleInteger;

PowerProduct(n:I, m:I): Finite with {} == add {
	macro Rep == PrimitiveArray I;

	import from I, Rep;

	sample:%				== per new n;
	local size:I				== retract(#$%);
	(a:%) = (b:%):Boolean			== false;
	(port:TextWriter) << (p:%):TextWriter	== port;
	#:Integer				== n::Integer;
	basis:PrimitiveArray %			== new size;
}

macro PP == PowerProduct;

main(n:I, m:I):() == {
	import from Integer, PP(n,m);

	a := #$PP(n,m);
	print << "# = " << a << newline;
}

import from I;

main(2,2);
