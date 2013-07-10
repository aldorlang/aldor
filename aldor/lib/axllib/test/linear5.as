--> testerrs

#include "axllib"

main(n:SingleInteger, m:SingleInteger):SingleInteger ==
{
   local msg:String;


   -- This is okay because throw(RuntimeError()) has type Exit which
   -- satisfies any type including SingleInteger.
   (m > n) =>
      throw RuntimeError("m > n");


   -- This is also okay for the same reasons. However, the type checker
   -- seems to use "msg" as the return value which must be examined to
   -- type check the value of this sequence.
   -- [This was due to "throw" being treated as follower and not maybe
   -- infix. Hence the ";" before the "throw" was being deleted.]
   (n < m) =>
   {
      msg := "n < m";
      throw RuntimeError(msg);
   }

   42;
}


import from SingleInteger;
main(21, 64);

