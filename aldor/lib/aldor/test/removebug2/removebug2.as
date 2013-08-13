#include "aldor"
#include "aldorio"

import from MachineInteger;
import from SortedList MachineInteger;

-------------------------------

main(): () == {
  a := [1,2,3];
  c := removeAll(2,a);
  stdout << c << endnl;
  a := [1,2,3];
  c := removeAll!(2,a);
  stdout << c << endnl;
}

main();