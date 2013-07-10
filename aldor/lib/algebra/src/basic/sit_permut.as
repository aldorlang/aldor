----------------------- sit_permut.as -----------------------------
--
-- Permutations of a finite numbe of elements
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == MachineInteger;
	A == PrimitiveArray;
}

#if ALDOC
\thistype{Permutation}
\History{Manuel Bronstein}{9/12/1999}{created}
\History{Manuel Bronstein}{30/01/2003}{bug fix: transpose! does not trash 1}
\Usage{import from \this~n}
\Params{{\em n} & \altype{MachineInteger} & The number of elements\\}
\Descr{\this(n) implements the group of permutations on $n$ elements.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{Group}}\\
\alexp{apply}: & (\%, Z) $\to$ Z & Image of an element\\
\alexp{mapping}: & \% $\to$ (Z $\to$ Z) & Action of a permutation\\
\alexp{sign}: & \% $\to$ Z & Sign\\
\alexp{transpose}: & (Z, Z) $\to$ \% & Transposition\\
\alexp{transpose!}: & (\%, Z, Z) $\to$ \% & Compose with a transposition\\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

Permutation(n:Z):Join(CopyableType, Group) with {
	apply: (%, Z) -> Z;
#if ALDOC
\alpage{apply}
\Usage{ \name($\sigma$, x) \\ $\sigma x$}
\Signature{(\%, \altype{MachineInteger})}{\altype{MachineInteger}}
\Params{
{\em $\sigma$} & \% & A permutation\\
{\em x} & \altype{MachineInteger} & An index\\
}
\Retval{Returns $\sigma x$.}
#endif
	mapping: % -> (Z -> Z);
#if ALDOC
\alpage{mapping}
\Usage{\name~$\sigma$}
\Signature{\%}{(\altype{MachineInteger} $\to$ \altype{MachineInteger})}
\Params{ {\em $\sigma$} & \% & A permutation\\ }
\Retval{Returns the map corresponding to $\sigma$.}
\alseealso{\alexp{apply}}
#endif
	sign: % -> Z;
#if ALDOC
\alpage{sign}
\Usage{\name~$\sigma$}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em $\sigma$} & \% & A permutation\\ }
\Retval{Returns the sign of $\sigma$, \ie $(-1)^\epsilon$
where $\epsilon$ is the number of tranpositions in the factorization
of $\sigma$.}
#endif
	transpose: (Z, Z) -> %;
	transpose!: (%, Z, Z) -> %;
#if ALDOC
\alpage{transpose}
\altarget{\name!}
\Usage{\name(x,y)\\ \name!($\sigma$,x,y)}
\Signatures{
\name: & (\altype{MachineInteger}, \altype{MachineInteger}) $\to$ \%\\
\name!: & (\%, \altype{MachineInteger}, \altype{MachineInteger}) $\to$ \%\\
}
\Params{
{\em $\sigma$} & \% & A permutation\\
{\em x,y} & \altype{MachineInteger} & Indices\\
}
\Retval{\name(x,y) returns the transposition $(x y)$, while
\name!($\sigma$,x,y) returns the composition $(x y) \circ \sigma$.}
\Remarks{When using \name!, the storage used by $\sigma$ is allowed to
be destroyed or reused, so do not use it unless $\sigma$ has been
locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases. Since there is no
guarantee of reuse, you should always use the permutation returned
by \name! rather than $\sigma$ after the call.}
#endif
} == {
	assert(n >= 0);
	add {
	Rep == A Z;	-- rep(s).i == s.i for 0 < i <= n
			-- rep(s).0 = sign(s)
			-- rep(s)(n+1) = 1 if s is the constant 1, 0 otherwise

	
	local nplus1:Z			== next n;
	local nplus2:Z			== next nplus1;
	local setsign!(s:%, e:Z):Z	== { import from Rep; rep(s).0 := e; }
	sign(s:%):Z			== { import from Rep; rep(s).0 }
	mapping(s:%):Z -> Z		== { (i:Z):Z +-> s i }
	one?(s:%):Boolean		== constant1?(s) or s = 1;

	local setConst1!(s:%):%	== {
		import from Rep;
		rep(s).nplus1 := 1;
		s;
	}

	local newperm():% == {
		import from Rep;
		a := new nplus2;
		a(nplus1) := 0;		-- force non-constant 1
		per a;
	}

	local constant1?(s:%):Boolean == {
		import from Rep;
		one?(rep(s)(nplus1));
	}

	(s:%) = (t:%):Boolean == {
		import from Z;
		constant1? s and constant1? t => true;
		for i in 1..n repeat {
			s.i ~= t.i => return false;
		}
		true;
	}

	extree(s:%):ExpressionTree == {
		import from Z, List ExpressionTree;
		ExpressionTreeList [extree(s i) for i in 1..n];
	}

	apply(s:%, i:Z):Z == {
		import from Rep;
		assert(i > 0); assert(i <= n);
		rep(s).i;
	}

	local set!(s:%, i:Z, j:Z):Z == {
		import from Rep;
		assert(i > 0); assert(i <= n);
		assert(j > 0); assert(j <= n);
		rep(s).i := j;
	}

	copy(s:%):% == {
		import from Z;
		constant1? s => s;
		s0 := newperm();
		setsign!(s0, sign s);
		for i in 1..n repeat s0.i := s i;
		s0;
	}

	local ident():% == {
		import from Z;
		s := newperm();
		setsign!(s, 1);
		for i in 1..n repeat s.i := i;
		s;
	}
	1:% == setConst1! ident();

	transpose(i:Z, j:Z):% == {
		assert(i > 0); assert(i <= n);
		assert(j > 0); assert(j <= n);
		i = j => 1;
		s := ident();
		setsign!(s, -1);
		s.i := j;
		s.j := i;
		s;
	}

	-- replaces s by (i j) s
	transpose!(s:%, i:Z, j:Z):% == {
		assert(i > 0); assert(i <= n);
		assert(j > 0); assert(j <= n);
		i = j => s;
		constant1? s => transpose(i, j);
		setsign!(s, -sign s);
		t := s.i;
		s.i := s.j;
		s.j := t;
		s;
	}

	inv(s:%):% == {
		import from Z;
		one? s => s;
		s1 := newperm();
		setsign!(s1, sign s);
		for i in 1..n repeat s1(s i) := i;
		s1;
	}

	(s:%) / (t:%):% == {
		import from Z;
		one? t => s;
		st := newperm();
		setsign!(st, sign(s) * sign(t));
		for i in 1..n repeat st(t i) := s i;
		st;
	}

	(s:%) * (t:%):% == {
		import from Z;
		st := newperm();
		setsign!(st, sign(s) * sign(t));
		for i in 1..n repeat st.i := s t i;
		st;
	}

	(s:%) ^ (m:Integer):% == {
		import from Z;
		zero? m => 1;
		one? m => s;
		m < 0 => inv(s)^(-m);
		sm := copy s;
		sgn:Z := { sign(s) > 0 or even? m => 1; -1 }
		setsign!(sm, sgn);
		for i in 2..m repeat sm := times!(sm, s);
		sm;
	}
	}
}

