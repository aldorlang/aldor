--------------------------------------------------------------------------------
--
-- sm_expocat.as: A basic category for multivariate polynomial exponents looked 
--             as monomials.
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO, and University of Lille I, 2001-2002
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- cat ExponentCategory

#include "algebra"

macro { 
      Z == Integer;
}

+++ `ExponentCategory(V)' provides multivariate monomials
+++ (i.e. products of variables from `V') looked as an addtive
+++ ordered monoid (with cancellation) by associating to
+++ every product of variables its sequence of degrees.
+++ Author: Marc Moreno Maza
+++ Date Created: 09/07/01
+++ Date Last Update: 09/07/01

define ExponentCategory(V: VariableType): Category == 
 GeneralExponentCategory with {
	exponent: V -> %;
	  ++ `exponent(v)' returns the monomial `v^1' as an exponent.
        exponent: (V, Z) -> %;
          ++ `exponent(v,z)' returns the monomial `v^z' as an exponent.
        exponent: Generator Cross(V, Z) -> %; 
          ++ `exponent(g)' retunrs the sum of the exponents 
          ++  `exponent(v,z)' for `(v,z)' in `g'.
        exponent: (List V, List Z) -> %; 
          ++ `exponent(lv,ln)' assumes that `lv' and `ln' have the same
          ++ lenght and retunrs the sum of the `exponent(lv.i, ln.i)'
          ++ for `i' in `1..#lv'.
	terms: % -> Generator Cross(V, Z);
	  ++ `terms(x)' returns the sequence of (variable, degree)
	  ++ pairs of `x'. The degree in each pair is always non-zero.
          ++ The name `terms' is justified since a domain of
          ++ `ExponentCategory(V)' is an additive monoid.
        mainVariable: % -> Partial V;
	  ++ `mainVariable(x)' returns the biggest variable in
          ++ the monomial (or exponent) `x'.
	variables: % -> List V;   
	  ++ `variables(x)' returns the ordered list of variables
	  ++ actually appearing in the monomial (or exponent) `x'.
	degree: (%, V) -> Z;
	  ++ `degree(e,v)' returns the degree of the variable `v' 
	  ++ in the exponent (or monomial) `e'.
	totalDegree: % -> Z; 
	  ++ `totalDegree(x)' returns the total degree of monomial `x'.
	totalDegree: (%, List V) -> Z; 
	  ++ `totalDegree(x, lv)' returns the total degree of the
	  ++ monomial `x' considered as a monomial in the variables 
	  ++ of `lv'.
        add!: (%, V, Z) -> %;
          ++ `add!(x,v,z)' returns the same as `add!(x,exponent(v,z))'.
	gcd: (%, %) -> %;
	  ++ `gcd(x, y)' computes the monomial gcd of `x' and `y'.
	lcm: (%, %) -> %;
	  ++ `lcm(x, y)' computes the monomial lcm of `x' and `y'.
        syzygy: (%, %) -> (%, %);          
          ++ `syzygy(x, y)' returns `(a, b)' such that `a+x' and 
	  ++ `b+y' are `lcm(x, y)'.
        default {
            local extree__VZ(cross: Cross(V,Z)): ExpressionTree == {
                  (v: V, z: Z) := cross;
                  import from V, Z;
                  assert(0 < z);
                  z = 1 => extree(v); 
                  import from List(ExpressionTree);
                  ExpressionTreeExpt [extree(v), extree(z)];
            }
            extree(x: %): ExpressionTree == {
                  import from Z;
                  zero? x => extree(1$Z);
                  lexpr: List(ExpressionTree) := 
                       [extree__VZ(cross) for cross in terms(x)];
                  empty? rest lexpr => first lexpr;
                  ExpressionTreeTimes lexpr;
            }
--            (port:TextWriter) << (x: %): TextWriter == {
--                  star?: Boolean := false;
--                  import from String, V, Z;
--                  for cross in terms(x) repeat {
--                      (v: V, z: Z) := cross;
--                      assert(0 < z);
--                      if star? then port << "*";
--                      star? := true;
--                      port << v;
--                      if z ~= 1 then port << "^" << z;
--                  }
--                  port;
--            }
        }
}

#if ALDOC
\thistype{ExponentCategory}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{\this~V: Category}
\Params{
{\em V} & \astype{VariableType} & The domain of variables \\
}
\Descr{\this~V provides multivariate monomials
(i.e. products of variables from V) looked as an addtive
ordered monoid (with cancellation) by associating to
every product of variables its sequence of degrees.}
\begin{exports}
\category{\astype{GeneralExponentCategory}} \\
\alexp{exponent}: & V $\to$ \%   &  The exponent of a variable \\
\alexp{exponent}: & (V, Z) $\to$ \%   &  The exponent of a power of a variable  \\
\alexp{exponent}: & \astype{Generator} \astype{Cross} (V, Z) $\to$ \%   &  The exponent of a power product \\
\alexp{exponent}: & (\astype{List} V,  \astype{List} Z) $\to$ \%  &  The exponent of a power product \\
\alexp{terms}: & \%  $\to$  \astype{Generator} \astype{Cross} (V, Z) &  Inverse map of \alexp{exponent} \\
\alexp{mainVariable}: & \%  $\to$  \astype{Partial} V  &  The biggest variable, if any \\
\alexp{variables}: & \%  $\to$  \astype{List} V & The list of variables of a monomial  \\ 
\alexp{degree}:  & (\%, V) $\to$  Z &  The degree w.r.t.~a variable   \\
\alexp{totalDegree}: & \%  $\to$ Z  & The sum of the degrees of an exponent \\
\alexp{totalDegree}: &  (\%,  \astype{List} V) $\to$ Z & The sum of the degrees w.r.t.~a list \\
\alexp{gcd}: & (\%, \%)  $\to$ \% & Monomial gcd  \\
\alexp{lcm}: & (\%, \%)  $\to$ \%  & Monomial lcm \\
\alexp{syzygy}: & (\%, \%)  $\to$ (\%, \%)   & Cofactors w.r.t.~lcm \\
\end{exports}
\begin{aswhere}
Z &==& \altype{Integer}\\
\end{aswhere}
#endif

#if ALDORTEST
---------------------- test expocat.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	B == Boolean;
        Z == Integer;
}

main():() == {
   import from String, Symbol, MachineInteger, TextWriter;

   V1 == OrderedVariableTuple(-"x",-"y",-"z");
   x1: V1 := variable(1)$V1;
   y1: V1 := variable(2)$V1;
   z1: V1 := variable(3)$V1;
   lv1: List(V1) == [x1,y1,z1];
   E11 == MachineIntegerDegreeLexicographicalExponent(V1);
   E12 == MachineIntegerDegreeReverseLexicographicalExponent(V1);
   E13 == MachineIntegerLexicographicalExponent(V1);
   import from E11, E12, E13;

   V2 == OrderedSymbol;
   x2: V2 := orderedSymbol("x");
   y2: V2 := orderedSymbol("y");
   z2: V2 := orderedSymbol("z");
   lv2: List(V2) == [z2,y2,x2];

   stdout  << "Testing ExponentCategory ..." << endnl;
   pack11 == ExponentCategoryTestPackage(V1,E11);
   stdout << "Testing MachineIntegerDegreeLexicographicalExponent ... ";
   if exponentTest(lv1)$pack11 then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack12 == ExponentCategoryTestPackage(V1,E12);
   stdout << "Testing MachineIntegerDegreeReverseLexicographicalExponent ... ";
   if exponentTest(lv1)$pack12 then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack13 == ExponentCategoryTestPackage(V1,E13);
   stdout << "Testing MachineIntegerLexicographicalExponent ...";
   if exponentTest(lv1)$pack13 then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }   
}

main();

stdout << endnl;
#endif
