-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
--> testrun -O  -l axllib
--> testcomp -O
#pile
S ==> SingleInteger;
Bool ==> Boolean;

-- give list of mapping a work-out
T1(): () == {
	import from S;
	import from List (S->S);

	l := [ +, -, (x: S): S +-> x*2 +2 ];

	for fn in l repeat
		print<<fn 7<<newline;
}
T1();

T2(): () == {
	import from List ((S, S) -> Bool);
	import from S;
	import from List S;
	l := [ <, > , <=, >=, =, ~=];
	m := [i for i in 1..10];

	for fn in l repeat
		for i1 in 1..10 
		for i2 in 10..1 by -1 repeat
			print<<i1<<i2<<fn(i2, i2)<<newline;
}
T2();

T3(): () == {
	import from List (S -> (S, Bool));
	import from List ((S, S) -> Bool);
	import from S;
	
	l := [ ((x: S): (S, Bool) +-> (x, fn(x, 0))) 
			for fn in [  <, > , <=, >=, =, ~= ] ];
	for fn in l repeat
		for z in -3..3 repeat {
			(a, b) := fn(z);
			print<<b<<" "<<a<<newline;
		}

}

T3();
