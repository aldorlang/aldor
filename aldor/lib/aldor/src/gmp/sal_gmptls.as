---------------------------- sal_gmptls.as -------------------------------
--
-- This file provides undocumented tools for GMP integer and float types
--
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Z == MachineInteger;
	BW == BinaryWriter;
	PZ == Record(z:Z);
}

GMPTools: with {
	readlimbs!: BinaryReader -> (Z, Pointer);
	writelimbs!: (BW, Z, Z, Generator Z) -> BW;
} == add {
	local wordsize:Z == bytes;

	writelimbs!(p:BW, sgn:Z, nlimbs:Z, limbs:Generator Z):BW == {
		import from Boolean, Z, Byte;
		p := p << sgn;				-- write sign first
		zero? sgn => p;
		p := p << nlimbs;			-- write size (limbs)
		for n in limbs repeat {
			nn := n;
			for m in 1..wordsize repeat {
				p := p << lowByte nn;
				nn := shift(nn, -8);
			}
		}
		p;
	}

	readlimbs!(p:BinaryReader):(Z, Pointer) == {
		import from Boolean, Byte, PZ;
		nlimbs:Z := << p;			-- read size first
		assert(nlimbs >= 0);
		p:PrimitiveArray Z := new nlimbs;
		byte := lowByte nlimbs;
		for i in 0..prev nlimbs repeat {
			n:Z := 0;
			st:Z := 0;
			for m in 1..wordsize repeat {
				byte := << p;
				n := n \/ shift(byte::Z, st);
				st := st + 8;
			}
			p.i := n;
		}
		(nlimbs, pointer p);
	}
}
