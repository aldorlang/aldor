#include "aldor"
#include "aldorio"
#include "algebra"

macro {
   Z == Integer;
   MI == MachineInteger;
   Bool == Boolean;
}
 
main():() == {
   import from Z,Bool;
   import from Partial(Bool);
   tw:Partial(Bool) := prime?(17);
   stdout << retract(tw) << newline;
    
 
 
}
main();

