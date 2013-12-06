#include "aldor"

GeneralAssert: with {
    assertTrue: Boolean -> ();
    assertFalse: Boolean -> ();
    assertTrue: (String, Boolean) -> ();
    assertFalse: (String, Boolean) -> ();

    fail: () -> Exit;
    fail: String -> Exit;
}
== add {
    import from TextWriter;
    import from String;
    import from Character;

    assertTrue(f: Boolean): () == if not f then fail();
    assertTrue(s: String, f: Boolean): () == if not f then fail(s);
    assertFalse(f: Boolean): () == if f then fail();
    assertFalse(s: String, f: Boolean): () == if f then fail(s);
    -- TODO: Replace overload with named arguments
    fail(s: String): Exit == {
    	    stdout << s << newline;
	    error s;
    }

    fail(): Exit == fail("Failed.");
}

Assert(T: with): with {
    if T has PrimitiveType then {
      assertEquals: (T, T) -> ();
      assertEquals: (String, T, T) -> ();
    }

    export from GeneralAssert;
}
== add {
    import from TextWriter;
    import from String;
    import from Character;
    import from GeneralAssert;
    
    if T has PrimitiveType then {
      assertEquals(a: T, b: T): () == if not(a = b) then {
         fail("expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
	 }

      assertEquals(s: String, a: T, b: T): () == if not(a = b) then {
         fail(s + ": expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
      }
    }

    string(T: PrimitiveType)(t: T): String == {
       buf: StringBuffer := new();
       if T has OutputType then
       	      coerce(buf) << t;
       else
	      coerce(buf) << "??";

       string(buf)
    }

}

import from MachineInteger, RandomNumberGenerator;
seed(randomGenerator(0), 1);
