--> testrun
--> testgen c

#include "aldor"
#pile

import
        puts: String -> MachineInteger
from Foreign C

puts(n: AldorInteger): AldorInteger == n

import from String

puts "Hello world!"

