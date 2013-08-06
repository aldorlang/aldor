-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testrun -laxllib -Q3


#include "axllib"

main():() ==
{
   local x:SingleInteger := 42;

   nasty():() ==
   {
      free x:SingleInteger;
      x := x + 1;
   }


   funky(y:SingleInteger):() ==
   {
      -- If the parameter `y' is not used more than once in this
      -- function then the compiler will try to avoid creating a
      -- temporary to hold the value of the argument when inlining
      -- us. However, if we are called with the free variable `x'
      -- as the argument then we MUST create a temporary even if
      -- we only use the argument once. This is demonstrated here
      -- by modifying the free variable before using the value
      -- at the point of application. Without a temporary we get
      -- the value after the updated which is wrong.
      nasty();


#if UseTwice
      print << "y = "<<  y << " = " << y << newline;
#else
      print << "y = " << y << newline;
#endif
   }


   -- Tell the user what to expect.
   print << "*** Expect y=42, y=43, x=44" << newline << newline;


   -- Call the test function several times.
   funky(x);
   funky(x);


   -- Finally show the value of the free variable.
   print << "x = " << x << newline; 
}

main();
