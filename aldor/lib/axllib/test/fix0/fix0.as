-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testcomp
--> testrun -O -l axllib

#include "axllib"

MyList(X: BasicType): FiniteAggregate X with {
	first: % -> X;
	rest:  % -> %;
	cons: (X, %) -> %;
	make:  X -> %;
	export from X;
} == add {
	import from FormattedOutput;

	NoValue ==> Integer;
	noValue ==> 0$Integer;
	U == Union(n: NoValue, r: Rep);
	Rep == Record(a: X, u: U);

	import from Rep;

	first(x: %): X == (rep(x)).a;
	rest(x: %):  % == per(rep(x).u.r);

	empty?(x: %): Boolean == false;
	last?(x: %):  Boolean == rep(x).u case n;

	cons(z:X , x: %): % == per([z, [rep x]]);
	make(z: X): 	  % == per([z, [noValue]]);
	generator(x0: %): Generator X == {
		generate {
		        x := x0;
			while not last? x repeat {
				yield first x;
				x := rest x;
			}
			yield first x;
		}
	}
	
	map(f: X->X, x: %): % == {
		last? x => per([f first x, [noValue]]);
		cons(f first x, map(f, rest x))
	}

	(p: TextWriter) << (x: %): TextWriter == {
		import from List X;
		(print("[new ~a ]", p))(<< [a for a in x])
	}

	sample: % == per [ sample$X , [noValue]];
	(a: %) = (b: %): Boolean == false;
	#(x: %): SingleInteger == {
		n: SingleInteger := 0;
		for e in x repeat n := n + 1;
		n
	}
}


T1(): () == {
	import from MyList Integer, SingleInteger;

	l1 := cons(1, cons(2, cons(3, make 4)));
	print << l1 << newline 
	      << (#l1) << " " << map(-, l1) << newline;
}

T1();
