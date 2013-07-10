------------------------------------------------------------------------------
--
-- sm_fvt.as: A basic categoty for multivariate polynomial variables.
--
------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

#include "algebra"

+++ `FiniteVariableType' is a category for multivariate polynomial variables
+++ that belong to a finite set.
+++ Author: Marc Moreno Maza
+++ Date Created: 09/07/01
+++ Date Last Update: 09/07/01

define FiniteVariableType: Category == VariableType with {
        variable: MachineInteger -> %;
          ++ `variable(n)' returns the `n'-th variable of the type.
        index: % -> MachineInteger;
          ++ `index(x)' returns the index of the `x' in `t'.
        #:  MachineInteger;
          ++ `#' returns the number of elements of the type.
        max: %;
          ++ `max' is the greatest variable of the type.
        min: %;
          ++ `min' is the smallest variable of the type.
        minToMax: List %;
          ++ `minToMax' returns the list of the variables of the
          ++ type sorted in increasing order.
        maxToMin: List %;
          ++ `maxToMin' returns the list of the variables of the
          ++ type sorted in decreasing order.
}


#if ALDOC
\thistype{FiniteVariableType}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{\this: Category}
\Descr{\this~is a category for multivariate polynomial variables that belong to a finite set.
Hence, a domain of this category implements a finite set of ordered variables.
The operations \alexp{variable} and \alexp{index} define a one-to-one map
from \% onto a range of (machine) integers.}
\begin{exports}
\category{\astype{VariableType}} \\
\alexp{variable}: & \astype{MachineInteger}  $\to$  \% &  Retuns the $n$-th variable of the type, if any \\
\alexp{index}: &  \% $\to$ \astype{MachineInteger} &  Retuns the associated machine integer \\
\alexp{\#}: & \astype{MachineInteger}  & The number of elements of the type \\
\alexp{max}: & \% & The greatest element of the type \\
\alexp{min}: &  \% & The smallest element of the type   \\
\alexp{minToMax}: & \astype{List} \% & The elements of the type sorted in increasing order  \\
\alexp{maxToMin}: &  \astype{List} \%   & The elements of the type sorted in decreasing order  \\
\end{exports}
#endif



#if ALDOC
\alpage{variable}
\Usage{\name~i}
\Signature{\altype{MachineInteger}}{\%}
\Params{{\em i} & \altype{MachineInteger} & an index \\ }
\Retval{Returns the $i$-th variable of the type if $i$ is the range
of indices associated with the type. Otherwise, an error or an exception is
raised.}
#endif


