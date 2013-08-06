#include "aldor"
#include "aldorio"

import from MachineInteger;

for i in 1..7 repeat {
   stdout << "Creating array of size 10^" << i << "... " << flush;
   isprime : PrimitiveArray MachineInteger := new (i^10, 0);
   stdout << "ok" << newline;
}
