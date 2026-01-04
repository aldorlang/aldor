#include "foamlib"

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
    if T has BasicType then {
      assertEquals: (T, T) -> ();
      assertEquals: (String, T, T) -> ();
      assertNotEquals: (T, T) -> ();
      assertNotEquals: (String, T, T) -> ();
    }

    export from GeneralAssert;
}
== add {
    import from TextWriter;
    import from String;
    import from Character;
    import from GeneralAssert;
    
    if T has BasicType then {

      assertEquals(a: T, b: T): () == if not(a = b) then {
         fail("expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
	 }

      assertEquals(s: String, a: T, b: T): () == if not(a = b) then {
         fail(s + ": expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
      }
      assertNotEquals(a: T, b: T): () == if (a = b) then {
         fail("didn't expect " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
	 }

      assertNotEquals(s: String, a: T, b: T): () == if (a = b) then {
         fail(s + ": didn't expect " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
      }

      -- FIXME: version in aldorlib should be local too
      local string(X: BasicType)(t: X): String == {
       buf: Array Character := [];
       writer(buf) << t;

       string(buf)
    }

  }
}

