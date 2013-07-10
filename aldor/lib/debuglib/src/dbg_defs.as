

-- The following are only used when compiling the library.
#if UseAxllib
#include "axllib"
SetCategory ==> BasicType;
#elseif UseAldorlib
#include "aldor"
SetCategory ==> BasicType;
SingleInteger ==> MachineInteger;
define BasicType: Category == Join(PrimitiveType, OutputType, HashType);
#elseif UseBasicmath
#include "basicmath"
import from TypePackage, Boolean;
#else
#error Please compile with -DUseAxllib or -DUseBasicmath or DUseAldor
#endif


-- Macros to avoid clashes with other libraries: all these domains
-- actually local to the debugging library and aren't supposed to
-- be visible to clients.
BreakPoint		==> DBG__BreakPoint;
BreakPointPackage	==> DBG__BreakPointPackage;
CallContext		==> DBG__CallContext;
IntStack		==> DBG__IntStack;
DebuggerState		==> DBG__InternalState;
DebugUtilities		==> DBG__DebugUtilities;
HelpPackage		==> DBG__HelpPackage;
JustifyPackage		==> DBG__JustifyPackage;
TextFileIndex		==> DBG__TextFileIndex;
TextFileTable		==> DBG__TextFileTable;
UserInterface		==> DBG__UserInterface;


-- Include the file that the clients use.
#include "debuglib"
