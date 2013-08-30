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

