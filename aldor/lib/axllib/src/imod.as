-----------------------------------------------------------------------------
----
---- imod.as: Modular integer arithmetic.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

ModularIntegerNumberSystem(I) ==> Ring with {
	integer:Literal -> %;
	coerce: I -> %;
	lift:   % -> I;
	inv:    % -> %;
	/:      (%, %) -> %;	-- Or error
}


ModularIntegerNumberRep(I: IntegerNumberSystem)(n: I): with {
	0:	%;
	1:	%;
	integer:Literal -> %;
	coerce: I -> %;
	zero?:  % -> Boolean;
	=:	(%, %) -> Boolean;     
	~=: 	(%, %) -> Boolean;     
	+:	% -> %;
	-:	% -> %;
	inv:	% -> %;
	+:	(%, %) -> %;
	-:	(%, %) -> %;
	lift:	% -> I;
	<<:	(TextWriter, %) -> TextWriter;
}
== add {
	Rep  == I;
	import from Rep;

	0: % == per 0;
	1: % == per 1;

	(port: TextWriter) << (x: %): TextWriter == port << rep x;

	coerce (i: I): %          == per(i mod n);
	integer(l: Literal): %    == per(integer l mod n);
	lift   (x: %): I          == rep x;

	zero?(x: %): Boolean      == x = 0;
	(x: %) = (y: %): Boolean  == rep(x) = rep(y);
	(x: %)~= (y: %): Boolean  == rep(x) ~= rep(y);

	+ (x: %): %        == x;
	- (x: %): %        == if x = 0 then 0 else per(n - rep x);
	(x: %) + (y: %): % == per(if (z := rep x-n+rep y) < 0 then z+n else z);
	(x: %) - (y: %): % == per(if (z := rep x  -rep y) < 0 then z+n else z);

	inv(j: %): % == {
		local c0, d0, c1, d1: Rep;
		(c0, d0) := (rep j, n);
		(c1, d1) := (rep 1, 0);
		while not zero? d0 repeat {
			q := c0 quo d0;
			(c0, d0) := (d0, c0 - q*d0);
			(c1, d1) := (d1, c1 - q*d1)
		}
		if c0 ~= 1 then error "inverse does not exist";
		if c1 < 0  then c1 := c1 + n;
		per c1
	}
}


SI ==> SingleInteger;

SingleIntegerMod(n: SI): ModularIntegerNumberSystem(SI) with {
	lift: % -> SI
}
== ModularIntegerNumberRep(SI)(n) add {
	Rep == SI;
	coerce(i: SI): %        == per(i mod n);
	coerce(i: Integer): %   == per(retract(i mod n::Integer)@SI);
	(x: %) ^ (k: Integer): %== {
		if (k<0) then inv(x^(-n))
		else power(1, x, k)$BinaryPowering(%,*,Integer);
	}
	(x: %) ^ (k: SI): %     == {
		if (k<0) then inv(x^(-n))
		else power(1, x, k)$BinaryPowering(%,*,SI);
	}
	(x: %) / (y: %): %      == x * inv y;
	lift(x: %): SI          == x pretend SI;

	(x: %) * (y: %): % == {
		import from Machine;
		(xx, yy) := (rep x, rep y);
		xx = 1 => y;
		yy = 1 => x;
		-- Small case
		HalfWord ==> 32767; --!! Should be based on max$Rep
		(n < HalfWord) or (xx<HalfWord and yy<HalfWord) => (xx*yy)::%;

		-- Large case
		(nh, nl) := double_*(xx pretend Word, yy pretend Word);
		(qh, ql, rm) := doubleDivide(nh, nl, n pretend Word);
		rm pretend %;
	}
#if 0
	(x: %) * (y: %): % == {
		import from Machine;
		(xx, yy) := (rep x, rep y);
		(nh, nl) := double_*(xx pretend Word, yy pretend Word);
		(qh, ql, rm) := doubleDivide(nh, nl, n pretend Word);
		rm pretend %;
	}
#endif
}


IntegerMod(n: Integer): ModularIntegerNumberSystem(Integer) with
== ModularIntegerNumberRep(Integer)(n) add {
	Rep == Integer;
	coerce(i: SI): %        == per(i::Integer mod n);
	coerce(i: Integer): %   == per(i mod n);
	(x: %) * (y: %): %      == per((rep x * rep y) mod n);
	(x: %) ^ (k: Integer): %== {
		if (k<0) then inv(x^(-n));
		else power(1, x, k)$BinaryPowering(%,*,Integer);
	}
	(x: %) ^ (k: SI): %     == {
		if (k<0) then inv(x^(-n));
		else power(1, x, k)$BinaryPowering(%,*,SI);
	}
	(x: %) / (y: %): %      == x * inv y;
}
