------------------------------ sal_base.as -----------------------------------
--
-- This file defines basic types, whose export are limited to wrapping
-- the built-in FOAM operations. Clients of those types should not have to
-- use the Machine type. Those types should be extended later.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

-- Boolean is not defined yet in the build sequence, so don't import it
#assert DoNotImportBoolean

#include "aldor"

import from Machine;

#if ALDOC
\thistype{PackableType}
\History{Manuel Bronstein}{14/6/2001}
{adapted from DenseStorageCategory (axllib)}
\Usage{\this: Category}
\Descr{\this~is the category of types providing their own primitive packed
arrays over themselves.
You can use \altype{PackedPrimitiveArray}(T) as a packed alternative to
\altype{PrimitiveArray}(T) whenever {\em T} provides such functions.}
\begin{exports}
\alexp{getPackedArray}: & (\altype{Pointer}, \altype{MachineInteger}) $\to$ \% &
access an array element\\
\alexp{newPackedArray}: & \altype{MachineInteger} $\to$ \altype{Pointer} &
create an array\\
\alexp{setPackedArray!}:
& (\altype{Pointer}, \altype{MachineInteger}, \%) $\to$ () &
changes an array element\\
\end{exports}
#endif

define PackableType: Category == with {
	getPackedArray: (Pointer, MachineInteger) -> %;
	setPackedArray!: (Pointer, MachineInteger, %) -> ();
#if ALDOC
\alpage{getPackedArray,setPackedArray!}
\altarget{getPackedArray}
\altarget{setPackedArray!}
\Usage{getPackedArray(a, n)\\ setPackedArray!(a, n, x)}
\Signatures{
getPackedArray: & (\altype{Pointer}, \altype{MachineInteger}) $\to$ \%\\
setPackedArray!: & (\altype{Pointer}, \altype{MachineInteger}, \%) $\to$ ()\\
}
\Params{
{\em a} & \altype{Pointer} & an array\\
{\em n} & \altype{MachineInteger} & an index\\
{\em x} & \% & a value\\
}
\Descr{getPackedArray(a, n) returns the element at position {\em n}
in {\em a}, while setPackedArray!(a, n, x) sets the element at position {\em n}
in {\em a}. Note that both functions are 0-indexed.}
#endif
	newPackedArray: MachineInteger -> Pointer;
#if ALDOC
\alpage{newPackedArray}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\altype{Pointer}}
\Params{{\em n} & \altype{MachineInteger} & a size\\ }
\Retval{Returns an array for {\em n} elements}
#endif
}

#if ALDOC
\thistype{PrimitiveType}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of the most basic types.}
\begin{exports}
\alexp{$=$}: & (\%, \%) $\to$ \altype{Boolean} & equality test\\
\alalias{\this}{$=$}{$\sim=$}:
& (\%, \%) $\to$ \altype{Boolean} & inequality test\\
\end{exports}
#endif

define PrimitiveType: Category == with {
	=: (%, %) -> Boolean;
	~=: (%, %) -> Boolean;
#if ALDOC
\alpage{$=$}
\Usage{a = b\\ a $\sim=$ b}
\Signatures{
$=$: & (\%,\%) $\to$ \altype{Boolean}\\
$\sim=$: & (\%,\%) $\to$ \altype{Boolean}\\
}
\Params{ {\em a, b} & \% & elements of the type\\ }
\Retval{ If $a = b$ returns \true, then $a$ and $b$ are guaranteed to
represent the same element of the type. The behavior if $a = b$ returns
\false~depends on the type, since a full equality test might not be
available. At least, it is guaranteed that $a$ and $b$ do not share the
same memory location in that case. The semantics of $a~\sim= b$ is
the boolean negation of $a = b$.}
#endif
	default { (a:%) ~= (b:%):Boolean == ~(a = b); }
}

+++ Union(T) is the disjoint union type former.
+++ Union values are not mutable.
+++
+++ Author: Aldor.org
+++
+++ Overview: Basic
-- The 'export from Boolean' is needed so that 'u case foo' compiles ok
Union(T: Tuple Type): with { export from Boolean } == add;

+++ The Boolean data type supports logical operations.
+++ Both arguments of the binary operations are always evaluated.
+++ The Boolean type is "magic" for the compiler which expects 
+++ Boolean values for such things as if statements.
Boolean: PrimitiveType with {
	~: % -> %;
	coerce: Bool -> %;
	coerce: % -> Bool;
	false:%;
	true:%;
} == add {
	Rep == Bool;

	coerce(b:%):Bool	== rep b;
	coerce(b:Bool):%	== per b;
	false:%			== false@Bool :: %;
	true:%			== true@Bool :: %;
	~(x:%):%		== (~(x::Bool))::%;
	(a:%) = (b:%):%		== (rep a = rep b)::%;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):%	== (rep a ~= rep b)::%;
}

+++ MachineInteger implements machine full-word integers.
MachineInteger: PrimitiveType with {
	0:		%;
	1:		%;
	-:		(%, %) -> %;
	bytes:		%;
	coerce:		SInt -> %;
	coerce:		% -> SInt;
        integer:	Literal -> %;
	min:		%;
	max:		%;
	odd?:		% -> Boolean;
	zero?:		% -> Boolean;
} == add {
	Rep == SInt;

	0:%			== per 0;
	1:%			== per 1;
	coerce(n:%):SInt	== rep n;
	coerce(n:SInt):%	== per n;
	min:%			== per min;
	max:%			== per max;
	integer(l: Literal):%	== per convert(l pretend Arr);
	odd?(n:%):Boolean	== odd?(rep n)::Boolean;
	zero?(n:%):Boolean	== zero?(rep n)::Boolean;
	(a:%) = (b:%):Boolean	== (rep a = rep b)::Boolean;
	(a:%) - (b:%):%		== per(rep a - rep b);

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):Boolean	== (rep a ~= rep b)::Boolean;

	bytes:% == {
		import from Boolean;
		m:SInt := max;
		a:% := 2147483647;
		assert((rep a <= m)::Boolean);
		m::% = a => 4;
		8;
	}
}

+++ AldorInteger implements software integers.
AldorInteger: PrimitiveType with {
	coerce:		BInt -> %;
	coerce:		% -> BInt;
} == add {
	Rep == BInt;

	(a:%) = (b:%):Boolean	== (rep a = rep b)::Boolean;
	coerce(n:%):BInt	== rep n;
	coerce(n:BInt):%	== per n;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):Boolean	== (rep a ~= rep b)::Boolean;
}

+++ Characters for natural language text.
+++ In the portable byte code files, characters are represented in ASCII.
+++ In a running program, characters are represented according to the
+++ machine's native character set, e.g. ASCII or EBCDIC.
Character: PrimitiveType with {
	<:	(%, %) -> Boolean;
	<=:	(%, %) -> Boolean;
	char:	MachineInteger -> %;
	coerce:	% -> Char;
	coerce: Char -> %;
	digit?:	% -> Boolean;
	eof:	%;
	letter?:% -> Boolean;
	lower:	% -> %;
	newline:%;
	null:	%;
	ord:	% -> MachineInteger;
	space:	%;
	space?:	% -> Boolean;
	tab:	%;
	upper:	% -> %;
} == add {
	macro Z == MachineInteger;
	Rep == Char;

	-- TEMPORARY: REALLY NEEDS C-int
	-- import { EOF: CInteger} from Foreign C;
	import { EOF: MachineInteger} from Foreign C;

	coerce(c:%):Char	== rep c;
	coerce(c:Char):%	== per c;
	ord(c:%):Z		== ord(rep c)::Z;
	char(i:Z):%		== per char(i::SInt);
	newline:%		== per newline;
	space:%			== per space;
	-- TEMPORARY: REALLY NEEDS C-int
	-- eof:%			== char EOF;
	eof:%			== per char(-1@SInt);
	null:%			== char 0;
	tab:%			== char 9;
	digit?(c:%):Boolean	== digit?(rep c)::Boolean;
	letter?(c:%):Boolean	== letter?(rep c)::Boolean;
	space?(c:%):Boolean	== c = space or c = tab;
	lower(c:%):%		== per lower rep c;
	upper(c:%):%		== per upper rep c;
	(a:%) = (b:%):Boolean	== (rep a = rep b)::Boolean;
	(a:%) < (b:%):Boolean	== (rep a < rep b)::Boolean;
	(a:%) <= (b:%):Boolean	== (rep a <= rep b)::Boolean;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	-- TEMPORARY: TERRIBLE 1.0.2 OPTIMIZER BUG: THIS DEF GETS TAKEN WRONGLY
	-- (a:%) ~= (b:%):Boolean	== (rep a ~= rep b)::Boolean;
}

+++ Bytes, mostly for I/O to binary streams
Byte: PrimitiveType with {
	coerce: % -> XByte;
	coerce: XByte -> %;
	coerce: % -> MachineInteger;
	coerce: Character -> %;
	coerce: % -> Character;
	eof:	%;
	lowByte: MachineInteger -> %;
} == add {
	macro Z == MachineInteger;
	Rep == XByte;

	coerce(b:%):XByte	== rep b;
	coerce(b:XByte):%	== per b;
	coerce(c:Character):%	== lowByte ord c;
	coerce(n:%):Character	== char(n::Z);
	lowByte(n:Z):%		== per convert(n::SInt);
	(a:%) = (b:%):Boolean	== { import from Z; (a::Z) = (b::Z); }
	eof:%			== (eof$Character) :: %;
	coerce(n:%):Z		== convert(rep n)@SInt :: Z;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):Boolean	== { import from Z; (a::Z) ~= (b::Z); }
}

+++ Pointer is the type of pointers to opaque objects.
Pointer:  PrimitiveType with {
	coerce:	 % -> MachineInteger;
	coerce:	 MachineInteger -> %;
        nil:     %;
        nil?:    % -> Boolean;
} == add {
	macro Z == MachineInteger;
        Rep == Ptr;
	coerce(p:%):Z		== convert(rep p)@SInt ::MachineInteger;
	coerce(n:Z):%		== per convert(n::SInt);
        nil:%			== per nil;
        nil?(p:%):Boolean	== nil?(rep p)::Boolean;
	(a:%) = (b:%):Boolean	== (rep a = rep b)::Boolean;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):Boolean	== (rep a ~= rep b)::Boolean;
}

+++ `Tuple(T)' provides functions for values of type `Tuple T'.
extend Tuple(T: Type): with {
	element: (%, MachineInteger) -> T;
	dispose!: % -> ();
	length: % -> MachineInteger;
	tuple: (MachineInteger, Pointer) -> %;
} == add {
	macro Z == MachineInteger;
	Rep == Record(size: SInt, values: Arr);

	import from Rep, Machine, Z;

	tuple(n:Z, v:Pointer):%	== per [n::SInt, v pretend Arr];
	length(t:%):Z		== rep(t).size::Z;
	element(t:%, i:Z):T	== get(T)(rep(t).values, prev(i::SInt));

	dispose!(t:%):() == {
		dispose! rep(t).values;
		dispose! rep(t);
	}
}

+++ Record(T) is the n-ary record type former.
extend Record(T: Tuple Type): with {
	nil: %;
	nil?: % -> Boolean;
} == add {
	nil:%			== nil$Pointer pretend %;
	nil?(r:%):Boolean	== nil?(r pretend Pointer)$Pointer;
}

