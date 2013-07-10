------------------------- sit_heugcd.as ----------------------------
#include "algebra"

#if ALDOC
\thistype{HeuristicGcd}
\History{Manuel Bronstein}{13/8/94}{created}
\Usage{import from \this(Z, P)}
\Params{
{\em Z} & \astype{IntegerCategory} & An integer--like ring\\
{\em P} & \astype{UnivariatePolynomialAlgebra0} Z & Polynomials over Z\\
}
\Descr{\this~provides an implementation of the HEUGCD algorithm for
univariate polynomials over the integers.}
\begin{exports}
\asexp{balancedRemainder}: & (Z, Z) $\to$ Z & Symmetric Remainder\\
\asexp{heuristicGcd}: & (P, P) $\to$ (\astype{Partial} P, P, P) &
the Heuristic gcd algorithm\\
\asexp{radixInterpolate}: & (Z, Z) $\to$ P & Radix interpolation\\
\end{exports}
#endif

HeuristicGcd(Z:IntegerCategory, P:UnivariatePolynomialAlgebra0 Z): with {
		balancedRemainder: (Z, Z) -> Z;
#if ALDOC
\aspage{balancedRemainder}
\Usage{\name(n, m)}
\Signature{(Z, Z)}{Z}
\Params{
{\em n} & Z & An integer\\
{\em m} & Z & An integer\\
}
\Retval{Returns $-m/2 \le r < m/2$ such that $n \equiv r \pmod m$.}
#endif
		heuristicGcd: (P, P) -> (Partial P, P, P);
#if ALDOC
\aspage{heuristicGcd}
\Usage{\name($p_1, p_2$)}
\Signature{(P, P)}{(\astype{Partial} P, P, P)}
\Params{ {\em $p_1, p_2$} & P & Polynomials\\ }
\Retval{Returns $(g, q_1, q_2)$ such that $g = \gcd(p_1, p_2)$,
$p_1 = g q_1$ and $p_2 = g q_2$.}
\Remarks{This heuristic can fail in theory, in which case
(\failed, $p_1, p_2$) is returned, although this has never been reported.}
#endif
		radixInterpolate: (Z, Z) -> P;
#if ALDOC
\aspage{radixInterpolate}
\Usage{\name(n, m)}
\Signature{(Z, Z)}{P}
\Params{
{\em n} & Z & A point\\
{\em m} & R & The value of the desired polynomial at n\\
}
\Retval{Returns the unique polynomial $p$ such that $p(n) = m$ and
$$
\vert\vert p \vert\vert_\infty \le \frac{n - 1}2\,.
$$
}
\Remarks{The above bound is useful when an upper bound $M$
on $\vert\vert p \vert\vert_\infty$ is known a priori,
since it is then sufficient to take $n \ge 2 M + 1$.}
#endif
} == add {
	-- From Zippel, "Effective Polynomial Computation",
	-- sec. 15.1 on HeuristicGCD
	heuristicGcd(p:P, q:P):(Partial P, P, P) == {
		import from Boolean, Integer, Z, Partial P;
		zero? p => ([q], 0, 1); zero? q => ([p], 1, 0);
		TIMESTART;
		d := next min(degree p, degree q);
		(cp, p) := primitive p; (cq, q) := primitive q;
		bound := (d * 2^d)::Z * max(height p, height q);
		for i in 1..100@Integer repeat {
			e := bound + random()@Z;
			h := primitivePart radixInterpolate(e, gcd(p e, q e));
			~failed?(pquo := exactQuotient(p, h)) and
				~failed?(qquo := exactQuotient(q, h)) => {
					(g, cp, cq) := gcdquo(cp, cq);
					TIME("heuristicGcd:success at ");
					return([times!(g, h)],
						times!(cp, retract pquo),
						times!(cq, retract qquo));
			}
		}
		TIME("heuristicGcd:failure at ");
		(failed, p, q);
	}

	balancedRemainder(a:Z, b:Z):Z == {
		assert(b > 0);
		r := a rem b;
		r < b quo 2 => r;
		r - b;
	}

	-- From Zippel, "Effective Polynomial Computation",
	-- sec. 15.1 on HeuristicGCD
	radixInterpolate(point:Z, image:Z):P == {
		assert(point > 0);
		p:P := 0;
		d:Integer := 0;
		while image ~= 0 repeat {
			h := balancedRemainder(image, point);
			p := add!(p, h, d);
			image := (image - h) quo point;
			d := next d;
		}
		p;
	}
}
