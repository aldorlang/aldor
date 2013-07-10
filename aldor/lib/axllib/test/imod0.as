-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib 


#include "axllib"
#library IMOD "imod.ao"

import from IMOD;
import from SingleInteger, Integer, Segment SingleInteger, Segment Integer;

#if Big
  Z==> Integer;
  Zp==> IntegerMod 5;
#else
  Z==> SingleInteger;
  Zp==> SingleIntegerMod 5;
#endif

Tables(Z, Zp) ==> {
	n: Zp := 10;
	--import from Zp;
	default i, j, lo, hi: SingleInteger;

	lo := -3;
	hi := 8;

	put(n: SingleInteger): TextWriter == {
		if n >= 0 then print << " ";
		print << n
	}


	table(s: String, op: (Zp, Zp) -> Zp): () == {
		print << " " << s << "  ";
		for j in lo..hi repeat put j << " ";
		print << newline;

		print << "    -----------------------------------" << newline;
		for i in lo..hi repeat {
			put i << ":  ";
			for j in lo..hi repeat
				print << op(i::Zp,j::Zp) << "  ";
			print << newline;
		}
		print << newline;
	}

	table("+", +);
	table("-", -);
	table("*", *);
	table("/", (a: Zp, b: Zp): Zp +-> if b=0 then 0 else a/b);

	print << " ^  ";
	for k in 0..(10@Integer) repeat print << k << "  ";
	print << newline;
	print << "    ---------------------------------" << newline;
	for i in lo..hi repeat {
		put i << ": ";
		for k in 0..(10@Integer) repeat
			print << i::Zp ^ k << "  ";
		print << newline;
	}
	print << newline;
}


f(): () == Tables(SingleInteger, SingleIntegerMod 5);
g(): () == Tables(Integer,       IntegerMod 5);

f();
g();
