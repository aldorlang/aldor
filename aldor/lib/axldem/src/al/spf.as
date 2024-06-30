#include "axllib.as"

macro SI == SingleInteger;

SmallPrimeField(p:SI): FiniteField == add {
	Rep == SI;
	import from Rep;
	0:% == per 0;
	1:% == per 1;
	sample: % == 0;	--!! Should get default from AbelianMonoid
	#:Integer == p::Integer;
	integer(l:Literal):% == {
		i:Integer := (integer l) mod (p::Integer);
		per retract i
	}
	(x:%) = (y:%):Boolean == rep(x) = rep(y);
	(x:%) ~= (y:%):Boolean == rep(x) ~= rep(y);
	zero?(x:%):Boolean == x = 0;
	(x:%) + (y:%):% == per mod_+(rep x, rep y, p);
	(x:%) - (y:%):% == per mod_-(rep x, rep y, p);
	(x:%) * (y:%):% == per mod_*(rep x, rep y, p);
	+ (x:%):% == x;
	- (x:%):% == per ((- rep(x)) mod p);
	(x:%) ^ (i:Integer):% == {
		x = 0 => if i = 0 then 1 else 0;
		x ^ retract(i mod ((p-1)::Integer))@SI;
	}
	(x:%) ^ (si:SI):% == {
		x = 0 => if si = 0 then 1 else 0;
		0 <= si and si < p => per mod_^(rep x, si, p);
		x ^ (si mod (p-1));
	}
	coerce(n: SingleInteger): % == per(n mod p);
	coerce(n: Integer): % == per(retract(n mod (p::Integer))@SI);
	inv(x:%):% == per mod_/(1, rep x, p);
	(x:%) \ (y:%):% == y / x;
	(x:%) / (y:%):% == per mod_/(rep x, rep y, p);
	(x:%) quo (y:%):% == x/y;
	(x:%) rem (y:%):% == 0;
	divide(x:%, y:%): (%, %) == (x/y, 0);
	reduce(s:SI):% == per(s mod p);
	lift(x:%):SI == rep x;
	gcd(x:%, y:%) :% == {
		x=0 and y=0 => 0;
		1
	}
	(port: TextWriter) << (x:%): TextWriter == port << rep x;
}

Zmod2: FiniteField == add {
	Rep == Boolean;
	import from Rep;
	import from SI;
	0:% == per false;
	1:% == per true;
	sample: % == 0;	--!! Should get default from AbelianMonoid
	#:Integer == 2;
	(x:%) + (y:%):% == per(rep(x) ~= rep(y));
	(x:%) * (y:%):% == per(rep(x) and rep(y));
	(x:%) - (y:%):% == x+y;
	+ (x:%):% == x;
	- (x:%):% == x;
	(x:%) \ (y:%):% == y / x;
	(x:%) / (y:%):% == x; -- assuming no division by zero
	(x:%) = (y:%):Boolean == rep(x) = rep(y);
	(x:%) ~= (y:%):Boolean == rep(x) ~= rep(y);
	zero?(x:%):Boolean == x = 0;

	(x:%) ^ (i:Integer):% == x;
	(x:%) ^ (si:SI):% == x;
	inv(x:%):% == x;
	(x:%) quo (y:%):% == x/y;
	(x:%) rem (y:%):% == 0;
	divide(x:%, y:%): (%, %) == (x/y, 0);

	integer(l:Literal):% == {
		import from Integer;
		per odd?(integer(l)@Integer)
	}
	coerce(n: SingleInteger): % == per odd? n;
	coerce(n: Integer): % == per odd? n;

	reduce(s:SI):% == per(odd? s);
	lift(x:%):SI == if rep(x) then 1@SI else 0@SI;
	gcd(x:%, y:%) :% == {
		x=0 and y=0 => 0;
		1
	}
	(port: TextWriter) << (x:%): TextWriter == port << lift x;
}
