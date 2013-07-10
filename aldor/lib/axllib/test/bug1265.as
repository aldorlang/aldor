--> testerrs

#include "axllib"

AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- Missing ; means that we really ought to have a suspicious juxtaposition
-- warning. As it stands the Execute macro extends to the ; before the final
-- print statement. After macro expansion we find no application of Execute
-- so we discard it leaving just the print statement.
--
-- Alternatively we ought to examine ALL macro definitions for bad recursion.
Execute(BLOCK) ==> BLOCK


Execute(
   try { throw AnError } catch E in
   {
      E has RuntimeException => printError(print)$E;
      true => throw E;
      never;
   }
);


print << "Successful termination." << newline;

