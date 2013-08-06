--> testrun -laxllib

#include "axllib"

define AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- Having fixed the segfault bug with this code, we now get a type error.
-- The compiler claims that "throw AnError" does not satisfy the context
-- Exit throw RuntimeException. The user guide is ambiguous about this ...
foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   throw AnError;
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

