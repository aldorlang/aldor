--> testrun -laxllib

#include "axllib"

define AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- In an attempt to prevent the compiler from complaining about the type
-- of a "throw" statement, here we force the function to have a void value
-- and are rewarded with a segfault.
foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   throw AnError;
   ();
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

