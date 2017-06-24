----------------------------- aldor_gloop.as -----------------------------
--
-- This file, which is not part of the libaldor build, provides a stub
-- needed when using libaldor from the compiler's interactive loop. 
--
-- See the README file for compilation instructions (must be compiled twice)
--
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

-- The interactive loop prints value automatically if the type exports
-- <<: (TextWriter, %) -> TextWriter
-- and if there is a TextWriter constant called print.
extend TextWriter: with { print: % } == add { print:% == stdout }

extend Union(T:Tuple Type):with { <<: (TextWriter, %) -> TextWriter } == add {
	(p:TextWriter) << (a:%):TextWriter == {
		import from String;
		p << "[union]";
	}
}

extend List(T:Type): with { <<: (TextWriter, %) -> TextWriter } == add {
	if not(T has OutputType) then {
		(p:TextWriter) << (a:%):TextWriter == {
			import from String, MachineInteger;
			empty? a => p << "[]";
			p << "[*" << #a << "*]";
		}
	}
}

extend Array(T:Type): with { <<: (TextWriter, %) -> TextWriter } == add {
	if not(T has OutputType) then {
		(p:TextWriter) << (a:%):TextWriter == {
			import from String, MachineInteger;
			zero?(n := #a) => p << "[]";
			p << "[*" << n << "*]";
		}
	}
}

local initAldorInterpreterLoop():() == {
	import from RandomNumberGenerator, MachineInteger;
	seed(randomGenerator 0, 123456789);
}

initAldorInterpreterLoop();

