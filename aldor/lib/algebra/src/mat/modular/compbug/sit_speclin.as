----------------------------   sit_speclin.as   -----------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it ©INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	B == Boolean;
	I == MachineInteger;
	A == PrimitiveArray;
}

#if ALDOC
\thistype{SpecializationLinearAlgebra}
\History{Manuel Bronstein}{4/10/2000}{created}
\Usage{import from \this(R, M)}
\Params{
{\em R} & \astype{CommutativeRing} & The coefficient domain\\
        & \astype{Specializable} & \\
{\em M} & \astype{MatrixCategory} R & A matrix type\\
}
\Descr{\this(R, M) provides basic linear algebra functionalities
using specializations for matrices over $R$.}
\begin{exports}
\asexp{rankLowerBound}:
& M $\to$ \astype{Partial} \builtin{Cross}
(\astype{Boolean}, \astype{MachineInteger}) &
Probable rank\\
\end{exports}
#endif

SpecializationLinearAlgebra(R:Join(CommutativeRing, Specializable),
				M:MatrixCategory R): with {
	rankLowerBound: M -> Partial Cross(B, I);
#if ALDOC
\aspage{rankLowerBound}
\Usage{\name~a}
\Signature{M}
{\astype{Partial} \builtin{Cross}(\astype{Boolean}, \astype{MachineInteger})}
\Params{ {\em a} & M & A matrix\\ }
\Retval{ Returns \failed if specialization failed,
otherwise $(rank?, r)$ such that $r \le \mbox{rank}(a)$,
and $r$ is exactly the rank of $a$ if $rank?$ is \true. }
\Remarks{$r$ can also happen to be the rank of $a$ when $rank?$ is \false,
but the algorithm was unable to prove it.}
#endif
} == add {
	rankLowerBound(m:M):Partial Cross(B, I) == {
		import from Boolean, I, A I, A A I, LazyHalfWordSizePrimes;
		TRACE("speclin::rankLowerBound:m = ", m);
		(r, c) := dimensions m;
		TRACE("speclin::rankLowerBound:r = ", r);
		TRACE("speclin::rankLowerBound:c = ", c);
		mp:A A I := new r;
		for i in 0..prev r repeat mp.i := new c;
		for i in 1..2 repeat {
			p := randomPrime();
			TRACE("speclin::rankLowerBound:p = ", p);
			~failed?(u := rankLowerBound(m, mp, p)) => return u;
		}
		failed;
	}

	local rankLowerBound(m:M, mp:A A I, p:I):Partial Cross(B, I) == {
		import from I, ModulopGaussElimination;
		TRACE("speclin::rankLowerBound:p = ", p);
		(r, c) := dimensions m;
		maxrank := min(r, c);
		TRACE("speclin::rankLowerBound:maxrank = ", maxrank);
		for i in 1..2 repeat {
			TRACE("speclin::rankLowerBound:i = ", i);
			specialize!(mp, m, p) => {
				rk := rank!(mp, r, c, p);
				return [(rk = maxrank, rk)];
			}
		}
		failed;
	}

	local specialize!(mp:A A I, m:M, p:I):Boolean == {
		TRACE("speclin::specialize!:p = ", p);
		macro F == SmallPrimeField0 p;
		import from I, A I, A A I, F, Partial F, PartialFunction(R, F);
		(r, c) := dimensions m;
		sigma := partialMapping(specialization(F)$R);
		for i in 1..r repeat {
			TRACE("speclin::specialize!:i = ", i);
			rowi := mp(prev i);
			for j in 1..c repeat {
				TRACE("speclin::specialize!:j = ", j);
				failed?(u := sigma m(i, j)) => return false;
				rowi(prev j) := machine retract u;
			}
		}
		true;
	}
}
