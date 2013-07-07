------------------------------------------------------------------------------
--
-- sm_rmp.as: A basic constructor for Recursive Multivariate Polynomials
--
--------------------------------------------------------------------------------
-- Copyright (c) Marc Moreno Maza
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) INRIA (France), USTL (France), UWO (Ontario) 2002
------------------------------------------------------------------------------

-- dom RecursiveMultivariatePolynomial0

#include "algebra"

macro { 
        NNI == Integer;
        Z == Integer;
        MI == MachineInteger;
}

+++ `RecursiveMultivariatePolynomial0(UP,R,V)' provides a basic domain for 
+++ multivariate polynomials with coefficients in `R' and variables 
+++ in `V'. The representation is recursive by using `UP'.
+++ Author: Marc Moreno Maza
+++ Date Created: 02/1996
+++ Date Last Update: 12/07/2001

RecursiveMultivariatePolynomial0(_
  UP: (R: Join(ArithmeticType, ExpressionType), avar: Symbol == new()
       ) -> UnivariatePolynomialAlgebra(R), _
  R :Ring, _
  V : VariableType):  RecursiveMultivariatePolynomialCategory0(R,V) with {
        univariate: % -> UP %;
          ++ `univariate(p)' returns `p' as a univariate polynomial
          ++ w.r.t. its main variable. This assumes that `p'
          ++ is not a constant.
        multivariate: (UP(%), V) -> %;
          ++ `multivariate(p,v)' returns `p' as a multivariate 
          ++ polynomial whose main variable is `v'. An error
          ++ is raised if one coeffcient of `p' does not have
          ++ degree zero w.r.t. `v'. 

} == add {

      import from Boolean, R, V, Z, MI, NNI;

      RVU ==> Record(var:V,terms:U);
      U ==> UP(%);
      Rep == Union(base:R,poly:RVU);
      import from U, RVU, Rep;

      -----------------------
      --% local constants %--
      -----------------------

      local characteristicZero?:Boolean == R has CharacteristicZero;

      -----------------------------------------
      --% Local functions dealing with Rep %--
      -----------------------------------------

      local univariatePretend(p:%): U == {
        ((rep(p)).poly).terms;
      }
      local multivariatePretend(v:V, up : U) : % == {
        per [poly == [var == v, terms == up]@RVU];
      }
      local multivariateCanonical (v:V, up : U) : % == {
         TRACE("multivariateCanonical, v = ", v);
         TRACE("up = ", up);
         zero? up or zero? degree up => leadingCoefficient(up);
         per [poly == [var == v, terms == up]@RVU];
      }
      local base(p:%): R == (rep(p)).base;
      local var(p:%): V == ((rep(p)).poly).var;
      local mdg(p:%): NNI == degree(((rep(p)).poly).terms);
      local ini(p:%): % == leadingCoefficient(((rep(p)).poly).terms);

      local reset!(p:%,r:R): % == {
        (rep(p)).base := r;
        p;
      }
      local reset!(p:%,v:V,up:U): % == {
        ((rep(p)).poly).var := v;
        ((rep(p)).poly).terms := up;
        p;
      }

      --------------------------------------------
      --% exported functions dealing with Rep %--
      --------------------------------------------

      ground?(p:%): Boolean == rep(p) case base;
      coerce(r:R) : % == per [base == r];
      coerce(n:Z):% == n::R::%;
      coerce(v:V):% == multivariatePretend(v, monom);
      multivariate(u: U, v: V): % == {
         for c in nonZeroCoefficients(u) repeat {
            assert(zero? degree(c, v));
         }
         multivariateCanonical(v, u);
      }
      univariate(p:%): U == {
        assert(not ground? p);
        ((rep(p)).poly).terms;
      }

      ------------------------------------------------------
      --% exported functions declared in PolynomialRing0 %--
      ------------------------------------------------------

      variable? (p:%) : Boolean == {
         ground?(p) => false;
         (univariatePretend(p)) = monom$U;
      }
      variable(p:%): V == {
         assert(variable?(p));
         var(p) ;
      }
      variables(p: %): SortedSet(V) == {
        ground?(p) => empty;
        s: SortedSet(V) := [mvar p];
        for trm in univariatePretend(p) repeat {
            (coef, exp) := trm;
            for v in variables(coef)@SortedSet(V) repeat {
                s := insert!(v, s);
            }
        }
        s;
      }
	variables(p: %): Generator(V) == {
		s: SortedSet(V) := variables(p);
		-- s is sorted ascending, so cons'ing in that order, we reverse
		lv: List(V) := empty;
		for v in s repeat lv := cons(v, lv);
		generator lv;
	}
      mainVariable(p: %): V == mvar(p);
      term(r:R,v:V,n:NNI) : % == {
         zero? r => 0;
         zero? n => coerce(r);
         multivariatePretend(v,term(r::%,n)$U);
      }

      ----------------------------------------------
      --%   exported functions in this domain    %--
      ----------------------------------------------

      mvar(p:%): V == {
         assert(not ground?(p));
         var(p);
      }
      mdeg(p:%) : NNI == {
         assert(not ground?(p));
         degree(univariatePretend(p))$U;
      }
      init(p:%):% == {
         assert(not ground?(p));
         leadingCoefficient(univariatePretend(p))$U;
      }
      head(p:%):% == {
         ground?(p) => p;
         multivariatePretend(var(p), term leadingTerm(univariatePretend(p)));
      }
      tail(p:%):% == {
         ground?(p) => 0;
         up := reductum(univariatePretend(p))$U;
         zero? up => 0;
         multivariateCanonical(var(p),up);
      }
      rank (p:%) :% == {
         ground?(p) => 1;
         multivariatePretend(var(p),term(1,degree(univariatePretend(p))));
      }

      -------------------------------------------
      --%   exported functions as ExpressionType   %--
      -------------------------------------------

      (p1 : %) = (p2 : %): Boolean == {
         (ground?(p1)) and (ground?(p2)) => 
            base(p1) = base(p2);
         (ground?(p1)) or (ground?(p2)) => false;
         var(p1) ~= var(p2) => false;
         univariatePretend(p1) =$U univariatePretend(p2);
      }
      copy(p: %): % == {
         -- WE COPY the struture, not its atomes
         ground?(p) => (base p)::%;
         local f:U -> U := map(copy$%)$U;
         up: U := f(univariatePretend(p));
         multivariatePretend(var(p), up);
      }
      copy!(p:%, q:%): % == {
         ground?(p) => {
           ground?(q) => reset!(p,base(q));
           copy q;
         }
         ground? q => copy q;
         q := copy(q);
         reset!(p,var(q),univariatePretend(q)@U);
      }
      extree(p: %): ExpressionTree == {
		ground? p => extree base p;
                (univariatePretend p)(extree mvar p);
      }

      ---------------------------------------------------
      --% exported functions as FreeAlgebra(R)        %--
      ---------------------------------------------------

       nonZeroCoefficients(p:%): Generator(R)  == generate {
            local toSee : List(%) := [p];
            local ip: %;
            local u: U;
            local v: V;
            while not empty? toSee repeat {
                p := first toSee;
                toSee := rest toSee;
                if (ground?(p))
                   then {
                      yield (base(p));
                   } else {
                      ip := leadingCoefficient(univariatePretend(p))$U;
                      u := reductum(univariatePretend(p))$U;
                      if zero? u
                         then {
                             toSee := cons(ip,toSee);
                         } else {
                             v := var(p);
                             toSee := cons(ip,cons(multivariateCanonical(v,u),toSee));
                      }
                }
            }
      }
      support(p:%): Generator(Cross(R,%)) == generate {
               -- import from Character, String, TextWriter;
               zero? p => yield (0,1);
               while not ground? p repeat {
                  ip: % := ini(p);
                  mp: % := rank(p);
                  tp: % := tail(p);
                  while (not ground? ip) repeat {
                     tp := tail(ip) * mp + tp;
                     mp := rank(ip) * mp;
                     ip := ini(ip);
                  }
                  -- stdout << base(ip) << ", " << mp << newline;
                  yield (base(ip),mp);
                  p := tp;
               }
               not zero? p => yield (base(p),1);
      }
      variableProduct(p: %): Generator(Cross(V, NNI)) == generate {
              assert(not ground? p);
              while (not ground? p) repeat {
                 v := var(p);
                 up := univariatePretend(p);
                 yield (v, degree(up));
                 p := leadingCoefficient(up);
              }
      }
      trailingCoefficient(p:%): R == {
        while not ground? (tp := tail(p)) repeat p := tp;
        base(tp);
      }
      reductum(p: %): % == {
          ground? p => 0;
          ip: % := ini(p);
          mp: % := rank(p);
          tp: % := tail(p);
          while (not ground? ip) repeat {
            tp := tail(ip) * mp + tp;
            mp := rank(ip) * mp;
            ip := ini(ip);
          }
          tp;
      }  
      map(f:R -> R)(p:%):% == {
            ground?(p) => f(base(p)) ::%;
            v: V := var(p);
            local up, mup: U;
            up := univariatePretend(p);
            mup := 0;
            for cross in terms(up) repeat {
                (c: %, d: NNI) := cross;
                c := map(f)(c);
                if c ~= 0 then mup := mup + term(c,d)$U;
            }
            multivariateCanonical(v,mup);
      }
      map!(f:R -> R)(p:%):% == {
            ground?(p) => reset!(p,f(base(p)));
            v: V := var(p);
            local up, mup: U;
            up := univariatePretend(p);
            mup := 0;
            for cross in terms(up) repeat {
                (c: %, d: NNI) := cross;
                c := map!(f)(c);
                if c ~= 0 then mup := add!(mup,term(c,d)$U);
            }
            zero? degree mup => {
              q: % := multivariateCanonical(v,mup);
              ground? q => return q;
              reset!(p,var(q),univariatePretend(q));
            }
            reset!(p,v,mup);
      }

      ------------------------------------------------------
      --% exported functions as Algebra)R) (TO CHECK)    %--
      ------------------------------------------------------

      0: % == 0$R ::%;
      1: % == 1$R ::%;

      zero?(p: %): Boolean == {
         ground?(p) => zero?(base(p));
         false;
      }
      one?(p: %): Boolean == {
         ground?(p) => one?(base(p));
         false;
      }
      if R has CommutativeRing then {
          reciprocal(p:%): Partial(%) == {
              not ground? p => failed;
              pr: Partial(R) := reciprocal(base(p));
              failed? pr => failed;
              [(retract pr)::%];
          }
      }
      - (p : %) : % == {
         (ground?(p)) => (-base(p))::%;
         multivariatePretend(var(p),-univariatePretend(p));
      }
      (p1 : %) + (p2 : %): % == {
         ground?(p1) => {
           ground?(p2) => (base(p1) +$R base(p2)):: %;
           multivariatePretend(var(p2),univariatePretend(p2) +$U term(p1,0@NNI)$U);
         }
         ground?(p2) => {
            multivariatePretend(var(p1),univariatePretend(p1) +$U term(p2,0@NNI)$U);
         }
         v1: V := var(p1);
         v2: V := var(p2);
         v1 < v2 => {
            multivariatePretend(var(p2),univariatePretend(p2) +$U term(p1,0@NNI)$U);
         }
         v2 < v1 => {
            multivariatePretend(var(p1),univariatePretend(p1) +$U term(p2,0@NNI)$U);
         }
         multivariateCanonical(v1, univariatePretend(p1) +$U univariatePretend(p2));
      }
      (p1 : %) - (p2 : %): % == {
         ground?(p1) => {
           ground?(p2) => (base(p1) -$R base(p2)):: %;
           multivariatePretend(var(p2), term(p1,0@NNI)$U -$U univariatePretend(p2));
         }
         ground?(p2) => {
            multivariatePretend(var(p1),univariatePretend(p1) -$U term(p2,0@NNI)$U);
         }
         v1: V := var(p1);
         v2: V := var(p2);
         v1 < v2 => {
            multivariatePretend(var(p2), term(p1,0@NNI)$U -$U univariatePretend(p2));
         }
         v2 < v1 => {
            multivariatePretend(var(p1),univariatePretend(p1) -$U term(p2,0@NNI)$U);
         }
         multivariateCanonical(v1, univariatePretend(p1) -$U univariatePretend(p2));
      } 
      (c: R) * (p: %): % == {
         zero? c => 0;
         ground? p => {
            zero? p => p;
            one? p => c::%;
            (c *$R base(p))::%;
         }
         multivariateCanonical(var(p), c::% *$U univariatePretend(p)); 
      }
      (p1 : %) * (p2 : %): % == {
         TRACE("*, p1 = ", p1);
         TRACE("*, p2 = ", p2);
         (ground?(p1)) => base(p1) * p2;
         v1 : V := var(p1);
         TRACE("v1 = ", v1);
        (ground?(p2)) => multivariateCanonical(v1, univariatePretend(p1) * (p2::U));
         v2 : V := var(p2);
         TRACE("v2 = ", v2);
         v1 < v2 => multivariateCanonical(v2, p1 *$U univariatePretend(p2));
         v2 < v1 => multivariateCanonical(v1, univariatePretend(p1) * (p2::U));
         multivariateCanonical(v1, univariatePretend(p1) *$U univariatePretend(p2));
      }
      times(p:%, r: R, v: V, n:NNI): % == {
         zero? n => p * r::%;
         p * term(r,v,n);
      }         
      times!(p:%, r: R, v: V, n:NNI): % == {
         times!(p,term(r,v,n));
      }
      (p: %) ^ (n: NNI): %  == {
         ground?(p) => (base(p) ^ n):: %;
         multivariateCanonical(var(p), univariatePretend(p) ^$U n); 
      }
      characteristic: NNI == characteristic$R;
      leadingCoefficient(p:%):R == {
         ground? p => base p;
         leadingCoefficient init p;
      }

      if R has CommutativeRing then {
         import from R pretend CommutativeRing;
         import from Partial(%);
         import from Partial(R);
         import from Partial(U);
         unitNormal(p: %): (%, %, %) == {
           zero? p => (p,1,1);
           local u,c,v : R;
           (c,u,v) := unitNormal leadingCoefficient p;
           (v*p, u::%, v::%);
         }
         unit?(p: %): Boolean == ground?(p) and unit?(base p);
         canonicalUnitNormal?: Boolean == canonicalUnitNormal?$R;
      }

      if R has IntegralDomain then {
         local (exquo)(up:U, r:%):U == {
            one? r => up;
            map((f:%):% +-> quotient(f, r)) up;
         }
         quotient (p:%,r:R) : % == {
            assert(~zero? r); 
            ground?(p) => (quotient(base(p),r)@R)::%;
            multivariateCanonical(var(p),map((f:%):% +-> quotient(f,r))(univariatePretend p));
         }
         exactQuotient (p1:%, p2:%) : Partial(%) == {
            assert(~zero? p2);
            (zero? p1) or (one? p2) => [p1];
            ground?(p1)  => {
               ground? p2 => {
                   wr : Partial(R) := exactQuotient(base(p1),base(p2));
                   failed? wr => failed;
                   [retract(wr)::%];
               }
               failed;
            }
            local wu: Partial(U);
            v1: V := var(p1);
            ground? p2 or v1 > (v2 := var p2) => {
               wu := exactQuotient(univariatePretend(p1),p2::U)$U;
               failed? wu => failed;
               [multivariateCanonical(v1,retract(wu)@U)];
            }
            v1 < v2 => failed;
            local up1, up2: U;
            up1 := univariatePretend(p1);
            up2 := univariatePretend(p2);
            wu : Partial(U) := exactQuotient(up1,up2);
            failed? wu => failed;
            [multivariateCanonical(v1,retract(wu)@U)];
         }
         quotient (p1:%, p2:%): % == {
            assert(~zero? p2);
            (zero? p1) or (one? p2) => p1;
            ground?(p1) => {
                assert( ground?(p2) ); 
                quotient(base(p1),base(p2))::%;
            }
            ground?(p2) => quotient(p1,base(p2))@%;
            v1: V := var(p1);
            v2: V := var(p2);
            assert(v1 >= v2);
            v1 > v2 => {
               up: U := (exquo)(univariatePretend(p1),p2);
               multivariateCanonical(v1,up);
            }
            local up1, up2: U;
            up1 := univariatePretend(p1);
            up2 := univariatePretend(p2);
            multivariateCanonical(v1,quotient(up1,up2));
         }
      }
      if R has FiniteCharacteristic then {
         import from R pretend FiniteCharacteristic; 
         pthPower(p:%):% == {
           ground?(p) => (pthPower(base(p))$R)::%;
           import from U pretend FiniteCharacteristic; 
           multivariateCanonical(var(p), pthPower(univariatePretend(p)));
         }
      }
}


#if ALDOC
\thistype{RecursiveMultivariatePolynomial0}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this~(UP,R,V)}
\Params{
{\em UP} &
(T: Join(\altype{ArithmeticType}, \altype{ExpressionType})) $\to$ POL~T &
Univariate functor\\
{\em R} & \altype{Ring} & The coefficient ring\\
{\em V} & \altype{VariableType} & The variables\\
}
\begin{aswhere}
POL &==& \altype{UnivariatePolynomialAlgebra}\\
\end{aswhere}
\Descr{\this~(UP,R,V) provides a basic domain for 
multivariate polynomials with coefficients in {\em R} and variables 
in {\em V}. The representation is recursive by means of {\em UP}.
}
\begin{exports}
\category{\altype{RecursiveMultivariatePolynomialCategory0}(R,V)} \\
\alexp{univariate}: & \% $\to$ UP \%  
         & Convert to univariate w.r.t. the greatest variable  \\
\alexp{multivariate}: & (UP(\%), V) $\to$ \%  
         & Convert to multivariate with given variable  \\
\end{exports}
#endif

