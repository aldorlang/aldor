#include "aldor"
#pile

Pol: with 
  expand1: % -> Generator Cross(Integer, Integer)
  expand2: % -> Generator Cross(Integer, Integer)
  expand3: % -> Generator Cross(Integer, Integer)
  expand4: % -> Generator Cross(Integer, Integer)
  zero: () -> %;
  one: () -> %;

== add
  Term == Record(coeff: Integer, deg: Integer)
  Rep == List Term
  import from Rep
  import from Term

  zero(): % == per []
  one(): % == per [[1, 0]]

  expand1(p: %): Generator(Cross(Integer, Integer)) == never

  expand2(p: %): Generator(Cross(Integer, Integer)) == generate
      for term in rep(p) repeat yield (term.coeff, term.deg)

  expand3(p: %): Generator(Cross(Integer, Integer)) == 
      (term.coeff, term.deg) for term in rep(p)

  expand4(p: %): Generator(Cross(Integer, Integer)) == generate
      l := rep p;
      while ~empty? l repeat 
            term := first l
	    l := rest l
      	    yield (term.coeff, term.deg)

test(): () ==
  import from Assert List Integer;
  import from List Integer;
  import from Integer;
  p: Pol := zero();
  expand2 p;
  expand3 p;
  expand4 p;

  p := one();
  expand2 p;
  expand3 p;
  expand4 p;

  l: List Integer := [x for (x, y) in expand1 p]
  assertEquals([1], l);

  l: List Integer := [y for (x, y) in expand1 p]
  assertEquals([0], l);

  l: List Integer := [x for (x, y) in expand2 p]
  assertEquals([1], l);

  l: List Integer := [y for (x, y) in expand2 p]
  assertEquals([0], l);

  l: List Integer := [x for (x, y) in expand3 p]
  assertEquals([1], l);

  l: List Integer := [y for (x, y) in expand3 p]
  assertEquals([0], l);

