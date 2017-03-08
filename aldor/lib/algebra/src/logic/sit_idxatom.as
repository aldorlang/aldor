#include "algebra"

IndexedAtom: Join(HashType, LogicAtom) with {
	atom: Integer -> %;
	index: % -> Integer;
	negate: % -> %;
	negated?: % -> Boolean;
	isNegation?: (%, %) -> Boolean;
	positive: % -> %;
}
== add {
   Rep == Integer;
   import from Rep;

   atom(x: Integer): % == {
       x <= 0 => never;
       per x;
   }

   extree(a: %): ExpressionTree == extree rep(a);

   (a: %) < (b: %): Boolean == rep(a) < rep(b);
   (a: %) = (b: %): Boolean == rep(a) = rep(b);

   index(x: %): Integer == abs(rep(x));

   negate(x: %): % == per(-rep(x));
   positive(x: %): % == if rep(x) < 0 then per(-rep(x)) else x;
   negated?(x: %): Boolean == rep(x) < 0;

   isNegation?(a: %, b: %): Boolean == rep(a) = -rep(b);

   hash(a: %): MachineInteger == hash rep a;
}

#if ALDORTEST
#include "algebra"
#include "aldorio"
#pile

test(): () ==
    import from Assert IndexedAtom
    import from IndexedAtom
    import from LogicAtomTests IndexedAtom
    import from Integer
    a1 := atom 1
    a2 := atom 2
    assertTrue(testAtom(a1))
    assertTrue(testAtom(a2))
    assertNotEquals(a1, a2)
    assertEquals(a1, atom 1)
    assertFalse negated? a1
    assertTrue negated? negate a1
    
#endif
