
------------------------------------------------------------------------------
--
-- spfresbiv.as: A package providing computations of the resultant
--                of two bivariate polynomials over a small prime field.
--
------------------------------------------------------------------------------
-- Copyright (c) Yuzhen Xie and Marc Moreno Maza
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) USTL & INRIA (France), UWO (Canada) 2003
------------------------------------------------------------------------------

#include "algebra"
#include "aldorio"

macro {
	Z == Integer;
	S == String;
	M == MachineInteger;
	SPF == SmallPrimeField;
	DUP == DenseUnivariatePolynomial;
	SUP == SparseUnivariatePolynomial;
	UP == DenseUnivariatePolynomial;
	ARR == Array; 
        DUPM == DUP(M);
      }

-- We compute resultant of biv. poly. over a Z/pZ
	-- the third argument is the degree bound
	-- After evaluation, we use the generic algorithm 
	-- for resultants in Z/pZ[v]
ResultantOfBivariatePolynomialsOverSmallPrimeField(
	Kp: SmallPrimeFieldCategory,
	Up: UnivariatePolynomialAlgebra(Kp),
	Vp: UnivariatePolynomialAlgebra(Up)): with {

	evaluationResultant: (Vp, Vp, Z) -> Up;
	interpolationResultant: (Vp, Vp, Z) -> Up;
	evaluationReduction: (Vp, Kp) -> Up;	
	interpolation: (ARR Kp, ARR Kp) -> Up;
	lagrangeLi: (M, ARR Kp) -> Up;
	integerImage: Up -> DUPM;

} == add {

	import from TextWriter, Character, String;

	local debugging?: Boolean == false;
	local printing?: Boolean == false;
	local char__Kp: Z == characteristic$Kp;
	local char__Kp__M: M == machine(characteristic$Kp);
	assert(odd?(char__Kp));
	assert(char__Kp > 1);
	local u: Up == monom$Up;

	evaluationResultant(f__Vp: Vp, g__Vp: Vp, D: Z): Up == {
		----------------
		--% Check-in %--
		----------------
		lc__f__Vp: Up := leadingCoefficient(f__Vp);
		lc__g__Vp: Up := leadingCoefficient(g__Vp);
		minimalNumberOfPoints: Z := D + 1 + degree(lc__f__Vp) + degree(lc__g__Vp);
		if (char__Kp < minimalNumberOfPoints) then {
			error "Prime bad w.r.t degree bound.";
		}
		----------------------
		--%  Declarations  %--
		----------------------
		local tmpModulo__Up: Up;-- the current prime u - a (a is the point where we evaluate)
		local tmpRes__Kp: Kp;   -- the lastly computed resultant
		local tmpRes__Up: Up;   -- the lastly computed resultant viewed as a polynomial
		local res__Up: Up;      -- the final result
		local acModulo__Up: Up; -- the current product of the primes
		crp__Up == ChineseRemaindering(Up);
		import from crp__Up;
		-----------------
		--% Computing %--
		-----------------
		count__Z: Z := 1;
		local au__Kp: Kp := 0;  -- the current point where we evaluate
		while (count__Z <= D + 1) repeat {	
			while (zero? apply(lc__f__Vp, au__Kp)
				or zero? apply(lc__g__Vp, au__Kp)) repeat {
					au__Kp := au__Kp + 1;
			}

			tmpModulo__Up: Up := u - au__Kp :: Up; -- the current prime
			tmpEvalf__Up: Up := evaluationReduction(f__Vp, au__Kp);
			tmpEvalg__Up: Up := evaluationReduction(g__Vp, au__Kp);
			tmpRes__Kp := resultant(tmpEvalf__Up, tmpEvalg__Up); 
			
			if (count__Z = 1) then {
				res__Up := tmpRes__Kp :: Up;
				acModulo__Up := tmpModulo__Up;
			} else {
				tmpRes__Up := tmpRes__Kp :: Up;
				previous__res__Up := res__Up;
				previous__acModulo__Up := acModulo__Up;
				res__Up := combine(acModulo__Up, tmpModulo__Up)(res__Up, tmpRes__Up)$crp__Up; 
				acModulo__Up := tmpModulo__Up * acModulo__Up;
			}
			count__Z := count__Z + 1;
			au__Kp := au__Kp + 1;
		}
		res__Up;
	}
	interpolationResultant(f__Vp: Vp, g__Vp: Vp, D: Z): Up == {  
		----------------
		--% Check-in %--
		----------------
		lc__f__Vp: Up := leadingCoefficient(f__Vp);
		lc__g__Vp: Up := leadingCoefficient(g__Vp);
		minimalNumberOfPoints: Z := D + 1 + degree(lc__f__Vp) + degree(lc__g__Vp);
		if (char__Kp < minimalNumberOfPoints) then {
			error "Prime bad w.r.t degree bound.";
		}

		au__arrKp: ARR Kp := new(machine(D+1)); -- value of u, can we give array size as D+1? D is a variable! 
		vres__arrKp: ARR Kp := new(machine(D+1));  -- value of resultant 

		if (char__Kp < D + 1) then error "Prime bad w.r.t degree bound.";
		count__Z: Z := 1;
		local au__Kp: Kp := 0;  -- the current point where we evaluate
		local tmpRes__Kp: Kp;  -- the temporary resultant
		
		while (count__Z <= D + 1) repeat {
			while (zero? apply(lc__f__Vp, au__Kp)
				or zero? apply(lc__g__Vp, au__Kp)) repeat {
					au__Kp := au__Kp + 1;
			}
			au__arrKp(machine(count__Z - 1)) := au__Kp;
			tmpEvalf__Up: Up := evaluationReduction(f__Vp, au__Kp);
			tmpEvalg__Up: Up := evaluationReduction(g__Vp, au__Kp);
			tmpRes__Kp := resultant(tmpEvalf__Up, tmpEvalg__Up); 
			vres__arrKp(machine(count__Z - 1)) := tmpRes__Kp;
		 	count__Z := count__Z + 1;
			au__Kp := au__Kp + 1;
		}
		interpolation(au__arrKp, vres__arrKp);			
	}
	interpolation(au__arrKp: ARR Kp, vres__arrKp: ARR Kp): Up == {
		size__M: M := #au__arrKp;
		-- local index__M: M;
		res__Up: Up := 0;		
		for index__M in 0 .. (size__M - 1) repeat {
			res__Up := res__Up + (vres__arrKp(index__M) :: Up) * lagrangeLi(index__M, au__arrKp);
		}
		res__Up;
	}
	lagrangeLi(i__M: M, au__arrKp: ARR Kp): Up == {
		size__M: M := #au__arrKp;
		-- local index__M: M;
		local Li__Up: Up := 1;
		local d__Kp: Kp;
		for index__M in 0 .. (size__M - 1) repeat {
			if ( index__M ~= i__M ) then {
				d__Kp := inv (au__arrKp(i__M) - au__arrKp(index__M) );
				Li__Up := d__Kp *  Li__Up * (u - au__arrKp(index__M) :: Up);
			}
		}
		Li__Up;
	}
	-- reduce a UP(UP(SPF(p))) to a UP(SPF(p)) by a given value of UP
	evaluationReduction(f__Vp: Vp, au__Kp: Kp): Up == {
		tmp__Kp: Kp := 0;
		red__Up: Up := 0;
		for term__Up in generator(f__Vp) repeat {
			(c__Up: Up, d__V: Z) := term__Up;
			tmp__Kp := apply(c__Up, au__Kp);
			red__Up := add!(red__Up, tmp__Kp, d__V);
		}
		red__Up;
	}
	integerImage(f__Up: Up): DUPM == {
		f__A: DUPM  := 0;
		local c__Z: M;
		local d: Z;
		maxBound: M := (char__Kp__M - 1) quo 2;
		minBound: M := - maxBound;
		for term__Up in generator(f__Up) repeat {
			(c__Kp: Kp, d) := term__Up;
			c__Z:= machine(c__Kp);
			c__Z := c__Z rem char__Kp__M;
			if (c__Z > maxBound) then  c__Z := c__Z - char__Kp__M;
			while (c__Z < minBound) repeat c__Z := c__Z + char__Kp__M;
			f__A := add!(f__A, c__Z, d);
		}
		f__A;
	}
}

#if ALDOC
\thistype{ResultantOfBivariatePolynomialsOverSmallPrimeField}
\History{Yuzhen Xie and Marc Moreno Maza}{Mars 2003}{created}
\Usage{import from \this~(Kp, Up, Vp)}
\Params{
{\em Kp} & \altype{SmallPrimeFieldCategory} \\
{\em Up} & \altype{UnivariatePolynomialAlgebra}  {Kp} \\
{\em Vp} & \altype{UnivariatePolynomialAlgebra} {Up} \\
}
\Descr{\this~(Kp, Up, Vp) provides resultant computations 
       for two bivariate polynomials over a small prime field.
}
\begin{exports}
\alexp{evaluationResultant}: & (Vp, Vp, Z)  $\to$ Up & resultant computed by an evaluation \\
                             &                       & and interpolation scheme. \\
                             &                                      & Arg 3 is a degree bound. \\
\alexp{evaluationReduction}: & (Vp, Kp)  $\to$ Up                   & evaluation at a point \\
\alexp{interpolation}:       & (A Kp, A Kp) $\to$ Up & Lagrange interpolant where the arrays give \\
                             &                                      & points and values resp. \\
\alexp{integerImage}:        & Up  $\to$ DUP
                                          I  & conversion. \\
\end{exports}                
\begin{aswhere}
DUP &==& \altype{DenseUnivariatePolynomial}\\
I &==& \altype{MachineInteger}\\
Z &==& \altype{Integer}\\
A &==& \altype{Array} \\
\end{aswhere}
#endif    
