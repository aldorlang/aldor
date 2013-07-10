------------------------------- alg_op.as ----------------------------------
-- Copyright Swiss Federal Polytechnic Institute Zurich, 1994
--
-- This type must be included inside "sit_extree.as" to be compiled,
-- but is in a separate file for documentation purposes.

#if ALDOC
\thistype{ExpressionTreeOperator}
\History{Manuel Bronstein}{21/11/94}{created}
\History{Manuel Bronstein}{23/6/2003}{added infix}
\Usage{\this: Category}
\Descr{\this~is the category of operators for expression trees.}
\begin{exports}
\alexp{aldor}:
& (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to \asharp code\\
\alexp{arity}: & \altype{MachineInteger} & Number of arguments\\
\alexp{axiom}:
& (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to Axiom code\\
\alexp{C}: & (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to C code\\
\alexp{fortran}:
& (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to FORTRAN code\\
\alexp{infix}:
& (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to one-dim infix output\\
\alexp{lisp}:
& (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to Lisp code\\
\alexp{maple}:
& (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to Maple code\\
\alexp{name}: &	\altype{Symbol} & Operator name\\
\alexp{tex}: & (TEXT, \altype{List} TREE) $\to$ TEXT & Conversion to \LaTeX\\
\alexp{texParen?}: & \altype{MachineInteger} $\to$ \altype{Boolean} &
Check whether to parenthetize\\
\alexp{uniqueId}: & \altype{MachineInteger} & A unique key per operator\\
\end{exports}
\begin{aswhere}
TEXT &==& \altype{TextWriter}\\
TREE &==& \altype{ExpressionTree}\\
\end{aswhere}
#endif

macro {
	Z	== MachineInteger;
	OP	== ExpressionTreeOperator;
	TEXT	== TextWriter;
	TREE	== ExpressionTree;
}

define ExpressionTreeOperator: Category == with {
	aldor:		(TEXT, List TREE) -> TEXT;
	axiom:		(TEXT, List TREE) -> TEXT;
	C:		(TEXT, List TREE) -> TEXT;
	fortran:	(TEXT, List TREE) -> TEXT;
	infix:		(TEXT, List TREE) -> TEXT;
	lisp:		(TEXT, List TREE) -> TEXT;
	maple:		(TEXT, List TREE) -> TEXT;
	tex:		(TEXT, List TREE) -> TEXT;
#if ALDOC
\alpage{aldor,axiom,C,fortran,infix,lisp,maple,tex}
\altarget{aldor}
\altarget{axiom}
\altarget{C}
\altarget{fortran}
\altarget{infix}
\altarget{lisp}
\altarget{maple}
\altarget{tex}
\Usage{{\em format}(p, [$t_1,\dots,t_n$])}
\Signature{(\altype{TextWriter}, \altype{List} \altype{ExpressionTree})}{\altype{TextWriter}}
\Params{
{\em p} & \altype{TextWriter} & The port to write to\\
{\em $t_i$} & \altype{ExpressionTree} & The arguments of the operator\\
}
\Descr{Writes to $p$ the expression corresponding to this operator applied to
the arguments $(t_1,\dots,t_n)$ in the requested format.}
#endif
	arity:		Z;
#if ALDOC
\alpage{arity}
\Usage{\name}
\Signature{}{\altype{MachineInteger}}
\Retval{Returns $-1$ if this operator can be applied to any number of arguments,
$n \ge 0$ if this operator can be applied to exactly $n$ arguments.}
#endif
	name:		Symbol;
#if ALDOC
\alpage{name}
\Usage{\name}
\Signature{}{\altype{Symbol}}
\Retval{Returns the name of this operator.}
#endif
	texParen?:	Z -> Boolean;
#if ALDOC
\alpage{texParen?}
\Usage{\name~prec}
\Signature{\altype{MachineInteger}}{\altype{Boolean}}
\Params{ {\em prec} & \altype{MachineInteger} & An operator precendence.\\ }
\Retval{Returns \true~if an expression tree with this operator as root should
be parenthetized when appearing as argument of an operator of precedence
{\em prec}, \false~otherwise.}
#endif
	uniqueId:	Z;
#if ALDOC
\alpage{uniqueId}
\Usage{\name}
\Signature{}{\altype{MachineInteger}}
\Retval{Returns a integer key which is associated to this operator only.
This is used for testing whether two operators are equal.}
#endif
	default {
		infix(p:TEXT, l:List TREE):TEXT == {
			import from TREE, String, Symbol;
			p := p << name << "(";
			for arg in l repeat p := infix(p << " ", arg);
			p << ")";
		}

		lisp(p:TEXT, l:List TREE):TEXT == {
			import from TREE, String, Symbol;
			p := p << "(" << name;
			for arg in l repeat p := lisp(p << " ", arg);
			p << ")";
		}
	}
}

