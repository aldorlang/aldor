#include "algebra"

Assert(T: with): with {
    if T has PrimitiveType then {
      assertEquals: (T, T) -> ();
      assertEquals: (String, T, T) -> ();
    }
    if T has AbelianMonoid then {
      assertZero: T -> ();
      assertNotZero: T -> ();
    }
    fail: () -> Exit;
    fail: String -> Exit;
}
== add {
    import from TextWriter;
    import from String;
    import from Character;
    
    if T has PrimitiveType then {
      assertEquals(a: T, b: T): () == if not(a = b) then {
         fail("expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
	 }

      assertEquals(s: String, a: T, b: T): () == if not(a = b) then {
         fail(s + ": expected " + string(T)(a) + " got " + string(T)(b) + " " + string(Boolean)(a=b));
      }
    }
    if T has AbelianMonoid then {
      assertZero(a: T): () == if zero? a then fail("Expected 0, got " + string(T)(a));
      assertNotZero(a: T): () == if not zero? a then fail("Expected 0, got " + string(T)(a));
    }

    fail(s: String): Exit == {
    	    stdout << s << newline;
	    never
    }

    fail(): Exit == fail("Failed.");

    string(T: PrimitiveType)(t: T): String == {
       buf: StringBuffer := new();
       if T has OutputType then
       	      coerce(buf) << t;
       else
	      coerce(buf) << "??";

       string(buf)
    }

}
