---------------------------- sal_partial.as ---------------------------------
--
-- This file defines a partial T, i.e. a union of T and failed.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{Partial}
\History{Manuel Bronstein}{7/10/98}{created}
\Usage{import from \this~T}
\Params{ {\em T} & Type & a type\\}
\Descr{\this(T) implements a union of T and \failed.}
\begin{exports}
\alexp{[]}: & T $\to$ \% & create an element\\
\alexp{failed}: & \% & the element \failed\\
\alexp{failed?}: & \% $\to$ \altype{Boolean} & check for the element \failed\\
\alexp{retract}: & \% $\to$ T & convert to an element of T\\
\end{exports}
\begin{exports}[if $T$ has \altype{PrimitiveType} then]
\category{\altype{PrimitiveType}}\\
\end{exports}
\begin{exports}[if $T$ has \altype{HashType} then]
\category{\altype{HashType}}\\
\end{exports}
\begin{exports}[if $T$ has \altype{InputType} then]
\category{\altype{InputType}}\\
\end{exports}
\begin{exports}[if $T$ has \altype{OutputType} then]
\category{\altype{OutputType}}\\
\end{exports}
\begin{exports}[if $T$ has \altype{SerializableType} then]
\category{\altype{SerializableType}}\\
\end{exports}
#endif

Partial(T:Type): with {
	if T has PrimitiveType then PrimitiveType;
	if T has InputType then InputType;
	if T has OutputType then OutputType;
	if T has SerializableType then SerializableType;
	bracket: T -> %;
#if ALDOC
\alpage{[]}
\Usage{[t]}
\Signature{T}{\%}
\Params{{\em t} & T & an element\\ }
\Retval{Returns the element t converted to an element of \%.}
#endif
	failed: %;
	failed?: % -> Boolean;
#if ALDOC
\alpage{failed}
\altarget{\name?}
\Usage{\name\\ \name?~x}
\Signatures{
\name: & \%\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{{\em x} & \% & a partial element\\ }
\Retval{\name~returns the special element \failed, while \name?(x) returns
\true if x is \failed, \false otherwise.}
#endif
	retract: % -> T;
#if ALDOC
\alpage{retract}
\Usage{\name~x}
\Signature{\%}{T}
\Params{{\em x} & \% & a partial element\\ }
\Retval{Returns the element x converted to an element of T, provided that
x is not \failed.}
	valueOr: (%, T) -> T;
#endif
#if ALDOC
\alpage{valueOr}
\Usage{\name~x}
\Signature{\%}{T}
\Params{{\em x} & \% & a partial element\\ }
\Retval{Returns the element x converted to an element of T, provided that
x is not \failed.}
#endif
} == add {
	Rep == Record(val:T);
	import from Rep;

	failed:%		== per nil;
	failed?(x:%):Boolean	== nil? rep x;
	[x:T]:%			== per [x];
	local leftBracket:Ch	== { import from Z; char 91; }
	local rightBracket:Ch	== { import from Z; char 93; }

	retract(x:%):T 	== {
		import from Boolean;
		assert(~failed? x);
		rep(x).val;
	}

	valueOr(x: %, def: T): T == if failed? x then def else retract x;

	if T has PrimitiveType then {
		(x:%) = (y:%):Boolean == {
			import from T;
			fy? := failed? y;
			failed? x => fy?;
			~fy? and retract x = retract y;
		}
	}

	if T has HashType then {
		hash(x:%):MachineInteger == {
			import from T;
			failed? x => 0;
			hash retract x;
		}
	}

	if T has OutputType then {
		(p:TextWriter) << (x:%):TextWriter == {
			import from Boolean, Character, T;
			f? := failed? x;
			p << leftBracket << f?;
			if ~f? then p := p << space << retract x;
			p << rightBracket;
		}
	}

	if T has SerializableType then {
		(p:BinaryWriter) << (x:%):BinaryWriter == {
			import from Boolean, T;
			f? := failed? x;
			p := p << f?;
			f? => p;
			p << retract x;
		}

		<< (p:BinaryReader):% == {
			local x:T;
			f?:Boolean := << p;
			if ~f? then x := << p;
			f? => failed;
			[x];
		}
	}

	if T has InputType then {
		<< (p:TextReader):% == {
			import from Boolean, Character, T;
			local c:Character;
			local x:T;
			while space?(c := << p) or c = newline repeat {};
			c ~= leftBracket => throw SyntaxException;
			f?:Boolean := << p;
			if ~f? then x := << p;
			while space?(c := << p) or c = newline repeat {};
			c ~= rightBracket => throw SyntaxException;
			f? => failed;
			[x];
		}
	}
}

extend Generator(T:Type): with { partialNext!: % -> Partial T } == add {
	partialNext!(g:%):Partial T == {
		for x in (g pretend Generator T) repeat return [x];
		failed;
	}
}

