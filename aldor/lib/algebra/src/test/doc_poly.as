#include "algebra"
#include "aldorinterp"

import from String, Symbol;
macro V == OrderedVariableTuple(-"x",-"y",-"z");
import from MachineInteger, V;
macro P == SparseMultivariatePolynomial(Integer,V);
x: P := variable(1)$V :: P;
y: P := variable(2)$V :: P;
z: P := variable(3)$V :: P;
p := (y + z)*x^2 + (y^3 + z)*x + z^4;

macro E1 == MachineIntegerDegreeLexicographicalExponent(V);
macro E2 == MachineIntegerLexicographicalExponent(V);
macro Q1 == DistributedMultivariatePolynomial1(Integer, V, E1);
macro Q2 == DistributedMultivariatePolynomial1(Integer, V, E2);

import from Q1, Q2;
import from Assert Integer;
import from Assert Q1, Assert Q2;
--import from Integer;

q1 := expand(E1)(Q1)(p);
(lcq1, leq1) := leadingTerm q1;
assertEquals(1$Integer, lcq1);
assertEquals(expand(E1)(Q1)(x*y^3), monomial(leq1));

(tcq1, teq1) := trailingTerm q1;
assertEquals(1$Integer, tcq1);
assertEquals(expand(E1)(Q1)(x*z), monomial(teq1));

q2 := expand(E2)(Q2)(p);
(lcq2, leq2) := leadingTerm q2;
assertEquals(1$Integer, lcq2);
assertEquals(expand(E2)(Q2)(x^2*y), monomial(leq2));

(tcq2, teq2) := trailingTerm q2;
assertEquals(1$Integer, tcq2);
assertEquals(expand(E2)(Q2)(z^4), monomial(teq2));

