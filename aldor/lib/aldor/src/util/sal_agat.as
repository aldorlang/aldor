---------------------------- sal_agat.as ----------------------------------
--
-- Interface to the Agat library, needed to support the AGAT macro
--
-- Copyright (c) Helene Prieto 1999
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

Agat: with {
	agat: (String, MachineInteger) -> ();
	agat: (String, Character) -> ();
	agat: (String, SingleFloat) -> ();
	agat: (String, DoubleFloat) -> ();
} == add {
	import {
		agatSendChar: (Pointer, Character) -> ();
		agatSendLong: (Pointer, MachineInteger) -> ();
		agatSendFloat: (Pointer, SingleFloat) -> ();
		agatSendDouble: (Pointer, DFlo$Machine) -> ();
	} from Foreign C;

	agat(str: String, val: MachineInteger): () == {
		agatSendLong(pointer str,val);
	}	
	agat(str: String, val: Character): () == {
		agatSendChar(pointer str,val);
	}
	agat(str: String, val: SingleFloat): () == {
		agatSendFloat(pointer str,val);
	}
	agat(str: String, val: DoubleFloat): () == {
		agatSendDouble(pointer str,val::(DFlo$Machine));
	}
}

