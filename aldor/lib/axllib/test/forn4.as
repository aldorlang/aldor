-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen c
--> testgen l
#include "axllib.as"

import from SingleInteger;

import {
	roo: (String, SingleInteger) -> SingleInteger;
	qoo: (String, SingleInteger) -> SingleInteger;
} from Foreign;

import {
	moo: (String, SingleInteger) -> SingleInteger;
	noo: (String, SingleInteger) -> SingleInteger;
} from Foreign C;

import {
	xxbar: () -> ();
} from Foreign C "myfile.h";

import {
	xxfoo: (String, SingleInteger) -> SingleInteger;
} from Foreign C "hisfile.h0";

import {
	xxboo: (String, SingleInteger) -> SingleInteger;
	xxhoo: (String, SingleInteger) -> SingleInteger;
} from Foreign C "hisfile.h0";

import {
	Earth: Type;
	earth: Earth;
	xxmover: Earth -> ();
} from Foreign C "c:\heavy\equip\vehicles.h";

import {
	toupper: SingleInteger -> SingleInteger;
} from Foreign C "<ctype.h>";

import from Foreign C "<math.h>";

x: Earth == earth;

moo("hello", 2);
noo("hello", 2);

toupper(100);

xxbar();
xxfoo("hello", 2);
xxboo("hello", 2);
xxhoo("hello", 2);
xxmover earth; 

macro {
	Ptr == Pointer;
	Int == SingleInteger;
}

export {
	fact:		Int -> Int;
	print:		Int -> ();
} to Foreign C;

fact(x: Int): Int == if x = 0 then 1 else x * fact(x-1);
print(x: Int): () == print << x << newline;
