--> testint
--> testrun -Q2 -laxllib
--> testrun -Q1 -laxllib
--@ Bug Number:  bug1028.as 
--@ Fixed  by:  pab   
--@ Tested by:  opt3.as 
--@ Summary:    Fixed buggette in dead assignment elimination with mult. values 

-- Command line: axiomxl -Q2 optbug.as
-- Version: 1.1.3
-- Original bug file name: optbug.as

------------------------   optbug.as   -----------------------
--
-- This is a small example of our major problem with the optimizer
--
-- % axiomxl -Fx -Q1 optbug.as
-- % optbug
-- % axiomxl -Fx -Q2 optbug.as
-- % optbug
-- Bus error
--
-- Occurs in many places, seems to be related to defaults returning Tuples.
--

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

boom():SI == {
	import from Integer, Matrix Integer;
	rank new(2, 2, 0);
}

boom();

