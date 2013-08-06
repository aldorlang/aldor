--* From Manuel.Bronstein@sophia.inria.fr  Mon Aug 23 16:28:44 1999
--* Received: from nirvana.inria.fr (IDENT:root@nirvana.inria.fr [138.96.48.30])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id QAA03930
--* 	for <ax-bugs@nag.co.uk>; Mon, 23 Aug 1999 16:28:41 +0100 (BST)
--* Received: by nirvana.inria.fr (8.8.8/8.8.5) id RAA15605 for ax-bugs@nag.co.uk; Mon, 23 Aug 1999 17:24:48 +0200
--* Date: Mon, 23 Aug 1999 17:24:48 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199908231524.RAA15605@nirvana.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] bad macro in foamopt.h

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q2 -fo -csys=linuxglibc-486 -fo bugasm.as
-- Version: 1.1.12p2
-- Original bug file name: bugasm.as

----------------------------- bugasm.as -------------------------------------
--
-- This file illustrates a bad macro definition in foamopt.h
-- for double word multiplication on linux/gcc machines.
-- This prevens optimization
--
-- % axiomxl -q2 -fo -csys=linuxglibc-486 -fo bugasm.as
-- bugasm.c: In function `CF3_foo':
-- /axiomxl-1.1.12/linuxglibc-486/include/foamopt.h:64: Invalid `asm' statement:
-- /axiomxl-1.1.12/linuxglibc-486/include/foamopt.h:64:
--                fixed or forbidden register 0 (ax) was spilled for class AREG.
-- #1 (Fatal Error) C compile failed.  Command was:
--                unicl -O -I/axiomxl-1.1.12/linuxglibc-486/include -c bugasm.c
--
--
-- I've been told that the macros fiWordTimesDoubleMacro
-- and fiWordDivideDoubleMacro should be fixed for use with newer gcc's:
-- % gcc -v
-- Reading specs from /usr/local/gcc/lib/gcc-lib/i686-pc-linux-gnu/2.95/specs
-- gcc version 2.95 19990728 (release)

#include "axllib"

macro W == Word$Machine;

Foo: with { foo: (W, W) -> (W, W) } == add {
	foo(a:W,b:W):(W,W) == { import from Machine; double_*(a, b) }
}
