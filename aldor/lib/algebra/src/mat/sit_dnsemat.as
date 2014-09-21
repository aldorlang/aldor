------------------------   sit_dnsemat.as   -----------------------
-- Copyright (c) Laurent Bernardin 1994
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994
-----------------------------------------------------------------------------

#include "algebra"
#include "aldorio"

macro {
	I == MachineInteger;
	V == Vector;
	ARR == PrimitiveArray;
}

#if ALDOC
\thistype{DenseMatrix}
\History {Laurent Bernardin}{23 Nov 1994}{created.}
\History {Marco Codutti}{10 May 1995}
	 {Updated for new releases of A\#, \libalgebra and {\em aldoc}.} 
\History{Manuel Bronstein}{1/12/1999}{redesigned for the new matrix category}
\History{Manuel Bronstein}{19/7/2002}{added diagonal(a,n) and scalar?}
\History{Manuel Bronstein}{30/8/2002}{fixed apply(r,c,n,m)}
\Usage   {import from \this~R}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R provides dense mutable matrices with entries in R.
They are $1$--indexed and do not bound check.}
\begin{exports}
\category{\altype{MatrixCategory} R}\\
\end{exports}
#endif

DenseMatrix(R: Join(ArithmeticType, ExpressionType)): MatrixCategory R == add {
	-- data is stored by rows
	Rep == Record(nbrows:I, nbcolumns:I, data:ARR R);

	0:%				== { import from I; zero(1,1); }
	1:%				== { import from I; one 1; }
	local entries(a:%):ARR R	== { import from Rep; rep(a).data; }
	local matrix(n:I,m:I,a:ARR R):%	== { import from Rep; per [n,m,a]; }
	local elements(a:%):(I,I,ARR R)	== { import from Rep; explode rep a; }
	-- copy:% -> %			== map((r:R):R +-> r);
	-- minus!:% -> %			== map!((r:R):R +-> -r);
	-- -:% -> %			== map((r:R):R +-> -r);
	(a:%) + (b:%):%			== zip(+$R, a, b);
	(a:%) - (b:%):%			== zip(-$R, a, b);
	add!(a:%, c:R, b:%):%		== zip!((r:R, s:R):R +-> r+c*s, a, b);
	add!(a:%, b:%):%		== zip!(+$R, a, b);
	minus!(a:%, b:%):%		== zip!(-$R, a, b);
	one(n:I):%			== { import from R; diagonal(1, n); }
	coerce(r:R):%			== { import from I; diagonal(r, 1); }
	zero (n:I,m:I):% == { import from I, R, ARR R; matrix(n,m,new(n*m,0)) }

	dimensions(a:%):(I, I) == {
		import from Rep;
		(rep(a).nbrows, rep(a).nbcolumns)
	}

	bracket(t:Tuple V R):% == {
		import from I, V R;
		zero?(m := length t) or zero?(n:=#(element(t,1))) => zero(0,0);
		assert(n > 0); assert(m > 0);
		a := zero(n, m);
		for j in 1..m repeat {
			v := element(t, j);
			assert(n = #v);
			for i in 1..n for x in v repeat a(i, j) := x;
		}
		a;
	}

	bracket(g:Generator V R):% == {
		import from I, V R, List V R;
		l:List V R := [g];
		zero?(m := #l) or zero?(n := #(first l)) => zero(0, 0);
		assert(n > 0); assert(m > 0);
		a := zero(n, m);
		for j in 1..m for v in l repeat {
			assert(n = #v);
			for i in 1..n for x in v repeat a(i, j) := x;
		}
		a;
	}

	rowCombine!(a:%, f:(R, R) -> R, i1:I, i2:I, j1:I, j2:I):% == {
		import from ARR R;
		(n,m,ar) := elements a;
		assert(i1 > 0); assert(i1 <= n);
		assert(i2 > 0); assert(i2 <= n);
		assert(j1 > 0); assert(j1 <= m);
		assert(j2 > 0); assert(j2 <= m);
		offset1 := m * prev i1;
		offset2 := m * prev i2;
		for j in prev(j1)..prev(j2) repeat
			ar(j+offset1) := f(ar(j+offset1), ar(j+offset2));
		a;
	}

	colCombine!(a:%, f:(R, R) -> R, j1:I, j2:I, i1:I, i2:I):% == {
		assert(i1 > 0); assert(i1 <= numberOfRows a);
		assert(i2 > 0); assert(i2 <= numberOfRows a);
		assert(j1 > 0); assert(j1 <= numberOfColumns a);
		assert(j2 > 0); assert(j2 <= numberOfColumns a);
		for i in i1..i2 repeat a(i, j1) := f(a(i, j1), a(i, j2));
		a;
	}

	rowSwap!(a:%, i1:I, i2:I, j1:I, j2:I):% == {
		import from ARR R;
		i1 = i2 => a;
		(n,m,ar) := elements a;
		assert(i1 > 0); assert(i1 <= n);
		assert(i2 > 0); assert(i2 <= n);
		assert(j1 > 0); assert(j1 <= m);
		assert(j2 > 0); assert(j2 <= m);
		offset1 := m * prev i1;
		offset2 := m * prev i2;
		for j in prev(j1)..prev(j2) repeat {
			t := ar(j+offset2);
			ar(j+offset2) := ar(j+offset1);
			ar(j+offset1) := t;
		}
		a;
	}

	colSwap!(a:%, j1:I, j2:I, i1:I, i2:I):% == {
		j1 = j2 => a;
		assert(i1 > 0); assert(i1 <= numberOfRows a);
		assert(i2 > 0); assert(i2 <= numberOfRows a);
		assert(j1 > 0); assert(j1 <= numberOfColumns a);
		assert(j2 > 0); assert(j2 <= numberOfColumns a);
		for i in i1..i2 repeat {
			t := a(i, j2); a(i, j2) := a(i, j1); a(i, j1) := t;
		}
		a;
	}

	(a:%) = (b:%) : Boolean == {
		import from I, R, ARR R;
		(n,m,aar) := elements a;
		(nb,mb,abr) := elements b;
		(n ~= nb) or (m ~= mb) => false;
		N := prev(n*m);
		for i in 0..N repeat aar.i ~= abr.i => return false;
		true;
	}

	(a:%) * (b:%) : % == {
		import from I, R, ARR R;
		(na,ma,aar) := elements a;
		(nb,mb,abr) := elements b;
		assert(ma=nb);
		rr:ARR R := new(na*mb);
		ri:I := 0;
		ai:I := 0;
		for i in 0..prev(na) repeat {
			for j in 0..prev(mb) repeat {
				e:R := 0;
				bj := j;
				for aj in ai..ai+prev(ma) repeat {
					e := e + aar.aj * abr.bj;
					bj := bj + mb;
				}
				rr.ri := e;
				ri := next ri;
			}
			ai := ai + ma;
		}
		matrix(na,mb,rr);
	}

	(a:%) * (v:V R):V R == {
		import from I, R, ARR R;
		(n,m,ar) := elements a;
		assert(m=#v);
		av:V R := zero n;
		ri:I := 1;
		ai:I := 0;
		for i in 0..prev(n) repeat {
			e:R := 0;
			for x in v repeat {
				e := e + ar.ai * x;
				ai := next ai;
			}
			av.ri := e;
			ri := next ri;
		}
		av;
	}

	local zip!(f:(R, R) -> R, a:%, b:%):% == {
		import from I, R, ARR R;
		(n,m,aar) := elements a;
		(nb,mb,abr) := elements b;
		assert(n=nb); assert(m=mb);
		N := prev(n*m);
		for i in 0..N repeat aar.i := f(aar.i, abr.i);
		a;
	}

	local zip(f:(R, R) -> R, a:%, b:%):% == {
		import from I, R, ARR R;
		(n,m,aar) := elements a;
		(nb,mb,abr) := elements b;
		assert(n=nb); assert(m=mb);
		rr:ARR R := new(nm := n*m);
		for i in 0..prev(nm) repeat rr.i := f(aar.i, abr.i);
		matrix(n,m,rr);
	}

	apply(a:%, r:Array I, c:Array I):% == {
		import from I;
		b := zero(n := #r, m := #c);
		for i in 1..n repeat for j in 1..m repeat
			b(i, j) := a(r(prev i), c(prev j));
		b;
	}

	apply (a:%,i:I,j:I) : R == {
		import from I, ARR R;
		(n,m,ar) := elements a;
		assert(i > 0); assert(i <= n);
		assert(j > 0); assert(j <= m);
		ar(prev(j)+prev(i)*m);
	}

	set!(a:%,i:I,j:I,r:R):R == {
		import from I, ARR R;
		(n,m,ar) := elements a;
		assert(i > 0); assert(i <= n);
		assert(j > 0); assert(j <= m);
		ar(prev(j)+prev(i)*m) := r;
	}

	map!(f:R -> R)(a:%):% == {
		import from I, ARR R;
		(n,m,ar) := elements a;
		N := prev(n*m);
		for i in 0..N repeat ar.i := f(ar.i);
		a;
	}

	map(f:R -> R)(a:%):% == {
		import from I, ARR R;
		(n,m,ar) := elements a;
		rr:ARR R := new(nm := n*m);
		for i in 0..prev(nm) repeat rr.i := f(ar.i);
		matrix(n,m,rr);
	}

	if R has Ring then {
		random (n:I,m:I):% == {
			import from R;
			rr:ARR R := new(nm := n*m);
			for i in 0..prev(nm) repeat rr.i := random();
			matrix(n,m,rr);
		}
	}

	apply(a:%,r:I,c:I,n:I,m:I) : % == {
		import from ARR R;
		b := matrix(n,m,new(n*m));
		for i in 1..n repeat {
			for j in 1..m repeat b(i,j) := a(r+prev i,c+prev j);
		}
		b;
	}

	setMatrix!(a:%,r:I,c:I,b:%) : % == {
		import from I, ARR R;
		(n,m) := dimensions b;
		for i in 1..n repeat {
			for j in 1..m repeat a(r+prev i,c+prev j) := b(i,j);
		}
		b;
	}

	transpose!(a:%):% == {
		import from I;
		(n,m) := dimensions a;
		assert(n = m);
		for i in 1..n repeat {
			for j in next i..m repeat {
				t := a(i, j); a(i,j) := a(j,i); a(j, i) := t;
			}
		}
		a;
	}

	transpose (a:%) : % ==  {
		import from I, ARR R;
		(n,m) := dimensions a;
		b := matrix(m,n,new(n*m));
		for i in 1..n repeat {
			for j in 1..m repeat b(j,i) := a(i,j);
		}
		b;
	}

	transpose (v:V R) : % ==  {
		import from I, ARR R;
		ar:ARR R := new(n := #v);
		for i in 0..prev n for x in v repeat ar.i := x;
		matrix(1,n,ar);
	}

	diagonal?(a:%) : Boolean == {
		import from I, R;
		(n,m) := dimensions a;
		n ~= m => false;
		for i in 1..n repeat {
			for j in 1..m | i ~= j repeat
				~zero?(a(i,j)) => return false;
		}
		true;
	}

	scalar?(a:%):Boolean == {
		import from I, R;
		(n,m) := dimensions a;
		n ~= m => false;
		zero? n => true;
		p := a(1,1);
		for i in 1..n repeat {
			i > 1 and a(i,i) ~= p => return false;
			for j in 1..m | i ~= j repeat
				~zero?(a(i,j)) => return false;
		}
		true;
	}

	zero!(a:%) : () == {
		import from I, R, ARR R;
		(n,m,ar) := elements a;
		N := prev(n*m);
		for i in 0..N repeat ar.i := 0;
	}

	zero?(a:%) : Boolean == {
		import from I, R, ARR R;
		(n,m,ar) := elements a;
		N := prev(n*m);
		for i in 0..N repeat ~zero?(ar.i) => return false;
		true;
	}

	if R has SerializableType then {
		(port:BinaryWriter) << (a:%):BinaryWriter == {
			import from I, ARR R;
			(n, m) := dimensions a;
			port := port << n << m;
			write(port, entries a, n * m);
		}

		<< (port:BinaryReader):% == {
			import from I, ARR R;
			n:I := << port;
			m:I := << port;
			matrix(n, m, read(port, n * m));
		}
	}

	-- TEMPORARY: COPIED FROM sit_matcat.as BECAUSE OF COMPILER BUG
	commutative?:Boolean	== false;
	copy:% -> %		== map((r:R):R +-> r);
	-: % -> %		== map((r:R):R +-> -r);
	minus!: % -> %		== map!((r:R):R +-> -r);
	-- map(f:R -> R):% -> %	== (a:%):% +-> map!(f)(copy a);
	colSwap!(a:%,i:I,j:I):%	== colSwap!(a,i,j,1,numberOfRows a);
	rowSwap!(a:%,i:I,j:I):%	== rowSwap!(a,i,j,1,numberOfColumns a);
	numberOfRows(a:%):I	== { (r,c) := dimensions a; r; }
	numberOfColumns(a:%):I	== { (r,c) := dimensions a; c; }
	tensor(a:%, b:%):%	== transpose [tensorGen(a, b)];
	(a:%)^(n:I):%		== { import from Integer; a^(n::Integer); }

	colCombine! (a:%,c1:R,i1:I,c2:R,i2:I) : % ==
		colCombine! (a,c1,i1,c2,i2,1,numberOfRows a);

	colCombine! (a:%,c1:R,i1:I,c2:R,i2:I,j1:I,j2:I) : % ==
		colCombine! (a,(x:R,y:R):R +-> c1*x+c2*y, i1,i2,j1,j2);

	colCombine! (a:%,f:(R,R)->R,i1:I,i2:I) : % ==
		colCombine! (a,f,i1,i2,1,numberOfRows a);

	rowCombine! (a:%,c1:R,i1:I,c2:R,i2:I) : % ==
		rowCombine! (a,c1,i1,c2,i2,1,numberOfColumns a);

	rowCombine! (a:%,c1:R,i1:I,c2:R,i2:I,j1:I,j2:I) : % ==
		rowCombine! (a,(x:R,y:R):R +-> c1*x+c2*y, i1,i2,j1,j2);

	rowCombine! (a:%,f:(R,R)->R,i1:I,i2:I) : % ==
		rowCombine! (a,f,i1,i2,1,numberOfColumns a);

	map(f:R -> R)(v:V R):% == {
		n := #v;
		a := zero(n, n);
		for j in 1..n repeat a(1,j) := v.j;
		for i in 2..n repeat for j in 1..n repeat
			a(i, j) := f a(prev i, j);
		a;
	}

	square?(a:%):Boolean == {
		import from I;
		(n, m) := dimensions a;
		n = m;
	}

	column(a:%, j:I):V R == {
		(n, m) := dimensions a;
		assert(j > 0); assert(j <= m);
		[a(i, j) for i in 1..n];
	}

	row(a:%, i:I):V R == {
		(n, m) := dimensions a;
		assert(i > 0); assert(i <= n);
		[a(i, j) for j in 1..m];
	}

	columns(a:%):Generator V R == generate {
		import from I;
		n := numberOfColumns a;
		for i in 1..n repeat yield column(a, i);
	}

	rows(a:%):Generator V R == generate {
		import from I;
		n := numberOfRows a;
		for i in 1..n repeat yield row(a, i);
	}

	(a:%) ^ (n:Integer):% == {
		assert(n >= 0);
		assert(square? a);
		zero? n => one numberOfRows a;
		one? n => a;
		b := copy a;
		for i in 2..n repeat b := times!(b, a);
		b;
	}

	(c:R) * (a:%):% == {
		zero? c => zero dimensions a;
		one? c => a;
		c = -1 => -a;
		map((r:R):R +-> c * r)(a);
	}

	times!(c:R, a:%):% == {
		zero? c => map!((r:R):R +-> 0)(a);
		one? c => a;
		c = -1 => minus! a;
		map((r:R):R +-> c * r)(a);
	}

	companion (l:V R, a:R == 1):% == {
		import from I, R;
		n := #l;
		assert(n>0);
		o := zero(n,n);
		for i in 1..prev n repeat {
			o(i,n) := l.i;
			o(next i, i) := a;
		}
		o(n, n) := l.n;
		o;
	}

	diagonal (a:R, n:I): % == {
		assert(n>0);
		o := zero(n,n);
		for i in 1..n repeat o(i,i) := a;
		o;
	}

	diagonal (l:V R): % == {
		import from I;
		n := #l;
		assert(n>0);
		o := zero(n,n);
		for i in 1..n repeat o(i,i) := l.i;
		o;
	}

	one?(a:%):Boolean == {
		import from I, R;
		(r, c) := dimensions a;
		r > 0 and c > 0 and one?(a(1, 1)) and scalar? a;
	}

	extree (a:%) : ExpressionTree == {
		import from I, R, V R, List ExpressionTree;
		(r, c) := dimensions a;
		l := [extree r, extree c];
		for v in rows a repeat l := append!(l, [extree x for x in v]);
		ExpressionTreeMatrix l;	
	}

	local tensorGen(a:%, b:%):Generator V R == generate {
		import from V R;
		for u in rows a repeat for v in rows b repeat yield tensor(u,v);
	}

	if R has DifferentialRing then {
		wronskian(v:V R):% == {
			import from R;
			map(differentiate) v;
		}
	}

	if R has Ring then {
		random():% == {
			import from I;
			random(1+random()$I mod 100, 1+random()$I mod 100);
		}
	}
}

#if ALDORTEST
-------------------------   test for matdense.as   -------------------------
#include "algebra"
#include "aldortest"

macro {
	Z  == Integer;
	V  == Vector Z;
	M == DenseMatrix Z;
}

local testConstruction(): () == {
    import from Assert Integer;
    import from Assert MachineInteger;
    import from MachineInteger;
    import from Z, V;
    m: M := [[1,2,3]];
    (nr, nc) := dimensions m;
    stdout << "Rows: " << nr << " Cols: " << nc << " --> " << m << newline;
--    assertEquals(1, nr);
--    assertEquals(3, nc);

    m := [[1,2,3],[4,5,6]];
    (nr, nc) := dimensions m;
    stdout << "Rows: " << nr << " Cols: " << nc << " --> " << m << newline;
--    assertEquals(2, nr);
--    assertEquals(3, nc);

}

local basic():Boolean == {
	import from Z, V, M, MachineInteger;

	a:M := transpose [[1,2,3],[4,5,6],[7,8,9]];
	b:M := transpose [[9,8,7],[6,5,4],[3,2,1]];
	v:V := [1,2,3];
	t:M := [[1,1,1],[1,1,1],[1,1,1]];
	a := add!(a,b);
	a ~= times!(10,t) => false;
	w := b*v;
	w ~= [46,28,10] => false;
	7*one(5) ~= map!((x:Z):Z +-> 7*x)(one 5) => false;
	b := random(5,7);
	b = transpose transpose b;
}

local kernel():Boolean == {
	import from MachineInteger, Z, V, M, LinearAlgebra(Z, M);

	a:M := [[1,2,-1,3],[3,4,-3,7],[5,6,-5,11],[7,8,-7,15]];
	ns := kernel a;
	r  := rank a;
	(numberOfColumns a - r) ~= numberOfColumns ns => false;
	r = 2 and zero?(a * ns);
}

testConstruction();

stdout << "Testing sit__dnsemat..." << endnl;
aldorTest("basic operations", basic);
aldorTest("kernel", kernel);
stdout << endnl;
#endif
