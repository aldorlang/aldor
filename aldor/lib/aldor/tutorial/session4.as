------------------------------ session4.as --------------------------------
--
-- Fourth interactive session of "First Course on Aldor and Aldorlib"
--

#include "aldor"
#include "aldorinterp"

SimpleListStringOperations: with {
	firstOccurence: (List String, String)            -> MachineInteger;
	sort: (List String, (String, String) -> Boolean) -> List String;
} == add {
	firstOccurence(l: List String, s: String): MachineInteger == {
		(l, n) := find(s, l);
		n;
	}

	-- insertion sort.
	sort(l:List String, lt:(String,String) -> Boolean):List String == {
		local insrt(x: String, l: List String): List String == {
			empty? l or lt(x, first l) => cons(x, l);
			cons(first l, insrt(x, rest l));
		}
		res:List String := empty;
		for s in l repeat res := insrt(s, res);
		res;
	}
}

import from SimpleListStringOperations, List String;
firstOccurence(["a", "b"], "a")

Wrapped: with {
	wrap:   MachineInteger -> %;
	unwrap: % -> MachineInteger;
		++ these are such that unwrap(wrap(x)) = x
} == add {
	macro Rep == MachineInteger;
	wrap(n: MachineInteger): % == per n;
	unwrap(x: %): MachineInteger == rep x;
}


import from MachineInteger, Wrapped;

-- this is an error
wrap(10) + 5

-- as is this:
unwrap(5)

OutputType

import { unlink:  String -> MachineInteger } from Foreign C;

confirmAndRemove(name:String):MachineInteger == {
	import from TextWriter, TextReader, Character;
	stdout << "Are you sure you want to remove " << name << "? ";
	answer:String := << stdin;
	answer := map(lower)(answer);          -- make lower case
	answer = "yes" or answer = "y" => unlink name;
	-1;
}

confirmAndRemove "foo"

