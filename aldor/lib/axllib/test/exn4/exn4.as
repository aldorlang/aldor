--> testrun -laxllib

#include "axllib"

AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- The decoration of the return type of this function with an exception
-- type segfaults the compiler in titdn: missing a tpossRefer.
foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   throw AnError;
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

