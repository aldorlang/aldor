-------------------------------------------------------------------------
--
-- eio_rsto.as
--
-------------------------------------------------------------------------

#include "aldor"

+++\begin{ardescription}{\arthis~provides functions for \Aldor's memory 
+++management.}
+++  This packages provides two functions. One function 
+++  (\arname{printMemoryOverview}) is used to print an overview of the 
+++  used memory at runtime while the other function 
+++  (\arname{invokeGarbageCollector}) is used to invoke \Aldor's Carbage 
+++  Collector explcitely.
+++\end{ardescription}
RuntimeStorage: with {

    +++begin{ardescription}{invokes \Aldor's garbage collector explictely.}
    +++  Calling \arname~forces collection of garbage. This function is used
    +++  typically, when \Aldor by itself is too lazy to collect garbage.
    +++\end{ardescription}
    invokeGarbageCollector: () -> ();
    
    
    +++begin{ardescription}{prints an overview of currently allocated memory
    +++to \arname[TextWriter]{stderr}.}
    +++  This overview contains detailed information about allocated pages of
    +++  memory and objects.
    +++\end{ardescription}
    printMemoryOverview: () -> ();

} == add {
    
    -----------------------------------------

    import { 
	stoGc: () -> (); 
	stoShow: () -> ();
    } from Foreign C;
    
    -----------------------------------------

    invokeGarbageCollector(): () == {
	stoGc();
    }

    -----------------------------------------

    printMemoryOverview(): () == {
	stoShow();
    }
    
    -----------------------------------------

}