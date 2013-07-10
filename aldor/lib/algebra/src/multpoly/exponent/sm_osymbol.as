------------------------------------------------------------------------------
--
-- sm_osymbol.as: Symbols as a type of variables.
--
------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

#include "algebra"


OrderedSymbol: VariableType with { 
        orderedSymbol: String -> %;
          ++ `orderedSymbol(s)' converts `s' to an element
          ++ of the type. Note that `s' is copied.
} == Symbol add { 
       Rep == String;
       import from Rep;
       orderedSymbol(s: String): % == {
          import from Symbol;
          per name (- s);
       }
       variable(s:Symbol): Partial(%) == {
         import from Partial(%);
         [per (name s)];    
       }
       symbol(s:%): Symbol == {
          import from Symbol;
          - (rep s);
       }
       (x: %) < (y: %): Boolean == {
          rep(x) < rep(y)
       }
}

#if ALDOC
\thistype{OrderedSymbol}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this}
\Descr{\this~implements symbols as a type of variables.}
\begin{exports}
\category{\astype{VariableType}} \\
\alexp{orderedSymbol}: & \astype{String} $\to$ \% & Conversion to an element of the type. \\
\end{exports}
#endif





