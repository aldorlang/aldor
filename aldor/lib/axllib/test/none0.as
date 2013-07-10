-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

export
	Type:		Type
	Tuple:		Type -> Type
	->:		(Tuple Type, Tuple Type) -> Type
	Boolean:        Type
	true:		Boolean
	1:		Type

-- Repeat s/b of type () so this is OK.
f0 () : () == {
	repeat { };
}

-- Empty sequence s/b of type () so this is OK.
f1 () : () == {
}

-- Exit at end of sequence allowed with return type ().
f2 () : () == {
	true => 1;
}

-- If w/o else at end of sequence allowed with return type ().
f3 () : () == {
	if true then 1;
}

f4 () : () == {
	if true then 1;
	();
}

#if TestErrorsToo

-- () does not satisfy Type.
g0 () : Type == {
	repeat { };
}

-- () does not satisfy Type.
g1 () : Type == {
}

-- Type (from '1') and () (from 'true => 1') have nothing in common.
g2 () : Type == {
	true => 1;
}

-- A value is needed but if has no else.
g3 () : Type == {
	if true then 1;
}

g4 () : Type == {
	if true then 1;
	()
}
#endif

bits() : Boolean == true
bits(x: Boolean) : Boolean == true

-- This should only have one interpretation, not two.
bits(true)
