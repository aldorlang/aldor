------------------------------------------------------------------------------
--
-- sm_tupleovar.as: A basic constructor for sets of ordered variables
--
------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

#include "algebra"

+++ `OrderedVariableTuple(t)' creates a finite ordered set of variables 
+++ from a tuple of symbols.
+++ The first element in the tuple is the biggest variable in the domain.
+++ Author: Marc Moreno Maza
+++ Date Created: 23/05/01 
+++ Date Last Update: 09/07/01

OrderedVariableTuple(t:Tuple Symbol): FiniteVariableType with {

} == (OrderedVariableList(ls) add { 

} where {
     list(x: Tuple Symbol): List(Symbol) == {
        import from MachineInteger, Tuple(Symbol), List(Symbol);
        y == [element(x,i) for i in 1..length(x)];
     }
     ls == list(t);
})



#if ALDOC
\thistype{OrderedVariableTuple}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this~t}
\Params{
{\em t} & \astype{Tuple}~\astype{Symbol} & The symbols defining the variables of the type \\
}
\Descr{\this~{\em t}~implements the finite set of ordered variables given by {\em t}.\\
\this~{\em t} is implemented as \astype{OrderedVariableList}~{\em l}
where {\em l} is the list of items in {\em t} in the same order.}
\begin{exports}
\category{\astype{FiniteVariableType}} \\
\end{exports}
#endif
