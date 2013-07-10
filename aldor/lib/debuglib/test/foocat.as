-----------------------------------------------------------------------------
----
---- axlcat.as: Basic categories
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#assert AssertConfig
#assert LibraryBasic

#include "axllib"


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

	default (x: %) ~= (y: %): Boolean == not (x = y);
	default	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	default (<<)(x: %)(p: TextWriter): TextWriter == p << x;
	default (x: %) case (y: %): Boolean == x = y;
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
	default (x: %) \/ (y: %): % == ~(~x /\ ~y);
	default xor(x: %, y: %): % == (x /\ ~y) \/ (~x /\ y);

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

	default empty? (a: %) : Boolean == {
		import from SingleInteger, Machine;
		((#a)::SInt = 0)::Boolean;
	}
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

	default {
		empty(): % ==
			[];
		map (f: S -> S, a:%): % ==
			[f x for x in a];
		map (f: (S,S)->S, a: %, b: %): % ==
			[f(x,y) for x in a for y in b];
	}
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

	default {
		(x: %) <  (y: %): Boolean == y > x;
		(x: %) >= (y: %): Boolean == not (x < y);
		(x: %) <= (y: %): Boolean == not (x > y);
		max(x: %, y: %): % == if x > y then x else y;
		min(x: %, y: %): % == if x < y then x else y;
	}

	default {
		-- These allow sequences, e.g.:   0 <= i < n;
		B% ==> Cross(Boolean, %);

		(x: %)   >  (y: %): B%      == (x >  y, y);
		(x: %)   <  (y: %): B%      == (x <  y, y);
		(x: %)   >= (y: %): B%      == (x >= y, y);
		(x: %)   <= (y: %): B%      == (x <= y, y);

		(bx: B%) >  (y: %): B%      == {(b,x):=bx; (b and x >  y, y)}
		(bx: B%) <  (y: %): B%      == {(b,x):=bx; (b and x <  y, y)}
		(bx: B%) >= (y: %): B%      == {(b,x):=bx; (b and x >= y, y)}
		(bx: B%) <= (y: %): B%      == {(b,x):=bx; (b and x <= y, y)}

		(bx: B%) >  (y: %): Boolean == {(b,x):=bx; b and x >  y}
		(bx: B%) <  (y: %): Boolean == {(b,x):=bx; b and x <  y}
		(bx: B%) >= (y: %): Boolean == {(b,x):=bx; b and x >= y}
		(bx: B%) <= (y: %): Boolean == {(b,x):=bx; b and x <= y}
	}

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

	default (a: %) / (b: %): % == a * inv b;
	default	(a: %) \ (b: %): % == inv a * b;
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

	default	+(x: %): %             == x;
	default	zero? (x: %) : Boolean == x = 0;
	default	sample: %              == 0;
}


+++ `AbelianGroup' provides additive arithmetic with subtraction.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, subtraction

define AbelianGroup: Category == AbelianMonoid with {
	-:	   % -> %;		++ Negation.
        -:         (%, %) -> %;		++ Subtraction.

	default (a: %) - (b: %): % == a + (-b);
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

	default divide(a: %, b: %): (%, %) == (a quo b, a rem b);
}


+++ `Field' provides arithmetic with division.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: arithmetic, field operations, division

define Field: Category == Join(EuclideanDomain, Group) with {
	default {
		gcd(a: %, b: %): % == { zero? a and zero? b => 0; 1 }
		(a: %) quo (b: %): % == a/b;
		(a: %) rem (b: %): % == 0;
		unit?(x: %): Boolean == not zero? x;
	}
}


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

	default {
		abs (x: %): % == if x > 0 then x else -x;
		sign(x: %): % == {x > 0 => 1; x < 0 => -1; 0}
		negative?(x: %): Boolean == x < 0;
		positive?(x: %): Boolean == x > 0;
	}
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


+++ Domains satisfying FortranReal can be passed to Fortran as
+++ values of type REAL.
+++
+++ This is a language defined category.
define FortranReal:Category == with {
	BSFlo ==> SFlo$Machine;
	coerce: % -> BSFlo;
	coerce: BSFlo -> %;
}


+++ Domains satisfying FortranDouble can be passed to Fortran as
+++ values of type DOUBLE PRECISION.
+++
+++ This is a language defined category.
define FortranDouble:Category == with {
	BDFlo ==> DFlo$Machine;
	coerce: % -> BDFlo;
	coerce: BDFlo -> %;
}


+++ Domains satisfying FortranArray can be passed to Fortran as
+++ array values of an appropriate format. Updates made by the
+++ Fortran call may be propagated back to the original value.
+++ The exports defined by this category are applied automatically
+++ by the compiler before and after the Fortran call.
+++
+++ This is a language defined category.
+++ 
+++ Author: AXIOM-XL library
+++ Date Created: 1999
+++ Keywords: fortran, array, automatic
define FortranArray:Category == with {
	-- If any changes are made to this category then fix
	-- gen0FtnArrayValue() and gen0FtnUpdateArray(). The
	-- return type of the second export isn't () otherwise
	-- both exports would generate the same name and type
	-- hash code. Ooops.
	BArr  ==> Arr$Machine;
	BSInt ==> SInt$Machine;

	convert: % -> BArr;
		++ convert(x) is automatically applied by the
		++ compiler before `x' is passed to Fortran.

	convert: (%, BArr) -> BSInt;
		++ convert(x, f) is automatically applied by the
		++ compiler after `x' was passed to Fortran as
		++ the raw machine array `f'. The return value
		++ is ignored.
}


+++ Domains which satisfy FortranMultiArray can be passed to
+++ Fortran as multi-dimensional arrays. The exports defined
+++ by this domain are generally only applied by the exports
+++ for FortranArray, which in turn are automatically applied
+++ by the compiler when array values are passed to Fortran.
+++
+++ This is NOT a language defined category.
+++ 
+++ Author: AXIOM-XL library
+++ Date Created: 1999
+++ Keywords: fortran, array, multi-dimensional
define FortranMultiArray:Category == FortranArray with {
	SI ==> SingleInteger;
	MI ==> SInt$Machine;
	BArr ==> Arr$Machine;

	flatten: (A:Type, A, SI, (A, SI) -> %) -> BArr;
		++ flatten(%, x, n, getter) is used to convert the
		++ `multi-dimensional array `x' with `n'  rows into
		++ a single-dimensional machine array organised in
		++ Fortran format. The `getter' function is used to
		++ extract individual rows from `x'.

	unflatten: (%, BArr, MI, MI) -> ();
		++ unflatten(x, f, off, step) is used to update `x'
		++ after it has been passed to Fortran as the raw
		++ array `f'. The updated array elements start at
		++ offset `off' (counting from 0) in `f' and are
		++ `step' elements apart.
}


+++ Domains satisfying FortranFString can be passed to Fortran as
+++ values of type CHARACTER*N for fixed values of N.
+++
+++ This is a language defined category.
define FortranFString:Category == DenseStorageCategory with
{
	BSInt ==> SInt$Machine;
	#: () -> BSInt;
		++ Length of the fixed-string values.
}


+++ Domains satisfying FortranFStringArray can be passed to Fortran
+++ as values declared as `CHARACTER *(N) arg(M)'. That is, an array
+++ of M elements, each element being a CHARACTER*N
+++
+++ This is a language defined category.
define FortranFStringArray:Category == FortranArray with
{
	BSInt ==> SInt$Machine;
	#: () -> BSInt;
		++ Length of the fixed-string elements.
}


+++ Domains which satisfy DenseStorageCategory promise to provide
+++ exports that can be used to create arrays in which their values
+++ are stored using their "natural" size, and allow their values to
+++ be placed in RawRecords. For example, Byte values would be stored
+++ using 8-bits while DoubleFloat would use 64.
+++ 
+++ Domains which satisfy this category and which define Rep as a
+++ constant (not a macro) do not need to provide implementations
+++ for the exports since the compiler will do this automatically.
+++ However, if the value of Rep is not the same as the packed
+++ representation, then the exports supplied by the compiler must
+++ be over-ridden by defining replacements. See DoubleFloat and
+++ Complex(R) for examples of how to do this.
+++ 
+++ Author: AXIOM-XL library
+++ Date Created: 1999
+++ Keywords: dense, packed, implicit
define DenseStorageCategory:Category == with
{
	BSInt ==> SInt$Machine;
	BArr  ==> Arr$Machine;
	BPtr  ==> Ptr$Machine;

	-- These names are deliberately ugly to reduce the chance
	-- that they will clash with anything. We could insert
	-- strange characters in the names but there is no need to
	-- be excessive. Remember that these operation names are
	-- part of Aldor and will be the same across all libraries.

	PackedArrayNew: BSInt -> BArr;
		++ Create a machine array with enough storage
		++ for the specified number of elements. There
		++ is no support for providing initial values.

	PackedArrayGet: (BArr, BSInt) -> %;
		++ Given a machine array, return the element
		++ at the specified index. Note that zero-based
		++ indexing is used.

	PackedArraySet: (BArr, BSInt, %) -> %;
		++ Given a machine array replace the value at
		++ the specified index with a new value. Note
		++ that zero-based indexing is used.

	PackedRecordSet: (BPtr, %) -> %;
		++ `PackedRecordSet(p, v)' writes the raw representation
		++ of `v' into the machine address `p'. Use `pretend' to
		++ view `p' as an Aldor record or array as appropriate.

	PackedRecordGet: BPtr -> %;
		++ `PackedRecordGet(p)' reads the raw representation of
		++ a value from the machine address `p'. Use `pretend'
		++ to view `p' as an Aldor record or array as appropriate.

	PackedRepSize: () -> BSInt;
		++ This function returns the amount of memory required
		++ to store a raw value in a raw record.


	-- Here are the default implementations that are provided
	-- automatically by the compiler. They are reproduced below
	-- to help with domains that need to over-ride these. Use
	-- the Machine types such as BSInt, BDFlo to get types with
	-- natural-sized values.
	--
	--	import
	--	{
	--		ArrNew: (Rep,  BSInt) -> BArr;
	--		ArrElt: (BArr, BSInt) -> Rep;
	--		ArrSet: (BArr, BSInt, Rep) -> Rep;
	--	} from Builtin;
	--
	--	PackedArrayNew(n:BSInt):BArr ==
	--		ArrNew((0@BSInt) pretend Rep, n);
	--
	--	PackedArrayGet(a:BArr, i:BSInt):% ==
	--		per ArrElt(a, i);
	--
	--	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
	--		per ArrSet(a, i, rep e);
	--
	--	PackedRecordGet(p:BPtr):% ==
	--		per ArrElt(p pretend BArr, 0);
	--
	--	PackedRecordSet(p:BPtr, v:%):% ==
	--		per ArrSet(p pretend BArr, 0, rep v);
	--
	-- For domains whose raw-representation is `SFlo' the following
	-- default implementation would be generated:
	--
	--	PackedRepSize():BSInt == {
	--		import {
	--			TypeSFlo: () -> BSInt;
	--			RawRepSize: BSInt -> BSInt;
	--		} from Builtin;
	--		RawRepSize(TypeSFlo());
	--	}
}


+++ `PrimitiveArrayCategory' provides primitive array operations.
+++ 
+++ Author: AXIOM-XL library
+++ Date Created: 1999
+++ Keywords: array
define PrimitiveArrayCategory(S: Type):Category == with {
        new: SingleInteger -> %;
                ++ `new(n)' creates a new empty array with space for
                ++ n elements.

        new: (SingleInteger, S) -> %;
                ++ `new(n,s)' creates a new array with n elements,
                ++ each initialized to s.

	dispose!: % -> ();
		++ `dispose! a' indicates that `a' will no longer be used.

        set!: (%, SingleInteger, S) -> S;
                ++ `v.n := s' sets the `n'-th element of `v' to `s'.

        apply: (%, SingleInteger) -> S;
                ++ `v.n' extracts the `n'-th element of `v'.

        resize!: (%, osz: SingleInteger, nsz: SingleInteger) -> %;
                ++ `resize!(v, oldsize, newsize)' returns an array with
                ++ the number of elements equal to newsize and containing
                ++ the first m elements of `v', where `m' is the minimum of
                ++ newsize and oldsize.  The value `v' must no longer be used.

	export from S;
}


