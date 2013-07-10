-----------------------------------------------------------------------------
----
---- efuns.as: Elementary functions for DoubleFloats
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

F ==> DoubleFloat;
C ==> Complex F;

inline from F, C;

EF(K) ==> with {
	sqrt:	K -> K;
	^:	(K, K) -> K;
	log:	K -> K;
	log:	(K, K) -> K;
	exp:	K -> K;

	sin:	K -> K;
	cos:	K -> K;
	tan:	K -> K;

	sinh:	K -> K;
	cosh:	K -> K;
	tanh:	K -> K;

	asin:	K -> K;
	acos:	K -> K;
	atan:	K -> K;

	asinh:	K -> K;
	acosh:	K -> K;
	atanh:	K -> K;
}


DoubleFloatElementaryFunctions: Join(EF F, EF C) with {
	atan: (F, F) -> F;
	abs:   C -> F;
	phase: C -> F;
}
== add {
	import from F, C, Integer, String;

	import {
		sqrt:	BDFlo -> BDFlo;
		pow:	(BDFlo, BDFlo) -> BDFlo;
		log:	BDFlo -> BDFlo;
		exp:	BDFlo -> BDFlo;

		sin: 	BDFlo -> BDFlo;
		cos: 	BDFlo -> BDFlo;
		tan: 	BDFlo -> BDFlo;

		sinh:	BDFlo -> BDFlo;
		cosh:	BDFlo -> BDFlo;
		tanh:	BDFlo -> BDFlo;

		asin:	BDFlo -> BDFlo;
		acos:	BDFlo -> BDFlo;
		atan:	BDFlo -> BDFlo;
		atan2:	(BDFlo, BDFlo) -> BDFlo;
	} from Foreign;

	sq x ==> x*x;

	--
	-- Real 
	--
	sqrt(x: F): F	      == sqrt(x::BDFlo)::F;
	(x: F) ^ (y: F): F    == pow(x::BDFlo,y::BDFlo)::F;
	log(x: F): F	      == log(x::BDFlo)::F;
	log(base: F, x: F): F == log x/log base;
	exp(x: F): F	      == exp(x::BDFlo)::F;

	sin(x: F): F          == sin(x::BDFlo)::F;
	cos(x: F): F          == cos(x::BDFlo)::F;
	tan(x: F): F          == tan(x::BDFlo)::F;

	asin(x: F): F         == asin(x::BDFlo)::F;
	acos(x: F): F         == acos(x::BDFlo)::F;
	atan(x: F): F         == atan(x::BDFlo)::F;
	atan(y: F, x: F): F   == atan2(y::BDFlo,x::BDFlo):: F;

	sinh(x: F): F         == sinh(x::BDFlo)::F;
	cosh(x: F): F         == cosh(x::BDFlo)::F;
	tanh(x: F): F         == tanh(x::BDFlo)::F;

	--!! These need to be done accurately.
  	asinh(x: F): F == {
		log(x + sqrt(1 + sq x));
	}
  	acosh(x: F): F == {
		x < 1 => throw RangeError(F, x,  "acosh");
		log(x + sqrt(sq x - 1))
	}
  	atanh(x: F): F == {
		x <= -1 or 1 <= x => throw RangeError(F, x,  "atanh");
		log((1+x)/(1-x))/2.0
	}

	--
	-- Complex
	--
	abs  (z: C): F   == sqrt norm z;
	phase(z: C): F   == atan(imag z, real z);

	sqrt(z: C): C    == exp(0.5*log z);
	(z: C)^(w: C): C == exp(w * log z);

	log(z: C): C          == complex(log abs z, phase z);
	log(base: C, z: C): C == log z/log base;

	exp (z: C): C    == {
		r := exp real z;
		complex(r*cos imag z,r*sin imag z)
	}
	sin(z: C): C == {
		x := real z; y := imag z;
		complex(sin x*cosh y, cos x*sinh y)
	}
	cos(z: C): C == {
		x := real z; y := imag z;
		complex(cos x*cosh y,-sin x*sinh y)
	}
	tan(z: C): C == {
		x := real z; y := imag z;
		sinx  := sin x;  cosx := cos x;
		sinhy := sinh y; coshy := cosh y;
		d := sq cosx + sq sinhy;
		complex(sinx*cosx/d, sinhy*coshy/d)
	}

	sinh(z: C): C == {
		x := real z; y := imag z;
		complex(sinh x*cos y, cosh x*sin y)
	}
	cosh(z: C): C == {
		x := real z; y := imag z;
		complex(cosh x*cos y, sinh x*sin y)
	}
	tanh(z: C): C == {
		x := real z; y := imag z;
		sinhx := sinh x; coshx := cosh x;
		siny  := sin y;  cosy  := cos y;
		d := sq sinhx + sq cosy;
		complex(sinhx*coshx/d, siny*cosy/d)
	}

	--!! These need to be done accurately.
	asin (z: C): C == -%i*log(%i*z + sqrt(1-sq z));
	acos (z: C): C == -%i*log(z + %i*sqrt(1-sq z));
	atan (z: C): C == (log(1 + %i*z) - log(1 - %i*z))/complex(0,2.0);
  	asinh(z: C): C == log(z + sqrt(1 + sq z));
  	acosh(z: C): C == 2.0*log(sqrt(0.5*(z+1)) + sqrt(0.5*(z-1)));
  	atanh(z: C): C == (log(1 + z) - log(1 - z))/complex(2.0, 0);
}
