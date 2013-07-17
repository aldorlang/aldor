-----------------------------------------------------------------------------
----
---- foamcat.as: Basic categories
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#assert AssertConfig
#assert LibraryBasic

#include "foamlib"


+++ `BasicType' is a basic category to which most types belong.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: type

define BasicType: Category == with {
        =:     	(%, %) -> Boolean;		++ Equality test.
        ~=:    	(%, %) -> Boolean;		++ Inequality test.
        <<: 	(TextWriter, %) -> TextWriter;	++ Basic output.
	<<:	% -> TextWriter -> TextWriter;	++ Basic output.
	sample:	%;				++ Example element.
	hash:	% -> SingleInteger;		++ Hashing function.
	case:   (%,%) -> Boolean ; 		++ for 'select' statements;
}

+++ `Logic' provides the basic operations for lattices,
+++ e.g., boolean algebra.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: logic, boolean

define Logic: Category == BasicType with {
        ~:        % -> %;		++ Logical complement.
        /\:       (%, %) -> %;		++ Logical `meet', e.g. `and'.
        \/:       (%, %) -> %;		++ Logical `join', e.g. `or'.
	xor:	  (%, %) -> %;		++ `Exclusive or' 
}


+++ `Conditional' allows values to be used in conditional context,
+++ such as in an `if' or in a `while'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: test, conditional, generic

define Conditional: Category == BasicType with {
        test:     % -> Boolean;		++ Test used in conditional context.
}


+++ `Aggregate(S)' provides operations for manipulating structures
+++  representing collections of values of type `S'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: aggregate, collection, container

define Aggregate(S: Type): Category == BasicType with {
	empty?:   % -> Boolean;		++ Is the element count zero?
	generator:% -> Generator S;	++ Generic traversal of an aggregate.
	map:      (S->S, %) -> %;	++ Form new aggregate using function.
					-- Different shapes => no binary map.
	export from S;
}

+++ `FiniteAggregate(S)' provides operations for manipulating structures
+++  representing finite collections of values of type `S'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: aggregate, collection, container

define FiniteAggregate(S: Type): Category == Aggregate S with {
	#:        % -> SingleInteger;	++ Count of the number of elements.
}

+++ `LinearAggregate(S)' provides operations for manipulating structures
+++  representing linear collections of values of type `S'.
+++!! For this release the constructors may generate errors if not given
+++!! suitable numbers of constituents.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: aggregate, collection, container

define LinearAggregate(S: Type): Category == Aggregate S with {
	empty:    () -> %;		++ Construction with no values.
	bracket:  Generator S -> %;	++ Construction from a generator.
	bracket:  Tuple S -> %;		++ Construction from a tuple.

	map:	  ((S,S)->S, %, %) -> %;++ Element-wise combination.
	apply:    (%, SingleInteger) -> S;
}

+++ `FiniteLinearAggregate(S)' provides operations for manipulating structures
+++  representing finite linear collections of values of type `S'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: aggregate, collection, container

define FiniteLinearAggregate(S: Type): Category ==
	Join(FiniteAggregate S, LinearAggregate S);


+++ `Finite' indicates that a type has only a finite number of values.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: finite, cardinality

define Finite: Category == BasicType with {
        #: Integer;			++ Number of values in the type.
}


+++ `Order' provides operations for an order relation, `>', and its dual, `<'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: inequality, order

define Order: Category == BasicType with {
        >:	(%, %) -> Boolean;	++ Greater than test.
        <:      (%, %) -> Boolean;	++ Less than test.
        >=:     (%, %) -> Boolean;	++ Greater than or equal test.
        <=:     (%, %) -> Boolean;	++ Less than or equal test.
	max:    (%, %) -> %;		++ Maximum argument.
	min:    (%, %) -> %;		++ Minimum argument.
}


+++ `OrderedFinite' provides the minimum and maximum values for a finite type.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: finite, minimum, maximum

define OrderedFinite: Category == Join(Finite, Order) with {
	min: %;				++ Minimum value belonging to the type.
        max: %;				++ Maximum value belonging to the type.
}

+++ `ArithmeticSystem' provides the basic aritmetic operations,
+++ without implying associativity, commutativity, etc.
define ArithmeticSystem: Category == with {
	0:	%;			 ++ Identity for `+'
	+:	% -> %;			 ++ Identity.
	+:	(%, %) -> %;		 ++ Addition.
	-:	% -> %;			 ++ Negation.
	-:	(%, %) -> %;		 ++ Subtraction.

	1:	%;			 ++ Identity for `*'
	*:	(%, %) -> %;		 ++ Multiplication
	^:	(%, Integer) -> %; 	 ++ Exponentiation or error

	coerce: Integer -> %;
	coerce: SingleInteger -> %;
}

+++ `Monoid' provides multiplicative arithmetic.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, multiplication

define Monoid: Category == BasicType with {
	1:	%;			 ++ Identity for multiplication.
        *:      (%, %) -> %;		 ++ Multiplication.
  	^:	(%, Integer) -> %; 	 ++ Exponentiation or error
}


+++ `Group' provides multiplicative arithmetic with division.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, group operations, multiplication, division

define Group: Category == Monoid with {
	/:	(%, %) -> %;         	++ Division: a/b = a*inv(b).
	\:	(%, %) -> %;		++ Division: a\b = inv(a)*b
	inv:	% -> %;			++ Inverse.
}


+++ `AbelianMonoid' provides additive arithmetic.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, addition

define AbelianMonoid: Category == BasicType with {
	0:	   %;			++ Identity for addition.
	+:	   % -> %;		++ Identity function.
        +:         (%, %) -> %;		++ Addition.
        zero?:     % -> Boolean;	++ Test whether value equals 0.
}


+++ `AbelianGroup' provides additive arithmetic with subtraction.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, subtraction

define AbelianGroup: Category == AbelianMonoid with {
	-:	   % -> %;		++ Negation.
        -:         (%, %) -> %;		++ Subtraction.
}


+++ `Ring' provides the basic arithmetic operations of
+++ addition, subtraction and multiplication.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, ring operations

define Ring: Category == Join(ArithmeticSystem, Monoid, AbelianGroup);


+++ `EuclideanDomain' provides ring operations and division with remainder.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, division, greatest common divisor, Euclidean domain

define EuclideanDomain: Category == Ring with {
	gcd:    (%, %) -> %;		++ Greatest commond divisor.
	quo:    (%, %) -> %;		++ Quotient leaving remainder.
	rem:    (%, %) -> %;		++ Remainder.
	divide: (%, %) -> (%,%); 	++ Quotient-remainder pair.
}


+++ `Field' provides arithmetic with division.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, field operations, division

define Field: Category == Join(EuclideanDomain, Group);


+++ `FiniteField' provides arithmetic on a finite set.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, field operations, division

define FiniteField: Category == Join(Finite, Field) with {
	integer:Literal -> %;
	reduce: SingleInteger -> %;
	lift:   % -> SingleInteger;  -- can give error 
}


+++ `OrderedAbelianMonoid' provides an addition consistent with an order.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, order

define OrderedAbelianMonoid: Category == Join(Order, AbelianMonoid);


+++ `OrderedAbelianGroup' provides group operations consistent with an order.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, order

define OrderedAbelianGroup: Category ==
	Join(OrderedAbelianMonoid, AbelianGroup);


+++ `OrderedRing' provides arithmetic consistent with an ordering.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, order

define OrderedRing: Category == Join(OrderedAbelianGroup, Ring) with {
        abs:       % -> %;		++ Absolute value.
	sign:      % -> %;		++ -1, 0, or 1.
        negative?: % -> Boolean;	++ Test whether value is negative?
        positive?: % -> Boolean;	++ Test whether value is positive?
}


+++ `IntegerNumberSystem' provides the basic operations for a type which
+++ represents a range of integers.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: integer, arithmetic

define IntegerNumberSystem: Category == Join(OrderedRing,EuclideanDomain) with
{
        integer:   Literal -> %;	++ Integer-style literals.
        even?:     % -> Boolean;	++ Test whether the value is even.
        odd?:      % -> Boolean;	++ Test whether the value is odd.
        single?:   % -> Boolean;	++ Can a SingleInteger hold the value?
        prev:      % -> %;		++ Decrement the value by 1.
        next:      % -> %;		++ Increment the value by 1.
        mod:       (%, %) -> %;		++ Modulus.
        length:    % -> SingleInteger;	++ Length in bits.
        shift:     (%, SingleInteger) -> %; ++ Shift by the a number of bits.
        bit:       (%, SingleInteger) -> Boolean; ++ Extract the desired bit.
	retract:   % -> SingleInteger;	++ Convert to SingleInteger, if can.
}


+++ `FloatingPointNumberSystem' provides the basic operations for a type which
+++ approximates the real numbers as a floating point system.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: floating point, real, approximate, arithmetic

define FloatingPointNumberSystem: Category == Join(OrderedRing, Field) with {
        float:     Literal -> %;	++ Floating point-style literals.
	<<:        (%, %) -> Boolean;   ++ Insignificance: a << b => a+b=b
	>>:        (%, %) -> Boolean;   ++ Dominance:      a >> b => a+b=a
        coerce:    Integer -> %;	++ Conversion from `Integer'.
        coerce:    SingleInteger -> %;	++ Conversion from `SingleInteger'.
        step:      SingleInteger -> (%, %) -> Generator %;
					++ `step(n)(a,b)' yields `n' evenly
					++ separated values on `[a,b]'

	-- Exponentiation to SingleInteger powers is not provided
	-- to avoid conflicts with Integer powers.
	-- ^:		(%, SingleInteger) -> %;

	integer: 	% -> Integer;
	fraction: 	% -> %;
	round:		(%, mode:RoundingMode == nearest()) -> Integer;

	prev:		% -> %;
	next:		% -> %;

	floor:		% -> %;
	ceiling:	% -> %;
		++ Next (lowest, highest) respv.
	round_+:    (%, %, mode:RoundingMode == nearest()) -> %;
	round_-:    (%, %, mode:RoundingMode == nearest()) -> %;
	round_*:    (%, %, mode:RoundingMode == nearest()) -> %;
	round_*_+:  (%, %, %, mode:RoundingMode == nearest()) -> %;
	round_/:    (%, %, mode:RoundingMode == nearest()) -> %;

	mantissa: % -> Integer;
	exponent: % -> Integer;

	export from RoundingMode;
}

+++ `ElementaryFunctions' provides the basic trigonometric and exponential
+++ functions and their inverses.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: elementary functions, transcendental functions

define ElementaryFunctions: Category == with {
	exp:	% -> %;			++ Exponential of `e'.

	sin:	% -> %;			++ Circular trigonometric sine.
	cos:	% -> %;			++ Circular trigonometric cosine.
	tan:	% -> %;			++ Circular trigonometric tangent.

	sinh:	% -> %;			++ Hyperbolic trigonometric sine.
	cosh:	% -> %;			++ Hyperbolic trigonometric cosine.
	tanh:	% -> %;			++ Hyperbolic trigonometric tangent.


	log:	% -> %;			++ Natural logarithm.
	log10:	% -> %;			++ Logarithm base 10.

	asin:	% -> %;			++ Functional inverse of `sin'.
	acos:	% -> %;			++ Functional inverse of `cos'.
	atan:	% -> %;			++ Functional inverse of `tan'.

	asinh:	% -> %;			++ Functional inverse of `sinh'.
	acosh:	% -> %;			++ Functional inverse of `cosh'.
	atanh:	% -> %;			++ Functional inverse of `tanh'.

	sqrt:	% -> %;			++ Square root.
	root:	(%, SingleInteger) -> %;++ General surd.
	^:	(%, %) -> %;		++ General exponentiation.
}

