-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--# array0.as:  test circular type constructors.

--> testcomp
--> testerrs
#pile

export
	Type:	Type
	->:	(Tuple Type, Tuple Type) -> Type
	Tuple:	Type -> Type

	Array:		(Integer, Type) -> Type
	Array:		Type -> Type
	Integer:	with( make: Array Type -> % )
#if TestErrorsToo
	Integer:	Type
#endif
