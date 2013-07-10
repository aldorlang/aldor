------------------------------ sort1.as --------------------------------
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

-- No need to call this "main", but an additional function
-- keeps the top-level name-space free from clutter.
main(): () == {
	import from MachineInteger, Array MachineInteger;
	import from TextWriter;		-- for 'stdout'
	import from WriterManipulator;	-- for 'endnl'
	arr := [x*(x-5) for x in 100..1 by -1];
	bubbleSort! arr;
	-- endnl sends a newline and flushes the stream
	stdout << arr << endnl;
}

-- call the function.
main();

