------------------------------------------------------------------------------
--
-- sm_vt.as: A basic categoty for multivariate polynomial variables.
--
------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

#include "algebra"

+++ `VariableType' is a category for multivariate polynomial variables.
+++ Variables are looked as symbols with additionnal properties such 
+++ as a total order.
+++ Author: Marc Moreno Maza
+++ Date Created: 08/07/01
+++ Date Last Update: 09/07/01

define VariableType: Category == Join(TotallyOrderedType, 
 ExpressionType, HashType, SerializableType, Parsable) 
    with { 
        variable: Symbol -> Partial(%);
          ++ `variable(s)' returns the variable associated with `s' 
          ++ if any, otherwise `failed' is returned.
        symbol: % -> Symbol;
          ++ `symbol(x)' returns the symbol associated with `x'.
}


#if ALDOC
\thistype{VariableType}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{\this: Category}
\Descr{\this~is a category for multivariate polynomial variables looked as 
symbols with additionnal properties such as a total order.}
\begin{exports}
\category{\astype{TotallyOrderedType}} \\
\category{\astype{ExpressionType}} \\
\category{\astype{Parsable}} \\
\category{\astype{SerializableType}} \\
\category{\astype{HashType}} \\
\alexp{variable}: & \astype{Symbol} $\to$ \astype{Partial} \% & Associated variable, if any \\
\alexp{symbol}: & \% $\to$ \astype{Symbol}  & Associated symbol \\
\end{exports}
#endif



