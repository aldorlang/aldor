------------------------------ sort0.as --------------------------------
--
-- This file is used by the "First Course on Aldor and Aldorlib" tutorial
--

#include "aldor"

bubbleSort!(arr: Array MachineInteger): Array MachineInteger == {
	import from MachineInteger;
	for i in #(arr)-1 .. 0 by -1 repeat {
		for j in 1..i repeat {
			if arr.(j-1) > arr.j then {
				t := arr(j-1);
				arr.(j-1) := arr.j;
				arr.j := t;
			}
		}
	}
	arr
}

