
#include "axllib.as"
--> testcomp
--> testrun -l axllib
--> testrun -O -l axllib

bobfun(bob: Object BasicType): () ==
   f avail bob where {
      f(T: BasicType, t: T) : () == {
         print << "This prints itself as: " << t << newline;
      }
   }


import from String, Integer, List Integer;

boblist: List Object BasicType := [
   object(String, "Ahem!"),
   object(Integer, 42),
   object(List Integer, [1, 2, 3, 4])
];

for bob in boblist repeat
   bobfun bob;
