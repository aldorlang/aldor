--
-- poker.as: A main program using functions from "choose.as".
--
#include "aldor"
#include "aldorio"

import from ChooseLib;
import from Integer;

pok := choose(52, 5);
stdout << "The number of different poker hands is " << pok << newline;
