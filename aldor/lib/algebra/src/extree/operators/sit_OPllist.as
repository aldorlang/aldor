--------------------------   sit_OPllist.as   ------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeLispList}
\History{Manuel Bronstein}{25 September 96}{created}
\Usage{import from \this}
\Descr{\this~is the lisp list operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following function is not yet implemented and produces dummy results :
{\bf fortran}.
\end{remarks}
#endif
	
macro Z	== MachineInteger;

ExpressionTreeLispList : ExpressionTreeOperator == ExpressionTreeList add {
	name:Symbol	== { import from String; -"lisplist" }
	uniqueId:Z	== UID__LLIST;
}

