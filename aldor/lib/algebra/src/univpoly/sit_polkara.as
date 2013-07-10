--------------------------- sit_polkara.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	A == PrimitiveArray R;
}

#if ALDOC
\thistype{UnivariatePolynomialKaratsuba}
\History{Manuel Bronstein}{6/12/96}{created}
\Usage{ import from \this~R }
\Params{
{\em R} & \altype{CommutativeRing} & The coefficient ring of the polynomials\\
}
\Descr{\this~R implements Karatsuba multiplication for dense univariate
polynomials with coefficients in R.}
\begin{exports}
\alexp{karatsuba!}: & (A, A, Z, A, Z, Z, (A, A, Z, A, Z) $\to$ ()) $\to$ () &
Karatsuba multiplication\\
\end{exports}
\begin{aswhere}
A &==& \altype{PrimitiveArray} R\\
Z &==& \altype{MachineInteger}\\
\end{aswhere}
#endif

UnivariatePolynomialKaratsuba(R:CommutativeRing): with {
	karatsuba!: (A, A, I, A, I, I, (A, I, A, I, I, A, I, I) -> ()) -> ();
} == add {
	local intdom?:Boolean == R has IntegralDomain;

	local zero?(c:A, offset:I, d:I):Boolean == {
		import from R;
		zero? d and zero?(c.offset);
	}

	-- global buffer for karatsuba: only grows as needed so we do
	-- not have to allocate a new work buffer for each product.
	-- store the size whether buf is an array or primitive array
	macro REC			== Record(siz:I, deg:I, buf:A);
	local karaWork:REC		== [1, 0, new 1];

	local karaWorkBuffer(n:I, cut:I):A == {
		import from REC;
		assert(n >= cut);
		buffer := karaWork.buf;
		n <= karaWork.deg => buffer;	-- existing buffer is sufficient
		karaWork.deg := n;
		-- compute the amount of storage needed
		m:I := 0;
		while n >= cut repeat {
			n := next(n quo 2);
			m := m + 2 * n;
			n := prev n;
		}
		TRACE("karatsuba: size needed for work buffer = ", m);
		sz := karaWork.siz;
		m <= sz => buffer;
		-- buffer := resize!(buffer, sz, m);
		free!(karaWork.buf);
		buffer := new m;
		karaWork.buf := buffer;
		karaWork.siz := m;
		buffer;
	}

	-- compute the low+high words of c in cc, both have degrees < e
	-- d = degree c, returns the degree of the sum  c_low + c_high
	-- ncc and nc are offsets in cc and c respectively
	local addlowhigh!(cc:A, ncc:I, c:A, nc:I, d:I, e:I):I == {
		import from R;
		assert(next(d) <= 2*e);
		for i in 0..prev e repeat cc(i+ncc) := c(i+nc);
		for i in e..d repeat cc(i-e+ncc) := cc(i-e+ncc) + c(i+nc);
		computeDegree(prev e, cc, ncc);
	}

	-- adds q to p, dq = degree q, p must be large enough
	-- np and nq are offsets in p and q respectively
	local add!(p:A, np:I, q:A, nq:I, dq:I):() == {
		import from R;
		for i in 0..dq repeat p(i+np) := p(i+np) + q(i+nq);
	}

	-- substracts q from p, dq = degree q, p must be large enough
	-- np and nq are offsets in p and q respectively
	local sub!(p:A, np:I, q:A, nq:I, dq:I):() == {
		import from R;
		for i in 0..dq repeat p(i+np) := p(i+np) - q(i+nq);
	}

	-- zeroes p up to terms of degree d
	local zero!(p:A, offset:I, d:I):() == {
		import from R;
		for i in 0..d repeat p(i+offset) := 0;
	}

	karatsuba!(pq:A, p:A, dp:I, q:A, dq:I, cut:I,
				times!:(A, I, A, I, I, A, I, I)->()):() ==
		karatsuba!(pq, 0, p, 0, dp, q, 0, dq, cut,
				karaWorkBuffer(max(dp, dq), cut), 0, times!);

	-- pq = pq + p q
	-- cut = degree cutoff (use quadratic multiplication below)
	-- npq, np, nq and nw are offsets in pq, p, q and work respectively
	local karatsuba!(pq:A, npq:I, p:A, np:I, dp:I, q:A, nq:I, dq:I, cut:I,
		work:A, nw:I, times!:(A, I, A, I, I, A, I, I) -> ()):() == {
		-- TRACE("karatsuba!: p = ", p);
		TRACE("karatsuba!: dp = ", dp);
		-- TRACE("karatsuba!: q = ", q);
		TRACE("karatsuba!: dq = ", dq);
		TRACE("karatsuba!: cut = ", cut);
		if dp < dq then (p, np, dp, q, nq, dq):= (q, nq, dq, p, np, dp);
		assert(dp >= dq);
		dp < cut => times!(pq, npq, p, np, dp, q, nq, dq);
		m := next(dp quo 2);
		TRACE("karatsuba!: m = ", m);
		assert(2 * m > dp);
		assert(2 * prev m <= dp);
		dpl := computeDegree(prev m, p, np);	-- dpl = degree(pl)
		TRACE("karatsuba!: degree p_l = ", dpl);
		m > dq => {				-- p q = pl q + x^m ph q
			if ~zero?(p, np, dpl) then	-- pq = pq + pl q
				karatsuba!(pq, npq, p, np, dpl, q, nq, dq, cut,
								work,nw,times!);
			karatsuba!(pq, npq+m, p, np+m, dp-m, q, nq, dq, cut,
								work,nw,times!);
		}
		m2 := 2 * m;
		dpsum := addlowhigh!(work, nw, p, np, dp, m);	  -- pl + ph
		dqsum := addlowhigh!(work, nw + m, q, nq, dq, m); -- ql + qh

		-- compute x^m (pl + ph) (ql + qh) = x^m c
		karatsuba!(pq, npq+m, work, nw, dpsum, work, nw+m, dqsum, cut,
							work, nw+m2, times!);
		dql := computeDegree(prev m, q, nq);	-- dql = degree(ql)
		TRACE("karatsuba!: degree q_l = ", dql);
		dplql := dpl + dql;
		assert(dplql <= dp);
		zero!(work, nw, dplql);
		if ~(zero?(p, np, dpl) or zero?(q, nq, dql)) then { -- pl ql
			karatsuba!(work, nw, p, np, dpl, q, nq, dql, cut,
						work, nw + next dplql, times!);
			if ~intdom? then dplql := computeDegree(dplql,work,nw);
			sub!(pq, npq + m, work, nw, dplql); -- x^m (c - plql)
			add!(pq, npq, work, nw, dplql);	    -- add x^0 pl ql
		}
		dphqh := dp + dq - m2;
		assert(dphqh <= dp);
		zero!(work, nw, dphqh);
		karatsuba!(work, nw, p, np+m, dp-m, q, nq+m, dq-m, cut,
						work, nw + next dphqh, times!);
		if ~intdom? then dphqh := computeDegree(dphqh, work, nw);
		sub!(pq, npq + m, work, nw, dphqh);	-- x^m (c - plql - phqh)
		add!(pq, npq + m2, work, nw, dphqh);	-- + x^{2m} ph qh
	}

	-- n = upper bound on the actual degree
	local computeDegree(n:I, c:A, offset:I):I == {
		assert(n >= 0);
		import from R;
		for i in n..0 by -1 repeat ~zero?(c(i+offset)) => return i;
		0;
	}
}
