---------------------- sit_interp.as -----------------------------
--
-- Expression Tree Interpreters
--
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE	== ExpressionTree;
	LEAF	== ExpressionTreeLeaf;
	SYMBTAB	== SymbolTable;
}

#if ALDOC
\thistype{Evaluator}
\History{Manuel Bronstein}{10/01/96}{created}
\Usage{\this~R: Category}
\Params{ R & \altype{PartialRing} & Resulting type of the evaluation\\ }
\Descr{\this~R is a category of interpreters, \ie~types which convert
expression trees into elements of R whenever possible.}
\begin{exports}
\alexp{eval!}: &
(TEXT, TREE, SYM R) $\to \overline R$ & Interpret an arbitrary tree\\
\alexp{evalLeaf!}: & (LEAF, SYM R) $\to \overline R$ & Interpret a leaf\\
\alexp{evalOp!}:
& (TEXT, TREE, SYM R) $\to \overline R$ & Interpret $op(args)$\\
\alexp{evalPrefix!}:
& (\altype{String}, Z, \altype{List} R, SYM R) $\to \overline R$ &
Interpret $op(args)$, $op$ is prefix\\
\end{exports}
\begin{aswhere}
Z &==& \altype{MachineInteger}\\
LEAF &==& \altype{ExpressionTreeLeaf}\\
$\overline R$ &==& \altype{Partial} R\\
SYM &==& \altype{SymbolTable}\\
TEXT &==& \altype{TextWriter}\\
TREE &==& \altype{ExpressionTree}\\
\end{aswhere}
#endif

define Evaluator(R:PartialRing): Category == with {
	eval!:		(TEXT, TREE, SYMBTAB R) -> Partial R;
	evalLeaf!:	(LEAF, SYMBTAB R) -> Partial R;
	evalOp!:	(TEXT, TREE, SYMBTAB R) -> Partial R;
	evalPrefix!:	(Symbol, Z, List R, SYMBTAB R) -> Partial R;
	default {
		local sfor:Symbol	== { import from String; -"for" }
		local sC:Symbol		== { import from String; -"C" }
		local saldor:Symbol	== { import from String; -"aldor" }
		local saxiom:Symbol	== { import from String; -"axiom" }
		local smaple:Symbol	== { import from String; -"maple" }
		local sfortran:Symbol	== { import from String; -"fortran" }
		local slisp:Symbol	== { import from String; -"lisp" }
		local stex:Symbol	== { import from String; -"tex" }

		evalPrefix!(s:Symbol, n:Z, l:List R, t:SYMBTAB R):Partial R == {
			failed;
		}

		evalLeaf!(t:LEAF, tab:SYMBTAB R):Partial R == {
			import from R;
			symbol? t => tab symbol t;
			integer? t => [integer(t)::R];
			failed;
		}

		eval!(p:TEXT, t:TREE, tab:SYMBTAB R):Partial R == {
			TRACE("evaluator::eval! ", t);
			leaf? t => evalLeaf!(leaf t, tab);
			evalOp!(p, t, tab);
		}

		local evalArgs!(p:TEXT, l:List TREE, tab:SYMBTAB R):
			Partial List R == {
			TRACE("evaluator::evalArgs! ", l);
			import from Partial R, List R;
			ans:List(R) := empty;
			for arg in l repeat {
				failed?(u := eval!(p,arg,tab)) => return failed;
				ans := cons(retract u, ans);
			}
			[reverse! ans];
		}

		-- for(i := a, i <= b, i := i + 1, expr)
		-- TEMPORARILY: for(i, a, lesseq(i, b), i + 1, expr)
		local evalFor!(p:TEXT, n:Z, l:List TREE, tab:SYMBTAB R):
			Partial R == {
			import from Boolean, TREE;
			TRACE("evaluator::evalFor! ", l);
			n ~= 5 or ~leaf?(t := first l) => failed;
			evalFor!(p, t, first rest l, first rest rest l,
					first rest rest rest l,
					first rest rest rest rest l, tab);
		}

		local evalFor!(p:TEXT, var:TREE, init:TREE, end:TREE, incr:TREE,
					code:TREE, tab:SYMBTAB R):Partial R == {
			import from LEAF, List TREE, Z, R;
			TRACE("evaluator::evalFor! ", init);
			-- uniqueId$operator(init) ~= UID__ASSIGN
				-- or ~leaf?(first(args := arguments init))
				-- or ~symbol?(leaf first args) => failed;
			-- lv := symbol leaf first args;
			~leaf?(var) or ~symbol?(leaf var) => failed;
			lv := symbol leaf var;
			oldval := tab.lv;
			-- failed? evalAssign!(p, #args, args, tab) => failed;
			failed?(u := eval!(p, init, tab)) => failed;
			tab.lv := retract u;
			ans:Partial R := failed;
			cont?:Boolean := true;
			while cont? repeat {
				if failed?(u := eval!(p, end, tab)) then
					cont? := false;
				else if retract u = 0 then {
					ans := u;
					cont? := false;
				}
				else if failed? eval!(p, code, tab)
					or failed?(u := eval!(p,incr,tab)) then
						cont? := false;
				-- TEMPORARY: REMOVE WHEN i := i + 1 IS ALLOWED
				else tab.lv := retract u;
			}
			if ~failed?(oldval) then tab.lv := retract oldval;
			ans;
		}

		local evalAssign!(p:TEXT, n:Z, l:List TREE, tab:SYMBTAB R):
			Partial R == {
			import from LEAF, TREE;
			n=2 and leaf? first l and symbol?(lf:=leaf first l) => {
				failed?(u:=eval!(p,first rest l,tab)) => failed;
				tab.(symbol lf) := retract u;
				u;
			}
			failed;
		}

		local evalLispList!(p:TEXT, n:Z, l:List TREE, tab:SYMBTAB R):
			Partial R == {
			import from LEAF, TREE;
			empty? l => failed;
			leaf? first l and symbol?(lf := leaf first l) =>
				evalLisp!(p, lf, prev n, rest l, tab);
			evalOp!(p, ExpressionTreeList l, tab);
		}

		local evalLisp!(p:TEXT, lf:LEAF, n:Z,l:List TREE,tab:SYMBTAB R):
			Partial R == {
			import from Character, String, R, List R,Partial List R;
			TRACE("Evaluator::evalLisp!:lf = ", lf);
			TRACE("Evaluator::evalLisp!:n = ", n);
			us := map!(upper)(name(s := symbol lf));
			TRACE("Evaluator::evalLisp!:us = ", us);
			n = 2 and us = "SETQ" => evalAssign!(p, n, l, tab);
			failed?(pl := evalArgs!(p, l, tab)) => failed;
			args := retract pl;
			n = 1 and evalPrint?(p, s, first args) => [first args];
			us = "PLUS" or us = "ADD" or us = "SUM" => sum args;
			us = "TIMES" or us = "PRODUCT" => product args;
			n = 2 and us = "EXPT" => first args ^ first rest args;
			n = 2 and us="QUOTIENT" => first args / first rest args;
			n = 2 and (us = "DIFFERENCE" or us = "DIFF") =>
						first args - first rest args;
			n = 1 and us = "MINUS" => - first args;
			n = 1 and us = "ADD1" => first(args) + 1;
			n = 1 and us = "SUB1" => first(args) - 1;
			failed?(u := evalPrefix!(s, n, args, tab)) => {
				failed?(v := evalLeaf!(lf, tab)) => failed;
				evalPrefix!(name$ExpressionTreeList, next n,
						cons(retract v, args), tab);
			}
			u;
		}

		evalOp!(p:TEXT, t:TREE, tab:SYMBTAB R):Partial R == {
			import from Boolean, Symbol;
			TRACE("evaluator::evalOp! ", t);
			assert(~leaf? t);
			import from LEAF, List TREE, Z;
			import from R, List R, Partial List R;
			op := uniqueId$operator(t);
			TRACE("evaluator::evalOp!: operator id = ", op);
			nargs := #(args := arguments t);
			TRACE("evaluator::evalOp!: # of arguments = ", nargs);
			op = UID__ASSIGN => evalAssign!(p, nargs, args, tab);
			op = UID__LLIST => evalLispList!(p, nargs, args, tab);
			s := name$operator(t);
			op = UID__PREFIX and s = sfor =>
				evalFor!(p, nargs, args, tab);
			failed?(pl := evalArgs!(p, args, tab)) => failed;
			arg1 := first(l := retract pl);
			arg2 := { nargs < 2 => arg1; first rest l };
			TRACE("evaluator::evalOp!: arguments = ", l);
			op = UID__PREFIX => {
				nargs = 1 and evalPrint?(p, s, arg1) => [arg1];
				nargs = 2 and
				  ~failed?(u := evalBoolean?(s,arg1,arg2)) => u;
				evalPrefix!(s, nargs, l, tab);
			}
			op = UID__LIST => list l;
			op = UID__PLUS => sum l;
			op = UID__TIMES => product l;
			op = UID__MINUS => {
				nargs = 1 => - arg1;
				nargs = 2 => arg1 - arg2;
				failed;
			}
			op = UID__DIVIDE => {
				nargs = 2 => arg1 / arg2;
				failed;
			}
			op = UID__EXPT => {
				nargs = 2 => arg1 ^ arg2;
				failed;
			}
			op = UID__LESSTHAN => {
				nargs = 2 => arg1 < arg2;
				failed;
			}
			op = UID__LESSEQUAL => {
				nargs = 2 => arg1 <= arg2;
				failed;
			}
			op = UID__MORETHAN => {
				nargs = 2 => arg1 > arg2;
				failed;
			}
			op = UID__MOREEQUAL => {
				nargs = 2 => arg1 >= arg2;
				failed;
			}
			failed;
		}

		local evalPrint(p:TEXT, f:(TEXT,TREE)->TEXT, semi?:Boolean,x:R):
			Boolean == {
				import from Character,String,WriterManipulator;
				f(p, extree x);
				if semi? then p << ";";
				p << newline << flush;
				true;
		}

		local evalPrint?(p:TEXT, name:Symbol, x:R):Boolean == {
			import from TREE;
			TRACE("evaluator::evalPrint?: name = ", name);
			TRACE("evaluator::evalPrint?: x = ", x);
			name = sC => evalPrint(p, C, true, x);
			name = saldor => evalPrint(p, aldor, true, x);
			name = saxiom => evalPrint(p, axiom, false, x);
			name = smaple => evalPrint(p, maple, true, x);
			name = sfortran => evalPrint(p, fortran, false, x);
			name = slisp => evalPrint(p, lisp, false, x);
			name = stex => evalPrint(p, tex, false, x);
			TRACE("evaluator::evalPrint?: ", "returning false");
			false;
		}

		-- TEMPORARY: TO REMOVE WHEN  < > <= >= are parsed
		local sless:Symbol	== { import from String; -"less" }
		local slesseq:Symbol	== { import from String; -"lesseq" }
		local sgreater:Symbol	== { import from String; -"greater" }
		local sgreatereq:Symbol	== { import from String; -"greatereq" }
		local evalBoolean?(name:Symbol, x:R, y:R):Partial R == {
			name = sless      => x < y;
			name = slesseq    => x <= y;
			name = sgreater   => x > y;
			name = sgreatereq => x >= y;
			failed;
		}
	}
}
