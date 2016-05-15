#include "aldor"

FoldingTransformationCategory(T: with): Category == with {
    /: (%, List T) -> T;
    /: (%, Generator T) -> T;
}

Fold(T: with): FoldingTransformationCategory(T) with {
	 /: (f: (T,T) -> T, List T) -> T;
	 /: (f: (T,T) -> T, Generator T) -> T;
	 folder: ((T, T) -> T) -> %
} == add {
  Rep ==> (T, T) -> T;

  folder(f: (T, T) -> T): % == per f;

  (f: (T,T) -> T) / (l: List T): T == (per f)/l;
  (f: (T,T) -> T) / (g: Generator T): T == (per f)/g;

  (folder: %) / (l: List T): T == {
     empty? l => never;
     acc := first l;
     for elt in rest l repeat {
         acc := rep(folder)(acc, elt);
     }
     return acc;
  }

  (folder: %) / (g: Generator T): T == {
     first := true;
     local acc: T;
     for elt in g repeat {
         acc := if first then elt else rep(folder)(acc, elt);
	 first := false;
     }
     first => never;
     return acc;
  }
}

FoldingTransformationCategory2(T: with, R: with): Category == with {
    /: (%, List T) -> R;
    /: (%, Generator T) -> R;
}

Fold2(T: with, R: with): FoldingTransformationCategory2(T, R) with {
	 /: (Cross(f: (T,R) -> R, R), List T) -> R;
	 /: (Cross(f: (T,R) -> R, R), Generator T) -> R;
	 /: (Cross(f: (R, T) -> R, R), List T) -> R;
	 /: (Cross(f: (R, T) -> R, R), Generator T) -> R;
	 folder: ((T, R) -> R, R) -> %
} == add {
  Rep ==> Cross((T, R) -> R, R);
  local fper(f: (T, R) -> R, init: R): % == { c:=(f,init); per c}
  folder(f: (T, R) -> R, init: R): % == fper(f, init);

  (c: Cross((T,R) -> R, R)) / (l: List T): R == { (f, init) := c; fper(f, init)/l }
  (c: Cross((T,R) -> R, R)) / (g: Generator T): R == { (f, init) := c; fper(f, init)/g }
  (c: Cross((R,T) -> R, R)) / (l: List T): R == { (f, r) := c; (swapArgs f, r)/l }
  (c: Cross((R,T) -> R, R)) / (g: Generator T): R == { (f, r) := c; (swapArgs f, r)/g }

  local swapArgs(f: (R, T) -> R)(t: T, r: R): R == f(r, t);

  (folder: %) / (l: List T): R == {
     empty? l => never;
     (f, init) := rep folder;
     acc := init;
     for elt in l repeat {
         acc := f(elt, acc);
     }
     return acc;
  }

  (folder: %) / (g: Generator T): R == {
     (f, init) := rep folder;
     local acc: R := init;
     for elt in g repeat {
         acc := f(elt, acc);
     }
     return acc;
  }
}

BooleanFold: with {
    /: ('_and', List Boolean) -> Boolean;
    /: ('_and', Generator Boolean) -> Boolean;

    /: ('_or', List Boolean) -> Boolean;
    /: ('_or', Generator Boolean) -> Boolean;
    export from '_and', '_or';
}
== add {
   (/)(x: '_and', l: List Boolean): Boolean == _and/(generator l);
   (/)(x: '_and', l: Generator Boolean): Boolean == {
	for b in l repeat if not b then return false;
	return true;
   }

   (/)(x: '_or', l: List Boolean): Boolean == _or/(generator l);
   (/)(x: '_or', l: Generator Boolean): Boolean == {
	for b in l repeat if b then return true;
	return false;
   }

}


#if ALDORTEST
#include "aldor"
#include "aldorio"

testSum(): () == {
    import from Assert Integer;
    import from Integer;
    import from Fold Integer;
    for n in 1..10 repeat
        assertEquals(n * (n+1) quo 2, (+)/(x for x in  1..n));
}


testBoolean(): () == {
   import from BooleanFold;
   import from Assert Boolean;
   import from List Boolean;

   assertTrue(_and/[]);
   assertTrue(_and/[true]);
   assertTrue(_and/[true, true]);
   assertTrue(_and/[true, true, true]);
   assertFalse(_and/[true, false, true]);

   assertFalse(_or/[]);
   assertFalse(_or/[false]);
   assertTrue(_or/[false, true]);
   assertTrue(_or/[true, false, true]);
}

testLazyBoolean(): () == {
   import from BooleanFold;
   import from Assert Boolean;
   import from Integer;

   local qq := 0;
   _or/((qq := qq + 1; even?(x) ) for x in 1..10);
   assertTrue(qq = 2);
   qq := 0;
   _and/((qq := qq + 1; x rem 3 > 0 ) for x in 1..10);
   assertTrue(qq = 3);
}

testSum();
testLazyBoolean();
testBoolean();
#endif
