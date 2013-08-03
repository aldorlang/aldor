#include "algebra"
#pile

TestRandomNumberGenerator: TestCategory with == add
  test(): () == 
    import from Assert MachineInteger
    import from MachineInteger
    import from List MachineInteger
    import from RandomNumberGenerator
    r := [n for n in randomGenerator()$RandomNumberGenerator for i in 1..10]
    assertEquals(10, #r)

import from TestRandomNumberGenerator;
test()
