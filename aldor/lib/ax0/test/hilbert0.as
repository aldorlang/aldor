-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 
--> testgen l

#include "ax0.as"

-- This file computes hilbert functions for monomial ideals
-- Reference:  "On the Computation of Hilbert-Poincare Series",
--	Bigatti, Caboara, Robbiano, AAECC vol 2 #1 (1991) pp 21-33

macro {
	B	== Boolean;
	SI	== SingleInteger;
	NNI	== NonNegativeInteger;
	I	== Integer;
	L	== List;
	Array	== Vector;
	Monom	== Monomial;
	POLY	== SparseUnivariatePolynomial Integer;
}

import from SI, NNI, I;

Monomial: OrderedSet with {
	monom:		Tuple SI -> %;
	monom:		Generator SI -> %;
	varMonom:	(SI, SI, SI) -> %;

	totalDegree:	% -> SI;
	variables:	L % -> L SI;

	divides?:	(%, %) -> B;
	homogLess:	(%, %) -> B;
	<:		(%, %) -> B;
	*:		(%, %) -> %;
	quo:		(%, %) -> %;
	quo:		(%, SI) -> %;

	generator:	% -> Generator SI;
	#:		% -> SI;
	apply:		(%, SI) -> SI;
	bracket:	Tuple SI -> %;
	bracket:	Generator SI -> %;
}
== Array(SI) add {
	Rep == Array SI;

	monom(t: Tuple SI): % == [t];
	monom(g: Generator SI): % == [g];

	varMonom(i: SI, n: SI, deg: SI): % ==
		[(if j = i then deg else 0) for j in 1..n];

	totalDegree(m: %): SI == reduce(+, rep m, 0)$Rep;

	variables(M: L %): L SI == {
		#M < 1 => nil;
		n := # first M;
		ans: L SI := nil;
		for v in 1..n repeat for m in M repeat
			m.v ~= 0 => {
				ans := cons(v, ans);
			 	break;
			}
		ans;
	}

	divides?(m1: %, m2: %): B == {
		for e1 in m1 for e2 in m2 repeat
			if e1 > e2 then return false;
		true;
	}

	homogLess(m1: %, m2: %): B == {
		d1 := totalDegree m1;
		d2 := totalDegree m2;
		d1 < d2 => true;
		d1 > d2 => false;
		m1 < m2;
	}

	(m1: %) < (m2: %): B == {
		for e1 in m1 for e2 in m2 repeat {
			if e1 < e2 then return true;
			if e1 > e2 then return false;
		}
		false;
	}

	(m1: %) * (m2: %): % ==
		[a1 + a2 for a1 in m1 for a2 in m2];

	-- remove vth variable
	(m: %) quo (v: SI): % ==
		[(if i = v then 0 else m.i) for i in 1..#m];

	(m1: %) quo (m2: %): % ==
		[max(a1 - a2, 0) for a1 in m1 for a2 in m2];
}

HilbertFunctionPackage : with {
	Hilbert:	L Monom -> POLY;
	adjoin:		(Monom, L Monom) -> L Monom;
}
== add {
	R == Record(size: SI, ideals: L L Monom, degs: L SI);
	import from R;

	adjoin(m: Monom, lm: L Monom): L Monom == {
		not lm => cons(m, nil);
		ris1: L Monom := nil;
		ris2: L Monom := nil;
		while lm repeat {
			m1 := first lm;
			lm := rest lm;
			if m <= m1 then {
				if not divides?(m, m1) then
					ris1 := cons(m1, ris1);
				iterate;
			}
			ris2 := cons(m1, ris2);
			if divides?(m1, m) then
				return concat!(reverse!(ris1),
						concat!(reverse! ris2, lm));
		}
		concat!(reverse!(ris1), cons(m, reverse! ris2));
	}

	reduce(lm: L Monom): L Monom == {
		lm := sortHomogRemDup lm;
		not lm => lm;
		ris :L Monom := nil;
		risd:L Monom := list first lm;
		d := totalDegree first lm;
		for m in rest lm repeat {
			if totalDegree m = d then
				risd := cons(m, risd);
			else
				ris := mergeDiv(ris, risd);
			d := totalDegree m;
			risd := [m];
		}
		mergeDiv(ris, risd);
	}

	mergeDiv(small: L Monom, big: L Monom): L Monom == {
		ans := small;
		for m in big repeat
			if not contained?(m, small) then ans := cons(m, ans);
		ans;
	}

	contained?(m: Monom, id: L Monom): B == {
		for mm in id repeat
			divides?(mm, m) => return true;
		false;
	}

	(I: L Monom) quo (m: Monom): L Monom ==
		reduce [mm quo m for mm in I];

	sort(I: L Monom, v: SI): L Monom ==
		sort((a: Monom, b: Monom): B +-> (a.v < b.v), I);

	sortHomogRemDup(l: L Monom): L Monom == {
		l := sort(homogLess, l);
		not l => l;
		ans := list first l;
		for m in rest l repeat
			if m ~= first ans then ans := cons(m, ans);
		reverse! ans;
	}

	decompose(K: L Monom, v: SI): R == {
		K := sort(K, v);
		idlist : L L Monom := nil;
		deglist : L SI := nil;
		size: SI := 0;
		J : L Monom := nil;
		while K repeat {
			d := first(K).v;
			tj : L Monom := nil;
			local m : Monom;
			while K and d = (m := first K).v repeat {
				tj := cons(m quo v, tj);
				K := rest K;
			}
			J := mergeDiv(tj, J);
			idlist := cons(J, idlist);
			deglist := cons(d, deglist);
			size := size + (#J)::I::SI;
		}
		[size, idlist, deglist];
	}

	var(n: SI): POLY ==
		monomial(1$Integer, n::I::NNI);

	Hilbert(K : L Monom): POLY == {
		not K => 1; -- no non-zero generators = 0 ideal
		not rest K => var totalDegree first K;
		lvar := variables K;
		Jbest := decompose(K, first lvar);
		for v in rest lvar while (#K)::I::SI < Jbest.size repeat {
			JJ := decompose(K, v);
			JJ.size < Jbest.size => Jbest := JJ;
		}
		Jold := first Jbest.ideals;
		dold := first Jbest.degs;
		f := var(dold) * Hilbert(Jold);
		for J in rest Jbest.ideals for d in rest Jbest.degs
		repeat {
			f := f + (var(d) - var(dold)) * Hilbert(J);
			dold := d;
		}
		var(0) - var(dold) + f;
	}
}
