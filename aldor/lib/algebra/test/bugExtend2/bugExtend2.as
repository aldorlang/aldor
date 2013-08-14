-----bugExtend2.as
--
-- aldor -g interp bugExtend2.as
--
 
#include "aldor"
 
define definedIOType == Join(InputType,OutputType);
 
extend MachineInteger: definedIOType == add; 
#include "algebra"

(MachineInteger has InputType) 
