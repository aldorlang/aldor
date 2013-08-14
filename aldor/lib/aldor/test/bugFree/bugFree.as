------------------------------------
--
-- description:
--   Declaration with free gives a value
--
--
-- executed via:
--   aldor -ginterp bugFree.as
--
------------------------------------

#include "aldor"
#include "aldorio"

import from TextWriter;
import from String;
import from StringBuffer;
import from Character;

{
        local debugWriter : TextWriter;

        testWriter(ac:()->TextWriter):() == {
           local debugStringWriter : StringBuffer := new();
           free debugWriter := debugStringWriter :: TextWriter;

	   debugWriter << "(direct piping)";
	   stdout << "direct piping was ok" << newline;
	   flush! stdout;

           ac() << "(piping via function)";
	   stdout << "piping via function was ok" << newline;
           stdout << "debugStringWriter afterwards: " << debugStringWriter << newline;
        }

	acquireDbgoutWorking():TextWriter == {
                free debugWriter;
                debugWriter;
	}

	acquireDbgoutFailing():TextWriter == {
                free debugWriter;
	}

}

-----------------------

testWriter(acquireDbgoutWorking);

stdout << newline;

testWriter(acquireDbgoutFailing);
