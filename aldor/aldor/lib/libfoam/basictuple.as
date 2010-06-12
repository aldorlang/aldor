
#include "foamlib"
macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== nil() @ Ptr pretend S;
	Nil?(S)(x)	== x @ S pretend Ptr = nil();
	Reserved	== Pointer;
}


BasicTuple: BasicType with {
	length:   % -> SingleInteger;
	element: (%, SingleInteger) -> Ptr;
--	tuple: Tuple Domain  -> %;
	tuple: Tuple Pointer -> %;
	dispose!:  % -> ();
} == add {
	Rep ==> Tuple Ptr;
	import from Rep, SingleInteger, Ptr;
	
--	tuple(t: Tuple Domain):  % == t pretend BasicTuple;
	tuple(t: Tuple Pointer): % == t pretend BasicTuple;

        import { PtrMagicEQ: (Ptr,Ptr) -> Boolean }  from Builtin ;
	(a: %) = (b: %): Boolean == {
--		(length a) ~= (length b) => false;
--		for i in 1..length a repeat {
--			if not PtrMagicEQ( element(a,i) , element(b,i)) then return false
--		}
		return true;
	}
	(a: %) ~= (b: %): Boolean == not (a = b);
	(p: TextWriter) << (t: %): TextWriter == {
		p << "(tuple: " << length t << " ";
		for i in 1..length t repeat 
			p << "??" << " ";
		p << ")";
	}
	length(x: %): SingleInteger        == length(rep x);
	element(x: %, n: SingleInteger): Ptr == element(rep x, n);
	sample: % == Nil %;
	dispose!(x: %): () == dispose! rep(x);

	hash(x:%):SingleInteger ==
	{
		local result:SingleInteger := length(x);
		local elem:SingleInteger;

		for i in 1..length x repeat
		{
			elem   := element(x,i) pretend SingleInteger;
			result := xor(result, hash elem);
		}

		result;
	}


	-- Originally these were defaults from BasicType etc
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
}
