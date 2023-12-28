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
      assertNotEquals: (T, T) -> ();
      assertNotEquals: (String, T, T) -> ();
      equalityAxioms: (T, T, T) -> ();
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
      assertNotEquals(a: T, b: T): () == if (a = b) then {
         fail("didn't expect " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
	 }

      assertNotEquals(s: String, a: T, b: T): () == if (a = b) then {
         fail(s + ": didn't expect " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
      }

      equalityAxioms(a: T, b: T, c: T): () == {
          import from List List T, List T;
	  import from Integer;
	  toString := string(List T);
          for l in [[a], [b], [c]] repeat
	      if not (l.0 = l.0) then fail("reflexive");
          for l in [[a, b], [a, c], [b, c]] repeat
	      ( (l.1 = l.2) ~= (l.2 = l.1) ) => fail("commutative");
	  for l in [[a,b,c], [a,c,b], [b,a,c], [b,c,a], [c,a,b],[c,b,a]] repeat
	      (l.1 = l.2) and (l.2 = l.3) and not (l.1=l.3) => fail("transitive");
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

Assert(F: (X: Type) -> BoundedFiniteDataStructureType X, D: Type): with {
    assertSizeEquals: (MachineInteger, F D) -> ();
    if D has PrimitiveType then {
        assertMember: (D, F D) -> ()
    }
    export from Assert F D
}
== add {

    assertSizeEquals(n: MachineInteger, a: F D): () == assertEquals(n, #a)$Assert(MachineInteger);

    if D has PrimitiveType then {
        assertMember(d: D, a: F D): () == assertTrue(member?(d, a))$GeneralAssert
    }
}

import from MachineInteger, RandomNumberGenerator;
seed(randomGenerator(0), 1);
