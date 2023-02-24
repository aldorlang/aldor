#include "aldor"

HashSet(T: HashType): BoundedFiniteDataStructureType T with {
    bracket: Tuple T -> %;
    bracket: Generator T -> %;
    insert!: (%, T) -> ();
    empty: () -> %;
}
== add {
    Rep ==> HashTable(T, Boolean);
    import from Rep;
    import from MachineInteger;
    default set: %;
    
    (a: %) = (b: %): Boolean == {
        import from BooleanFold;
        # a = #b and (_and)/(member?(elt, a) for elt in b)
    }

    generator set: Generator T == {
        k for (k, v) in rep set;
    }
    # set: MachineInteger == numberOfEntries(rep set);
    free! set: () == free! rep set;
    empty? set: Boolean == empty? rep set;
    empty(): % == per table();
    copy set: % == per copy rep set;

    insert!(set, e: T): () == {
        set!(rep set, e, true);
    }
    
    member?(e: T, set): Boolean == {
        import from Partial Boolean;
        not failed? find(e, rep set);
    }
    
    [(tuple: Tuple T)]: % == {
        import from MachineInteger;
        acc := empty();
	for n in 1..length tuple repeat
	    insert!(acc, element(tuple, n));
	acc
    }
    
    [(g: Generator T)]: % == {
        acc := empty();
	for elt in g repeat insert!(acc, elt);
	acc;
    }

    if T has OutputType then
        (tw: TextWriter) << (set: %): TextWriter == {
	    import from String;
	    import from T;
	    tw << "[";
	    sep := "";
	    for k in set repeat {
	        tw << sep << k;
		sep := ", ";
	    }
	    tw << "]";
	}
	
}

#if ALDORTEST
#include "aldor"
#include "aldorio"
#pile

test(): () ==
    import from Assert Integer
    import from Fold2(Integer, Integer)
    import from Integer
    import from HashSet Integer
    e: HashSet Integer := [n for n in 1..1000]
    assertEquals(500*1001, (+, 0)/(x for x in e));
    assertTrue(empty? empty())
    assertFalse(empty? e)
    
test();
#endif
