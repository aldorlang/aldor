-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#include "axllib.as"

Foo:Category == with {
	foo: % -> %;
	default {
		foo(x:%):% == {
			% has Ring => x +$(% pretend Ring) x;
			x;
		}
	}
}

MyInt: Join(IntegerNumberSystem, Foo) == Integer add {};

-- if this extension is commented out, then the ouput below is correctly 6
extend Integer:Foo == add {};

import from MyInt;

-- expect 6
print << foo 3 << newline;


