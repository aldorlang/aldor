
--> testerrs

#include "axllib"

AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


AnotherError:with == add;


foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   -- This throw is wrong: typeOf(AnotherError) is (with == add) which
   -- doesn't satisfy RuntimeException. The bug is that the error message
   -- is ... type  except RuntimeException not ... type () except ...
   throw AnotherError;
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

