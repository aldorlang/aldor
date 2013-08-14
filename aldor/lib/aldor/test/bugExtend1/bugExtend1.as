-----bugExtend1.as
--
-- aldor -g interp bugExtend1.as
--
 
#include "aldor"
 
define definedIOType == Join(InputType,OutputType);
 
extend MachineInteger: DataStructureType;--definedIOType;
--#include "algebra"
 
(MachineInteger has InputType)
