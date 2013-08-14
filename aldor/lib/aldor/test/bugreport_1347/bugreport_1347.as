#include "aldor"
#pile

import from TextWriter, String, Character

F ==> DoubleFloat
A ==> CheckingArray
--A ==> Array
AF ==> A F
--AAF ==> A AF
BFLS ==> BoundedFiniteLinearStructureType
MI ==> MachineInteger

test(): () ==
	import from MI, F, AF--, AAF
  	n := 10
	--u := [ [ 0.0 for j in 0..n ] for i in 0..n ]	
	u := [ 0.0 for j in 0..n ]
	stdout << u << newline
	--u.1.1 := 1.0
	u.1 := 1.0
	stdout << u << newline

test()


	