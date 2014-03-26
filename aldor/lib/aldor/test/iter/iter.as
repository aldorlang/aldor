#include "aldor"
#pile
import from Assert Integer

Tbl: with 
  new: Integer -> %
  generator: % -> Generator Cross(Integer, Integer)
== add
  Rep == Integer
  import from Rep

  new(n: Integer): % == per n;

  generator(t: %): Generator Cross(Integer, Integer) == (x, x+1) for x in 1..rep(t);


test(): () ==
   import from Tbl
   import from Integer
   tbl := new(4)
   count := 0
   for (x, y) in tbl repeat
      assertEquals(y, x+1)
      count := count + 1
   assertEquals(count, 4)

test()

