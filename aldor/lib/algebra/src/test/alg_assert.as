#include "algebra"

extend Assert(T: with): with {
    if T has AbelianMonoid then {
        assertZero: T -> ();
        assertNotZero: T -> ();
    }
}
== add {
    import from GeneralAssert;
    import from String;
    if T has AbelianMonoid then {
      local string(a: T): String == {
         buf: StringBuffer := new();
	 coerce(buf) << a;
	 string(buf);
      }
      assertZero(a: T): () == if zero? a then fail("Expected 0, got " + string(a));
      assertNotZero(a: T): () == if not zero? a then fail("Expected 0, got " + string(a));
    }

}