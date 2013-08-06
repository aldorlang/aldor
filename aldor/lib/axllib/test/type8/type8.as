-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -O -l axllib
--> testcomp -O

#include "axllib"

Poly3(X: Ring, v1: String == "a", v2: String == "b"): Ring == X add {
	import from X;
	(<<)(out: TextWriter, x: %): TextWriter == 
		print("(~a ~a ~a)")(<< v1, << v2, << x pretend X)
};

h(R: Ring): () == {
	import from Poly3(R);
	print << 1$Poly3(R);
	print << newline;
}
i(R: Ring): () == {
	import from Poly3(R, "c");
	print << 1$Poly3(R, "c");
	print << newline;
}
j(R: Ring): () == {
	import from Poly3(R, "c", "d");
	print << 1$Poly3(R, "c", "d");
	print << newline;
}

k(R: Ring): () == {
	import from Poly3(R, v2 == "d");
	print << 1$Poly3(R, v2=="d");
	print << newline;
}

l(R: Ring): () == {
	import from Poly3(R, v2 == "d", v1 == "f");
	print << 1$Poly3(R, v2=="d", v1=="f");
	print << newline;
}
TT(): () == {
	import from List (Ring -> ());
	for fn in [h, i, j, k, l] repeat {
		fn(Integer);
		fn(Ratio Integer);
	}
}



