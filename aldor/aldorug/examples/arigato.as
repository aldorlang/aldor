--
-- arigato.as: A main Axiomxl program calling the C function `nputs'.
--
#include "aldor"

MI ==> MachineInteger;

import { nputs: (MI, String) -> MI } from Foreign C;
import from MI, String;

nputs(3, "Arigato gozaimasu!");

