----
---- polycat.as
----
---- Polynomial categories

#include "axllib"

+++ `OrderedAbelianMonoidSup' provides ordered arithmetic with supremum.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, order

define OrderedAbelianMonoidSup: Category == OrderedAbelianMonoid with {
	sup: 	(%, %) -> %;		++ Supremum.
	sum: 	% -> SingleInteger;	++ Grade.
	nonNegative?:  % -> Boolean;
}


+++ `ExponentCategory' provides the exponent operations for polynomials.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-93
+++ Keywords: polynomials, algebra

ExponentCategory: Category ==
	Join(OrderedAbelianMonoidSup,OrderedAbelianGroup);


+++ `PolynomialCategory' provides the basic operations for polynomials.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-93
+++ Keywords: polynomials, algebra

PolynomialCategory(R: Ring, E: AbelianMonoid): Category == Ring with {
	*:		(R, %) -> %;
	*:		(%, R) -> %;
	degree:		% -> E;
	monomial:	(R, E) -> %;
	reductum:	% -> %;
	leadingCoefficient: % -> R;
	coerce:		R -> %;
}


+++ `OrderedDirectProductCat' provides arithmetic for vectors with an ordering.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-93
+++ Keywords: arithmetic, order, vector, exponent

OrderedDirectProductCat: Category == ExponentCategory with {
	unitVector: 	SingleInteger -> %;
	vector: 	Tuple SingleInteger -> %;
	apply: 		(%, SingleInteger) -> SingleInteger;
}

