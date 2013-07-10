--> testcomp
--> testrun -l axllib
--> testrun -O -l axllib
--> testint
--> testint -O

#include "axllib"

define ZeroDivide(R: Ring): Category == Exception with {
	n: R;
} 

define Memory: Category == Exception with; 

define Memory: Memory@Category == add; 

ZeroDivide(R: Ring)(v: R == 1+1): ZeroDivide(R) == add {
	n: R == v;
} 

foo(n: Integer): () == {
	print << n;
	x := try myDivide(4, n) catch E in {
		E has ZeroDivide(Integer) => {
			print << "  Div by zero, val: " << n$E;
			215
		}
		true => throw E;
		never;
	} finally {
		print << "." << newline;
	}
--	try myDivide(1,1) catch finally print << "xxx" << newline;
	print << "Result: " << x << newline;
}


myDivide(n: Integer, m: Integer): Integer == {
	zero? m => throw ZeroDivide(Integer) n;
	m = 1   => throw (ZeroDivide Integer)();
	m = 33  => throw Memory;
	n quo m;
}

bar(): () == {
	import from List Integer;
	l := [0,1,17,33];
	for x in l repeat {
		try {
			foo(x);
			1
		} catch E in {
			if E has Memory then print << "Yow!" << newline;
			44;
		} finally {
			print << "Dunnit" << newline;
		}
	}
}

bar();

#if 0
myDivide(n: Integer, m: Integer): Integer throw (ZeroDivide(Integer), Memory) == {
	zero? m => throw ZeroDivide(Integer) n;
	n = 1 => throw (add@Memory);
	n quo m;
	
}

#endif
