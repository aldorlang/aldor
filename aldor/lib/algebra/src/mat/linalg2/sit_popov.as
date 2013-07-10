---------------------------- sit_popov.as ---------------------------------
-- Copyright (c) Thom Mulders 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	ARR == PrimitiveArray I;
	Sn == Permutation;
	V == Vector;
}

#if ALDOC
\thistype{UnivariatePolynomialPopovLinearAlgebra}
\History{Thom Mulders}{July 2000}{created for determinant}
\History{Thom Mulders}{24/11/2000}{added hermite and rowEchelon}
\History{Manuel Bronstein}{29/11/2000}{added kernel}
\Usage{import from \this(F, FX, M)}
\Params{
{\em F} & \altype{Field} & The coefficient field\\
{\em FX} & \altype{UnivariatePolynomialAlgebra} F & Polynomials over $F$\\
{\em M} & \altype{MatrixCategory} FX & A matrix type over $FX$\\
}
\Descr{\this(F, FX, M) provides basic linear algebra functionalities
using weak Popov forms for matrices over $FX$.}
% \Remarks{ The efficiency of Popov form methods vary greatly depending on
% the coefficient field, so you should not call them directly but rather
% use functions from \alfunc{LinearAlgebra} instead, which selects the
% appropriate algorithms.}
\begin{exports}
\alfunc{LinearAlgebra}{determinant}: & M $\to$ FX & Determinant\\
\alexp{hermite}: & M $\to$ M & Hermite form\\
\alfunc{LinearAlgebra}{kernel}: & M $\to$ M & Kernel\\
\alfunc{LinearAlgebra}{maxInvertibleSubmatrix}:
& M $\to$ (AZ, AZ) & Maximal minor\\
\alexp{popov}: & M $\to$ M & weak Popov form\\
\alfunc{LinearAlgebra}{rank}: & M $\to$ \altype{MachineInteger} & Rank\\
\alfunc{LinearAlgebra}{span}: & M $\to$ AZ & Span\\
\end{exports}
\begin{alwhere}
AZ &==& \altype{Array} \altype{MachineInteger}\\
\end{alwhere}
#endif

UnivariatePolynomialPopovLinearAlgebra(F:Field,
		P:UnivariatePolynomialAlgebra0 F, M:MatrixCategory P): with {
	determinant: M -> P;
	hermite: M -> M;
#if ALDOC
\alpage{hermite}
\Usage{\name~a}
\Signature{M}{M}
\Params{ {\em a} & M & A matrix\\ }
\Retval{ Returns the Hermite form of $a$.}
#endif
	kernel: M -> M;
	maxInvertibleSubmatrix: M -> (Array I, Array I);
	popov: M -> M;
	rank: M -> I;
	span: M -> Array I;
} == add {
	import from I, Z, ARR, P, M, F;

	hermite(A:M):M	== hermite0(A, numberOfRows A);
	popov(A:M):M	== popov0(A, numberOfRows A);
	rank(A:M):I	== rank0(A, numberOfRows A);

	-- makes a copy of each individual entry of A
	local deepCopy(A:M):M == deepCopy!(zero dimensions A, A);

	-- stores a copy of each individual entry of A into B
	local deepCopy!(B:M, A:M):M == {
		(n, m) := dimensions A;
		assert(n = numberOfRows B);
		assert(m = numberOfColumns B);
		for i in 1..n repeat for j in 1..m repeat B(i,j) := copy A(i,j);
		B;
	}

	-- Weak Popov form up to row n and up to column m.
	-- Elimination performed up to column h.
	-- Returns r, the rank of the principal nxm block of A.
	-- as well as the modified permutation Q
	-- Rows 1 upto r in weak Popov form, other rows are zero
	-- Matrix and indices represented by A, index and Q, i.e.
	-- entry (i,j) is stored in A(Q.i,j) and index of row i is
	-- in index(Q.i).
	-- destroys A and the individual entries of A
	local weakPopov!(A:M,nn:I,Q:Sn nn,index:ARR,n:I,m:I,h:I):(I,Sn nn) == { 
		import from P;
		r:I := 0;
		for i in 1..n repeat
			(r, Q) := increasePopov!(A,nn,Q,index,r,i,m,h);
		(r, Q);
	}

	-- Weak Popov form up to column m, elimination performed
	-- up to colun h.
	-- Rows 1 upto high in weak Popov form
	-- Adds row i to weak Popov form.
	-- New weak Popov form in rows 1..high(+1)
	-- Returns number of nonzero rows in weak Popov form (= high(+1))
	-- as well as the modified permutation Q
	-- Matrix and indices represented by A, index and Q.
	-- destroys A and the individual entries of A
	local increasePopov!(A:M,n:I,Q:Sn n,index:ARR,high:I,i:I,m:I,h:I):
		(I,Sn n) == {
		TRACE("popov::increasePopov!: A = ", A);
		TRACE("popov::increasePopov!: n = ", n);
		TRACE("popov::increasePopov!: high = ", high);
		TRACE("popov::increasePopov!: i = ", i);
		TRACE("popov::increasePopov!: m = ", m);
		TRACE("popov::increasePopov!: h = ", h);
#if TRACE
		stderr << "popov::increasePopov!: Q(1..n) = ";
		for ii in 1..n repeat stderr << Q.ii << " ";
		stderr << endnl;
		stderr << "popov::increasePopov!: index(1..n) = ";
		for ii in 1..n repeat stderr << index ii << " ";
		stderr << endnl;
#endif
		import from F, P;
		l := high + 1;
		TRACE("popov::increasePopov!: l = ", l);
		if i~=l then Q := transpose!(Q,i,l);
#if TRACE
		stderr << "popov::increasePopov!: Q(1..n) = ";
		for ii in 1..n repeat stderr << Q.ii << " ";
		stderr << endnl;
#endif
		repeat {
			index(Q.l) := pivotIndex(A,Q.l,m);
			TRACE("popov::increasePopov!:index(Q.l) = ",index(Q.l));
			zero? index(Q.l) => return(high, Q);
			k:I := 1;
			while index(Q.k)~=index(Q.l) repeat { k := next k }
			TRACE("popov::increasePopov!: k = ", k);
			k=l => return(next high, Q);
			if degree A(Q.k,index(Q.k)) >
				degree A(Q.l,index(Q.l)) then
							Q := transpose!(Q,k,l);
#if TRACE
			stderr << "popov::increasePopov!: Q(1..n) = ";
			for ii in 1..n repeat stderr << Q.ii << " ";
			stderr << endnl;
#endif
			elementaryOperation!(A,Q.k,Q.l,index(Q.k),h);
			TRACE("popov::increasePopov!: A = ", A);
		}
	}

	-- Performs elementary operation from row i onto row k with
	-- pivot in column j.
	-- Elimination performed up to column m.
	-- Degree A(i,j) <= degree A(k,j).
	-- destroys A and the individual entries of A
	local elementaryOperation!(A:M, i:I, k:I, j:I, m:I): () == {
		import from F, P;
		c := -leadingCoefficient A(k,j)/leadingCoefficient A(i,j);
		d := degree A(k,j) - degree A(i,j);
		for l in 1..m repeat
			A(k,l) := add!(A(k,l),c,d,A(i,l),d,d+degree A(i,l));
	}

	-- Returns index of row i in A up to column m.
	-- Returns 0 when row i is zero up to column m.
	local pivotIndex(A:M, i:I, m:I): I == {
		import from Z, P;
		index:I := 0;
		deg:Z := -1;
		for j in 1..m repeat {
			d := degree A(i,j);
			if d >= deg then {
				index := j;
				deg := d;
			}
		}
		deg=-1 => 0;
		index;
	}

	determinant(A:M): P == {
		assert(square? A);
		determinant!(deepCopy A, numberOfRows A);
	}

	local popov0(A:M, n:I):M == {
		import from Sn n;
		index:ARR := new next n;  -- ignore index.0
		m := numberOfColumns A;
		AA := deepCopy A;
		weakPopov!(AA,n,1,index,n,m,m);
		AA;
	}

	local rank0(A:M, n:I):I == {
		import from Sn n;
		index:ARR := new next n;  -- ignore index.0
		m := numberOfColumns A;
		(rk, Q) := weakPopov!(deepCopy A,n,1,index,n,m,m);
		rk;
	}

	span(A:M):Array I == {
		(a, p, r, st, d) := rowEchelon A;
		[st i for i in 1..r];
	}

	maxInvertibleSubmatrix(A:M):(Array I, Array I) == {
		(a, p, r, st, d) := rowEchelon A;
		([p i for i in 1..r], [st i for i in 1..r]);
	}

	kernel(A:M):M == {
		import from Backsolve(P, M);
		(a, p, r, st, d) := rowEchelon A;
		k:List V P := empty;
		(n, m) := dimensions a;
		for j in 1..prev(st 1) repeat k:= cons(backsolve(a,p,st,0,j),k);
		for i in 1..prev r repeat
			for j in next(st i)..prev st(next i) repeat
				k:=cons(backsolve(a,p,st,i,j),k);
		if r > 0 then for j in next(st r)..m repeat
			k:=cons(backsolve(a,p,st,r,j),k);
		[v for v in k];
	}

	-- destroys A and the individual entries of A
	local determinant!(A:M, n:I): P == {
		import from P, Sn n;

		index:ARR := new next n;	-- just ignore index.0

		(r, Q) := weakPopov!(A,n,1,index,n,n-1,n);
		r < n-1 => return 0;
		A(Q.n,n)=0 => return 0;
		 
		d := A(Q.n,n);
		for i in n-1..1 by -1 repeat {
			k:I := 1;
			while index(Q.k)~=i repeat { k := k+1;}
			Q := transpose!(Q,i,k);
			(ignore, Q) := increasePopov!(A,n,Q,index,i-1,i,i-1,i);
			d := d*A(Q.i,i);
		}
		ss := sign Q;
		ss=1 => d;
		-d;
	}

	local hermite0(A:M, n:I):M == {
		import from Sn n;

		TRACE("popov::hermite0: A = ", A);
		B := deepCopy A;
		TRACE("popov::hermite0: B = ", B);
		(H,Q,index) := hermite!(B,n);
		TRACE("popov::hermite0: H = ", H);
#if TRACE
		stderr << "popov::hermite0: Q(1..n) = ";
		for i in 1..n repeat stderr << Q.i << " ";
		stderr << endnl;
		stderr << "popov::hermite0: index(1..n) = ";
		for i in 1..n repeat stderr << index i << " ";
		stderr << endnl;
#endif

		HH := zero(n,m := numberOfColumns A);
		for i in 1..n | ~zero?(index(Q.i)) repeat {
			assert(~zero?(H(Q.i,index(Q.i))));
			c := inv leadingCoefficient(H(Q.i,index(Q.i)));
			for j in index(Q.i)..m repeat HH(i,j) := c*H(Q.i,j);
		}
		HH;
	}

	--                                   h
	--      <-------------------------------------------------------->
	--               m 
	--      <----------------->
	--       _________________________________________________________
	--      |                  | |     | |      | |         | |      |
	--      |                  | |     | |      | |         | |      |
	--    t |                  | |     | |      | |         | |      | n
	--      |__________________|_|_____|_|______|_|_________|_|______|
	--                                 |_|______|_|_________|_|______|   
	--                                          |_|_________|_|______| 

	-- destroys A and the individual entries of A
	local hermite!(A:M, n:I):(M,Sn n,ARR) == {
		import from Sn n;
		TRACE("popov::hermite!: A = ", A);

		m := numberOfColumns A;
		index:ARR := new next n;  -- ignore index.0

		-- Eliminate dependent rows
		(rk, Q) := weakPopov!(A,n,1,index,n,m,m);
		-- Now all nonzero rows of A are independent
		TRACE("popov::hermite!: A after weakPopov = ", A);
#if TRACE
		stderr << "popov::hermite!: Q(1..n) = ";
		for i in 1..n repeat stderr << Q.i << " ";
		stderr << endnl;
		stderr << "popov::hermite!: index(1..n) = ";
		for i in 1..n repeat stderr << index i << " ";
		stderr << endnl;
#endif

		-- Extract nonzero rows
		t:I := 0;
		Q := 1;
		for i in 1..n repeat {
			ind := pivotIndex(A,i,m);
			if ind~=0 then {
				t := next t;
				Q := transpose!(Q,t,i);
				index(Q.t) := ind;
			}
		}
		nn := t;
		TRACE("popov::hermite!: nn = t = ", nn);
#if TRACE
		stderr << "popov::hermite!: Q(1..n) = ";
		for i in 1..n repeat stderr << Q.i << " ";
		stderr << endnl;
		stderr << "popov::hermite!: index(1..n) = ";
		for i in 1..n repeat stderr << index i << " ";
		stderr << endnl;
#endif

		for j in m..1 by -1 repeat {
			TRACE("popov::hermite!: j = ", j);
			for i in 1..t repeat
				extendedIncreasePopov!(A, mapping Q, index, _
							i, t, nn, j-1, m);
			TRACE("popov::hermite!: A after extendedIncreasePopov = ", A);
#if TRACE
			stderr << "popov::hermite!: Q(1..n) = ";
			for i in 1..n repeat stderr << Q.i << " ";
			stderr << endnl;
			stderr << "popov::hermite!: index(1..n) = ";
			for i in 1..n repeat stderr << index i << " ";
			stderr << endnl;
#endif

			i:I := 1;
			while i<=t and index(Q.i)~=0 repeat { i := next i;}
			TRACE("popov::hermite!: i = ", i);
			if i<=t then {
				Q := transpose!(Q,i,t);
				index(Q.t) := j;
				adjust2!(A,mapping Q,index,t,nn,m);
				t := prev t;
				i := 1;
				while i<=t and index(Q.i)<j-1 repeat {
					i := next i;
				}
				if i<=t then {
					ind := pivotIndex(A,Q.i,j-2);
					di:I := {
						zero? ind => -1;
						machine degree(A(Q.i,ind));
					}
					adjust1!(A,mapping Q,index,i,di,t,nn,m);
				}
			}
		}

		A,Q,index;
	}

	-- Weak Popov form up to column m, elimination performed
	-- up to column h.
	-- Rows 1..(i-1) in weak Popov form.
	-- Adds row i to weak Popov form.
	-- New weak Popov form in rows 1..i.
	-- Matrix and indices represented by A, index and Q, i.e.
	--    - row k of matrix is stored as row Q[k] of A, k=1..i;
	--    - index of row k is stored as index[Q[k]], k=1..(i-1).
	-- Index of row i will be stored as index[Q[i]].
	-- For t<k<=n its index i_k=index[Q[k]] is the index of the
	-- first nonzero element in row k.
	--  We always keep for 1<=l<=t the degree of entry (l,i_k) at
	-- most d_l+e_k, where d_l is the degree of the pivot in the
	-- l-th row up to m and e_k is the degree of entry (k,i_k).
	-- We do this by reduction using the first nonzero entries
	-- in rows t+1,...,n.
	-- destroys A and the individual entries of A
	local extendedIncreasePopov!(A:M, Q:I->I, index:ARR,
					i:I, t:I, n:I, m:I,  h:I):() == {
		import from P;

		l := i;
		repeat {
			index(Q.l) := pivotIndex(A,Q.l,m);
			if index(Q.l)=0 then return;
			k:I := 1;
			while k<=i and (index(Q.k)~=index(Q.l) or k=l) repeat {
				k := next k;
			}
			k>i => return;
			if degree(A(Q.k,index(Q.k))) >
				degree(A(Q.l,index(Q.l))) then {
					temp := k;
					k := l;
					l := temp;
			}
			elementaryOperation!(A,Q.k,Q.l,index(Q.k),h);
			ind := pivotIndex(A,Q.l,m);
			dl:I := {
				zero? ind => -1;
				machine degree(A(Q.l,ind));
			}
			adjust1!(A,Q,index,l,dl,t,n,h);
		}
	}

	-- Make degrees in entries (l,i_k) at most d_l+e_k for t<k<=n.
	-- i_k is the index of the first nonzero entry in row k (t<k<=n).
	-- d_l is the degree of the pivot in the l-th row up to m.
	-- e_k is the degree of entry (k,i_k) for t<k.
	-- i_k1<i_k2 if t<k1<k2.
	--  Elimination performed up to column h.
	-- destroys A but not the individual entries of A
	local adjust1!(A:M, Q:I->I, index:ARR, l:I, dl:I, t:I, n:I, h:I):() == {
		for i in t+1..n repeat {
			q := A(Q.l,index(Q.i)) quo
					shift(A(Q.i,index(Q.i)),(dl+1)::Z);
			for j in index(Q.i)..h repeat
				A(Q.l,j) := A(Q.l,j)-q*A(Q.i,j);
		}
	}

	-- destroys A but not the individual entries of A
	local adjust2!(A:M, Q:I->I, index:ARR, t:I, n:I, h:I): () == {
		import from P;

		TRACE("popov::adjust2!: A = ", A);
		TRACE("popov::adjust2!: t = ", t);
		TRACE("popov::adjust2!: n = ", n);
		TRACE("popov::adjust2!: h = ", h);
#if TRACE
		stderr << "popov::adjust2!: Q(1..n) = ";
		for i in 1..n repeat stderr << Q.i << " ";
		stderr << endnl;
		stderr << "popov::adjust2!: index(1..n) = ";
		for i in 1..n repeat stderr << index i << " ";
		stderr << endnl;
#endif
		m := index(Q.t);
		TRACE("popov::adjust2!: m = ", m);
		D:ARR := new t;  -- ignore D.0
		for i in 1..t-1 repeat D.i := machine degree(A(Q.i,index(Q.i)));
		d:ARR := new next n;  -- ignore d.0..d.(t-1)
		for i in t..n repeat d.i := machine degree(A(Q.i,index(Q.i)));
		s:I := 0;
		q:PrimitiveArray P := new t;  -- ignore q.0
		for k in 1..t-1 repeat {
			TRACE("popov::adjust2!: k = ", k);
			TRACE("popov::adjust2!: D.k = ", D.k);
			TRACE("popov::adjust2!: A(Q.t,m) = ", A(Q.t, m));
			q.k := A(Q.k,m) quo shift(A(Q.t,m),D.k::Z);
			s := max(s,machine degree(q.k));
		}
		-- ignore residue.0..residue.(m-1)
		residue:PrimitiveArray P := new next h;
		for k in m..h repeat
			residue.k := A(Q.t,k);
		for l in 1..s repeat {
			for k in m..h repeat
				residue.k := shift(residue.k,1@Z);
			for u in t+1..n repeat {
				c := coefficient(residue(index(Q.u)),d.u::Z) /
					leadingCoefficient(A(Q.u,index(Q.u)));
				if c~=0 then
					for v in index(Q.u)..h repeat
						residue.v:=residue.v-c*A(Q.u,v);
			}
			for i in 1..t-1 repeat {
				c := coefficient(q.i,l::Z);
				if c~=0 then 
					for j in m..h repeat
						A(Q.i,j) := A(Q.i,j)-c*
							shift(residue.j,D.i::Z);
			}
		}
	}


	local rowEchelon(A:M):(M,I->I,I,ARR,I) ==
		rowEchelon0!(deepCopy A,numberOfRows A);

	-- copies the entries of A but not A itself
	local rowEchelon!(A:M):(I->I,I,ARR,I) == {
		(mat, rho, r, st, sgn) :=
			rowEchelon0!(deepCopy!(A, A),numberOfRows A);
		(rho, r, st, sgn);
	}

	-- destroys A and the individual entries of A
	local rowEchelon0!(A:M, n:I):(M,I->I,I,ARR,I) == {
		import from Sn n;

		m := numberOfColumns A;
		(R,Q,index) := hermite!(A,n);
		r:I := 0;
		st:ARR := new next n;  -- ignore st.0
		for i in 1..n repeat
			if index(Q.i)~=0 then {
				r := next r;
				st.i := index(Q.i);
			}
			else
				st.i := m+1;
		(R,mapping Q,r,st,sign Q);
	}
}
