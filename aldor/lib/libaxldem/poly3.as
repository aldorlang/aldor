#include "axllib"

--Lau == Enumeration (coef: BasicType);
--Lau == 'coef';
--print << Lau << newline;

define MyBasicType: Category == with {
        =:     	(%, %) -> Boolean;
        ~=:    	(%, %) -> Boolean;
}

Term(S: MyBasicType, Expon: MyBasicType): MyBasicType with {
	apply: (%, 'coef') -> S;
--	apply: (%, MyBasicType) -> S;
	lau: % -> %;
}
== add {
	Rep == List S;
	import from Rep;
	0: % == 0$Integer pretend %;
	apply(f:%, tag:'coef'):S == rep(f).first;
--	apply(f:%, tag: MyBasicType):S == rep(f);
	(f1:%) = (f2:%): Boolean == true;
	(f1:%) ~= (f2:%): Boolean == true;
	+++ hahaha
	lau(xxx: %): % == {
		xxx ~= 0 => xxx; 
		xxx;
	}
}

#assert 1
#if 1
Polynomial(S: MyBasicType, Expon: MyBasicType): MyBasicType with {
	nate:	% -> %;
}
== add {
	Rep == List Term(S, Expon);
	import from Rep;
	import from Term(S, Expon);

	0: % == 0$Integer pretend %;

	(xx: %) =  (yy: %): Boolean == rep(xx) = rep(yy);
	+++ ddd
	(xx: %) ~= (yy: %): Boolean == rep(xx) ~= rep(yy);

	nate(x: %): % == {
		x ~= 0 => x;
		x;
	}
}
#endif
