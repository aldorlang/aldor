#include "algebra"
#pile

TestFold: TestCategory with == add
  import from Assert MachineInteger
  import from MachineInteger
  import from List MachineInteger
  import from RandomNumberGenerator
  import from Fold MachineInteger;

  test(): () ==
    assertEquals(0, (+)/[0]@List(MachineInteger));

    test1();
    test2()
  test1(): () ==
    for n in 1..100 repeat
        assertEquals(n * (n+1) quo 2, (+)/[i for i in 1..n])

  test2(): () ==
    for n in 1..100 repeat
        assertEquals(n * (n+1) quo 2, (+)/(i for i in 1..n))

import from TestFold;
test()
