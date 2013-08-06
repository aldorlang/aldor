--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA06063; Fri, 22 Sep 95 15:46:17 BST
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.00703-0@sun3.nsfnet-relay.ac.uk>;
--*           Fri, 22 Sep 1995 15:42:08 +0100
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id QAA24590 
--*           for <ax-bugs@nag.co.uk>; Fri, 22 Sep 1995 16:41:42 +0200
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id QAA16385 
--*           for ax-bugs@nag.co.uk; Fri, 22 Sep 1995 16:41:42 +0200
--* Date: Fri, 22 Sep 1995 16:41:42 +0200
--* Message-Id: <199509221441.QAA16385@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [2] optimizer bug -Q1 ok, -Q2 = bus error

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

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
