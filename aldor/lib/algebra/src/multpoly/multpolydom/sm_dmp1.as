--------------------------------------------------------------------------------
--
-- sm_dmp1.as: A basic domain for distributed polynomials.
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- dom DistributedMultivariatePolynomial1

#include "algebra"

macro { 
        SUP == SparseUnivariatePolynomial;
}

DistributedMultivariatePolynomial1(R: Ring,  V: VariableType, 
   E: ExponentCategory(V)): FiniteAbelianMonoidRing0(R,V,E) with {

} == DistributedMultivariatePolynomial0(R,E) add {
 
	Term == Record(co: R, ex: E);
	Rep  == List Term;

        -------------------------------------------
        --%  The representation is sparse and   %--
        --%   decreasing wrt exponent's order   %--
        -------------------------------------------

        import from R, V, E, Term, Rep;

        local commutativeRing?: Boolean       == R has CommutativeRing;
        local integralDomain?: Boolean        == R has IntegralDomain;
        local characteristicZero?: Boolean    == R has CharacteristicZero;
        local finiteCharacteristic? : Boolean == R has FiniteCharacteristic;
        local ordered?: Boolean               == R has OrderedArithmeticType;

        -----------------------------------------
        --% Local functions dealing with Term %--
        -----------------------------------------

        local printTerm(p: TextWriter, t: Term): () == {
		(co, ex) := explode t;
		zero? ex => p << co;
                import from String;
		if co ~= 1 then p << co << "*";
                p << ex;
        }
        local - (t: Term): Term == {
               [-t.co, t.ex];
        }
        local neg!(t: Term): Term == {
              t.co := -t.co;
              t;
        }
        local equalTerm(t1: Term, t2: Term): Boolean == {
               (t1.ex = t2.ex) and (t1.co = t2.co);
        }
        local extreeTerm(t: Term): ExpressionTree == {
              (co, ex) := explode t;
              zero? ex => extree(co);
              co = 1 => extree(ex);
              le: List ExpressionTree := [extree(co), extree(ex)];
              ExpressionTreeTimes le;
        }
        local constructTerm(cross: Cross(R,E)): Term == {
              (c: R, e: E) := cross;
              [c, e];
        }
        local copyTerm(t: Term): Term == {
             [explode t];
        }
        local termTimesPoly(tco: R, tex: E, rx: Rep): Rep == {
		if integralDomain? then {
			termTimes(tx:Term):Term == [tco*tx.co, tex+tx.ex];
			map(termTimes)(rx);
		}
		else {
 			[[r, tex + tx.ex] for tx in rx | 
				not zero? (r := tco * tx.co)];
		}
	} 

        ----------------------------------------
        --% Local functions dealing with Rep %--
        ----------------------------------------

        local mapMinus(xx: Rep): Rep == {
              map(-)(xx);
        }
        local mapMinus!(xx: Rep): Rep == {
              map(neg!)(xx);
        }
        local mapCopyTerm(xx: Rep): Rep == {
              map(copyTerm)(xx);
        }
	--- constant * pol + term * pol
        local pomopo!(isone?:Boolean, c:R, xx:Rep, tco:R, tex:E, yy:Rep):Rep == {
                assert(not zero? c);
                assert(not zero? tco);
                local res, newend, last: Rep;
                res := empty;
                while (not empty? xx) and (not empty? yy) repeat {
                        tx := first xx;
                        ty := first yy;
                        exy := tex + ty.ex;
                        if tx.ex > exy then {
                                if not isone? then tx.co := c * tx.co;
				if (not(isone?) and not(integralDomain?) 
				    and (zero? tx.co)) then {
					xx := rest xx;
					iterate;
				}
                                newend := xx;
                                xx := rest xx;
                        }
                        else {
                                coy := tco * ty.co;
                                yy := rest yy;
                                if exy > tx.ex then {
					if (not(integralDomain?) and
					    (zero? coy)) then {
						iterate;
					}
                                        newend := [[coy, exy]];
                                }
                                else {
					if not isone? then tx.co:= c*tx.co;
					tx.co := tx.co + coy;
                                        if zero? tx.co then {
                                                xx := rest xx;
                                                iterate;
                                        }
                                        else {
                                                newend := xx;
                                                xx := rest xx;
                                        }
                                }
                        }
                        if empty? res then {
                                res := newend;
                                last := res;
                        }
                        else {
                                assert(not empty? last);
                                setRest!(last, newend);
                                last := newend;
                        }
                }
		if not empty? xx then { -- then end is c * xx
			if isone? then newend := xx;
			else newend := rep times!(c, per xx);
		}
		else {  -- then end is (tco, tex) * yy 
			newend := termTimesPoly(tco, tex, yy);
		}
                empty? res => newend;
                assert(not empty? last);
                setRest!(last, newend);
                res;
        }

        ----------------------------------------------
        --% Exported functions as an AbelianMonoid %--
        ----------------------------------------------

	(i: Integer) * (x: %) : % == { 
		zero? i => 0@%;
		zero?(i*1$R) => 0;
		if (integralDomain?) and (characteristicZero?) then {
			itimes(tx:Term):Term == [i*tx.co, tx.ex];
			per (map(itimes)(rep x));
		}
		else {
			per [[r, tx.ex] for tx in rep x | 
				not zero? (r := i*tx.co)];
		}
	}

        -----------------------------------------------
        --% Exported functions as an ArithmeticType %--
        -----------------------------------------------

	1: % == per [[1@R, 0@E]];

        one?(x: %) : Boolean == {
                xx:= rep x;
                empty? xx => false;
                (empty? rest xx) and (zero? (first(xx).ex)) and (one? (first(xx).co));
        }

        commutative?: Boolean == commutativeRing?;

        (x: %) * (y: %) : % == {
                xx: Rep := rep x;
                empty? xx => x;
                yy: Rep := rep y;
                empty? yy => y;
                ground? (x) => first(xx).co * y;
                res : Rep := empty;
                import from MachineInteger;
                if commutativeRing? and (#xx > #yy) then { 
                        (xx,yy) := (yy,xx); 
                        (x,y) := (y,x);
                }
                xx := reverse xx;
                while (not empty? xx) repeat {
                        tx := first xx;
                        res := pomopo!(true, 1$R, res, tx.co, tx.ex, yy);
                        xx := rest xx;
                }
                per res;
        }

        -----------------------------------------------------
        --% Exported functions as a LinearCombinationType %--
        -----------------------------------------------------

        add!(x: %, c2: R, y: %): % == add!(1,x,c2,0,y);

        ----------------------------------------------------------------
        --% Exported functions as an IndexedFreeLinearArithmeticType %--
        ----------------------------------------------------------------

        add!(x: %, c2: R, e: E, y: %): % ==  add!(1,x,c2,e,y);

        --------------------------------------------------------
        --% Exported functions as a FiniteAbelianMonoidRing0 %--
        --------------------------------------------------------

        add!(c1: R, x: %, c2: R, e: E, y: %): % == {
		zero? c1 => per termTimesPoly(c2, e, rep y);
                zero? x => {
                      z: % := term(c2, e);
                      one? z => copy y;
                      z * y;
                }
		zero? c2 => times!(c1, x);
                per pomopo!(one? c1, c1, rep x, c2, e, rep y);
	}

        -----------------------------------------------
        --% Exported functions as a PolynomialRing0 %--
        -----------------------------------------------

        coerce(v: V): % == term(1,exponent(v));

        variable?(x: %): Boolean == {
           ground? x => false;
           not term? x => false;
           (r: R, e: E) := leadingTerm(x);
           lv: List(V) := variables(e);
           not empty? rest lv => false;
           not one? r => false; 
           v: V  := first lv;
           d: Integer := degree(e,v);
           one? d;
        }
        variable(x: %): V == {
           assert(variable? x);
           v?: Partial(V) := mainVariable(degree(x));
           retract v?
        }
        variables(x: %): Generator(V) == {
		-- OF COURSE we should use a merge! from SortedSet IF WE HAD IT
		sv: SortedSet(V) := empty;
		g: Generator Cross(R, E) := terms(x);
		for cross in terms x repeat {
			(r: R, e: E) := cross;
			for v in variables(e) repeat sv := insert!(v,sv);
		}
		-- sv is sorted ascending, so cons'ing in that order we reverse
		lv: List(V) := empty;
		for v in sv repeat lv := cons(v, lv);
		generator lv;
        }
        mainVariable(x:%): V == {
           assert(not ground? x);
           v?: Partial(V) := mainVariable(degree(x));
           v: V := retract(v?);
           xx: Rep := rest(rep x);
           local u: V;
           while (not empty? xx) repeat {
                 t := first xx;
                 xx := rest xx;
                 v? := mainVariable(t.ex);
                 failed? v? => break;
                 u := retract(v?);
                 if u > v then v := u;
           }
           v;
        }
        degree(x: %, v: V): Integer == { 
           d: Integer := 0;
           xx: Rep := rep x;
           local n: Integer;
           while (not empty? xx) repeat {
               t := first xx; 
               xx := rest xx;
               e := t.ex;
               n := degree(e,v);
               if n > d then d := n;
           }
           d;
        }
        coefficient(x: %, v: V, n: Integer): % == {
		l: % := 0;
		e: E := exponent(v,n)$E;
                g: Generator Cross(R, E) := terms(x);
		for cross in g repeat {
			(r: R, f: E) := cross;
			if (degree(f,v) = n) then {
                                f := cancel(f,e);
                                l := add!(l,r,f);
			}
		}
		l;
        }
	term(r: R, v: V, n: Integer): % == {
		zero? r => 0@%;
		per [[r, exponent(v,n)$E]];
	}
        term (r:R, g: Generator(Cross(V, Integer))): % == {
                zero? r => 0@%;
                e: E := exponent(g)$E;
                term(r, e);
        }
        term (r:R, lv: List V, ln: List Integer): % == {
                zero? r => 0@%;
                e: E := exponent(lv,ln)$E;
                term(r, e);
        }
        variableProduct(x: %): Generator(Cross(V, Integer)) == {
                terms(degree(x))$E;
        }
        times (x: %, r: R, v: V, n: Integer): % == {
               ------------------------
               --% TO IMPROVE LATER %--
               ------------------------
               x * term(r,exponent(v,n));
        }
        times! (x: %, r: R, v: V, n: Integer): % == {
               ------------------------
               --% TO IMPROVE LATER %--
               ------------------------
               x * term(r,exponent(v,n));
        }
        coerce(n: Integer): % == {
              zero? n => 0;
              term(n::R,0$E);
        }
        if R has FiniteCharacteristic then {
               import from R pretend FiniteCharacteristic;
               pthPower(x: %): % == {
               ------------------------
               --% TO IMPROVE LATER %--
               ------------------------
                  ground? x => pthPower(leadingCoefficient(x)) :: %;
                  assert(commutativeRing?);
                  y: % := 0;
                  g: Generator Cross(R, E) := terms(x);
                  p: Integer := characteristic$R;
                  for cross in g repeat {
			(r: R, e: E) := cross;
                        y := add!(y, pthPower(r), times(p,e));
                  }
                  y;
               }
        }
}


#if ALDOC
\thistype{DistributedMultivariatePolynomial1}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this~(R,V,E)}
\Params{
{\em R} & \astype{Ring} & The coefficient ring \\
{\em V} & \astype{VariableType} & The variable type \\
{\em E} & \altype{ExponentCategory} V & The exponent domain \\
}
\Descr{\this~(R,V,E) provides a basic domain for 
multivariate polynomials with coefficients in $R$ and variables 
in $V.$ 
The monomials are coded by means of exponents
 from $E.$
Polynomials are represented in a sparse and distributed way.
This means that each polynomial $x$ is coded
as a list of term $(r,e)$ with $r \in R, r\neq 0$ and $e \in E$
such that $x$ is the sum of these terms.}
\begin{exports}
\category{\astype{FiniteAbelianMonoidRing0}(R,V,E)} \\
\end{exports}
#endif






