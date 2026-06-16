#include "foamlib"
#pile

import from MachineInteger

fact(n: ?): ? == if n = 1 then 1 else n * fact(n-1)
