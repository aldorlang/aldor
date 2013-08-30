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
    for i in 1..10 repeat
        x := random(5)$MachineInteger
        assertTrue(x < 5 and x >= 0)

import from TestRandomNumberGenerator;
test()
