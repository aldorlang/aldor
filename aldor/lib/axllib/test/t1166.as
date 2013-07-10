--> testerrs -q2 -fo 
--* Subject: [2] bad macro in foamopt.h

--@ Bug Number:  bug1166.as 
--@ Fixed  by:  TTT   
--@ Tested by:  t1166.as 
--@ Summary:    The  and  side-effect specification should not be used for compliance with gcc 2.95.x 

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

