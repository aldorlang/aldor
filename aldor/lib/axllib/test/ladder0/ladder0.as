-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--# ladder.as:  test circular type constructors.
#pile

--> testcomp

export
	Type:	Type
	->:	(Tuple Type, Tuple Type) -> Type
	Tuple:	Type -> Type

	Ladder:	(D: with( f: % -> E ), E: with( g: % -> D ) ) -> Type
