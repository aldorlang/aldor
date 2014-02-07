#include "axllib"

GeneralAssert: with {
    assertTrue: Boolean -> ();
    assertFalse: Boolean -> ();
    assertTrue: (String, Boolean) -> ();
    assertFalse: (String, Boolean) -> ();
    assertFail: ( () -> ()) -> ();
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
            print << s << newline;
            error s;
    }

    fail(): Exit == fail("Failed.");

    assertFail(f: () -> ()): () == {
        flg := true;
        try { f();
              flg := false}
        catch E in {
                flg := true
        }
        if not flg then fail("Expected an exception")
    }
}

Assert(T: with): with {
    if T has BasicType then {
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

    local (a: String) + (b: String): String == concat(a, b);

    if T has BasicType then {
      assertEquals(a: T, b: T): () == if not(a = b) then {
         fail("expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
         }

      assertEquals(s: String, a: T, b: T): () == if not(a = b) then {
         fail(s + ": expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
      }
    }

    string(T: BasicType)(t: T): String == {
       buf: Buffer := new();
       if T has BasicType then
              coerce(buf) << t;
       else
              coerce(buf) << "??";

       string(buf)
    }

}
