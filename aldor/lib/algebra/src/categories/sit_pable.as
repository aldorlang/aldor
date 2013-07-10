----------------------------- sit_pable.as ----------------------------------
--
-- Expression Tree Arithmetic Interpreters
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	TREE	== ExpressionTree;
	LEAF	== ExpressionTreeLeaf;
}

#if ALDOC
\thistype{Parsable}
\History{Manuel Bronstein}{30/08/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types that convert
expression trees into themselves whenever possible.}
\begin{exports}
\category{\altype{InputType}}\\
\alexp{eval}: & \altype{ExpressionTree} $\to$ \altype{Partial} \% &
Interpret a tree\\
\alexp{eval}: & \altype{ExpressionTreeLeaf} $\to$ \altype{Partial} \% &
Interpret a leaf\\
\alexp{eval}:
& (\altype{MachineInteger}, \altype{List} \altype{ExpressionTree})
$\to$ \altype{Partial} \% & Interpret a node\\
\end{exports}
#endif

define Parsable: Category == InputType with {
	eval:	TREE -> Partial %;
	eval:	LEAF -> Partial %;
	eval:	(MachineInteger, List TREE) -> Partial %;
#if ALDOC
\alpage{eval}
\Usage{\name~e\\ \name~t\\ \name(op,[$e_1,\dots,e_n$])}
\Signatures{
\name: & \altype{ExpressionTree} $\to$ \altype{Partial} \%\\
\name: & \altype{ExpressionTreeLeaf} $\to$ \altype{Partial} \%\\
\name: & (\altype{MachineInteger}, \altype{List} \altype{ExpressionTree})
$\to$ \altype{Partial} \%\\
}
\Params{
{\em e},$e_i$ & \altype{ExpressionTree} & Expression trees\\
{\em t} & \altype{ExpressionTreeLeaf} & A leaf\\
{\em op} & \altype{MachineInteger} & Code for an operator\\
}
\Retval{\name(e) and \name(t) return the result of evaluating the
given tree or leaf in the type, while \name(op,[$e_1,\dots,e_n$])
returns the result of evaluating $op(e_1,\dots,e_n)$ in the type,
where $op$ is a code from {\tt include/algebrauid.as}.}
#endif
	default {
		eval(t:TREE):Partial % == {
			TRACE("parsable::eval ", t);
			leaf? t => eval leaf t;
			evalOp t;
		}

		local evalOp(t:TREE):Partial % == {
			import from Boolean;
			TRACE("parsable::evalOp ", t);
			assert(~leaf? t);
			eval(uniqueId$operator(t), arguments t);
		}

		<< (port:TextReader):% == {
			import from InfixExpressionParser;
			import from Partial TREE, Partial %;
			retract eval retract parse! parser port;
		}
	}
}
