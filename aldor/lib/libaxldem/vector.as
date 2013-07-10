-- vec.as

#include "axllib.as"

macro {
	SI    == SingleInteger;
	B     == Boolean;
}

Vector(S: Ring): Join(BasicType, Aggregate S) with {
	new:   (SI, S)    -> %;
	apply: (%, SI)    -> S;
	set!:  (%, SI, S) -> S;
	empty: ()         -> %;
	+: (%, %)         -> %;
	-: (%, %)         -> %;
	-: %              -> %;
	*: (%, %)         -> %;
	*: (S, %)         -> %;
	dot: (%, %)       -> S;
	vector : Tuple S  -> %;
	vector : Generator S -> %;
}
== Array S add {
	Rep ==> Array S;
	import from Rep, S;
	import from SI;

	new(size: SI, val:S):% == per new(size, val);
	new(size: SI):% == new(size, 0);
	apply(v: %, index:SI):S== rep(v).index;
	set!(v: %, index:SI, val:S):S== rep(v).index := val;
	#(v:%):SI == # rep v;
	empty():% == new(0);
	empty?(v:%):B == #v = 0;
	vector(ts : Tuple S): % == per array ts;
	vector(its : Generator S): % == per array its;

	map(f:S-> S, v: %):% == {
		 n:SI := #v;
		 vv:% := new(n);
		 for i in 1..n repeat
		 	vv.i := f v.i;
		 vv
	}

	minimum(n1:SI, n2:SI):SI == {
		 n1 < n2 => n2;
		 n2
	}

	map(f:(S,S) -> S, v1:%, v2:%) :% == {
		 n:SI := minimum(#v1,#v2);
		 vv:% := new(n);
		 for i in 1..n repeat
		 	vv.i := f(v1.i, v2.i);
		 vv
	}

	-(v:%):%          == map(-,v);
	(v1:%) + (v2:%):% == map(+,v1,v2);
	(v1:%) - (v2:%):% == map(-,v1,v2);
	(v1:%) * (v2:%):% == map(*,v1,v2);
	(n: S) * (v: %):% == map((x:S):S+-> n*x, v);

	dot(v1:%,v2:%):S== {
		 n:SI := minimum(#v1, #v2);
		 vv:% := new(n);
		 sum:S:=0;
		 for i: SI in 1..n repeat
		 	sum := sum + v1.i * v2.i;
		 sum
	}

	apply: (%, %) -> S == dot;

	(v1:%) = (v2:%):B == {
		 (n:SI:=#v1) ~= #v2 => false;
		 for i: SI in 1..n repeat
		 	v1.i ~= v2.i => return false;
		 true
	}

	generator(v: %): Generator S == generate {
		 for i: SI in 1..#v repeat
		 	yield v.i;
	}

	(p: TextWriter) << (v: %): TextWriter == {
		  p << "[";
		  if not empty? v then {
		  	p << v.1;
		  	for i in 2..#v repeat p << ", " << v.i
		  }
		  p << "]"
	}
}
