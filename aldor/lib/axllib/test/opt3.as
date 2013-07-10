-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------   optbug.as   -----------------------
-- originally bug1028.as
--> testrun  -O -l axllib
--> testcomp -O

#include "axllib.as"

macro {
	SI == SingleInteger;
	ARR == PrimitiveArray SI;
}

MatCategory0(R: Ring): Category == BasicType with {
	cols:		% -> SI;
	rows:		% -> SI;
	dimensions:     % -> (SI,SI);
	if R has EuclideanDomain then rank: % -> SI;
}

LinearAlgebra(R:EuclideanDomain,P:MatCategory0 R):with { rank:P->SI } == add {
	rank (a:P) : SI == {
		import from ARR;
		(n,m)   := dimensions a;
		(r,d,c) := (0, 0, new 1);
		r;
	}
}

MatCategory(R: Ring): Category == MatCategory0 R with {
    if R has EuclideanDomain then {
         macro LA == LinearAlgebra(R pretend EuclideanDomain,  %);
         default rank(a:%):SI == rank(a)$LA;
    }
}

Matrix(R: Ring): MatCategory R with { new: (SI,SI,R) -> % } == add {
	DT  ==> PrimitiveArray R;
	Rep ==> Record(nbrows:SI,nbcolumns:SI,data:DT);

	import from R, SI, ARR, Rep;

	if R has EuclideanDomain then {
          macro LA == LinearAlgebra(R pretend EuclideanDomain,  %);
          rank(a:%):SI == rank(a)$LA;
	}

	sample:% == new(2,2,0);
	new (n:SI,m:SI,e:R) : % == { per [n,m,new(n*m,e)]; }
	rows    (a:%) : SI == rep(a).nbrows;
	cols (a:%) : SI == rep(a).nbcolumns;
	entries (a:%) : DT == rep(a).data;
	(port: TextWriter) << (a:%) : TextWriter == port;
	dimensions (a:%) : (SI,SI)    == (rows(a),cols(a));
	(a:%) = (b:%) : Boolean == false;
}

boom():() == {
	import from Integer, Matrix Integer, SingleInteger;
	print << rank new(2, 2, 0) << newline;
}

boom();
