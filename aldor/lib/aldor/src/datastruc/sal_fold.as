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
	 folder: ((T, R) -> R, R) -> %
} == add {
  Rep ==> Cross((T, R) -> R, R);
  local fper(f: (T, R) -> R, init: R): % == { c:=(f,init); per c}
  folder(f: (T, R) -> R, init: R): % == fper(f, init);

  (c: Cross((T,R) -> R, R)) / (l: List T): R == { (f, init) := c; fper(f, init)/l }
  (c: Cross((T,R) -> R, R)) / (g: Generator T): R == { (f, init) := c; fper(f, init)/g }

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

