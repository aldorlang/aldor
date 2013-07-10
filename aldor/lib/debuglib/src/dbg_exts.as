-----------------------------------------------------------------------------
----
---- extes.as:  -- some extends .
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "dbg_defs"
SI     ==> SingleInteger;

extend Array(T:Type): with {
	extend!: (%, T) -> %;
	++ extend!(carr, c) append c to the end of array carr

} == add {

	extend!(carr: %, c: T): % == {
		import from SI;
		last:SI := #carr;	
		result: % := new(last+1);
		for i in 0..#carr-1 repeat result.i := carr.i;
		result.last := c;
		free! carr;
		result;
	}
}


extend String: HashType with {
                                                                                                                              
        string: Array Character -> %;
                ++ `string ac' creates a string out of an array of characters.

	--hash: % -> MachineInteger;
}
== add {
	import from SI, Character;

        string(a: Array Character): % == {
                l := #a;
                r: % := new(l);
                for i in 0..l-1 repeat r.i := a.i;
                r
        }

	hash(s:%): MachineInteger == {
		
		ans: MachineInteger := 0;
		for i in 0..prev #s repeat {
			ans := ans + ord s.i;
		}
			
		ans;
	}
}

extend TextReader: with {
	readline!: (%) -> String;
		++ `readline! t' reads a line from TextReader and returns
		++ a new string containing it.  The string will end with a
		++ newline character, unless the end is reached without one.
}
== add {

	import from Character;

        readline!(rdr: %): String == {
		import from TextWriter, Array Character;
                buf: Array Character := empty;

                repeat {
			c := read! rdr;
                        c = null  => break;
    	                buf := extend!(buf, c);
			c = newline => break;
                }
                s := string buf;
                free! buf;

                s
        }
}