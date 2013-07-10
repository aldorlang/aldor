------------------------------ sit_maple.as ------------------------------
--
-- Basic Maple - Aldor Interface
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	TREE	== ExpressionTree;
	PTREE	== Partial TREE;
}

#if ALDOC
\thistype{Maple}
\History{Manuel Bronstein}{15/04/98}{created}
\Usage{import from \this}
\Descr{\this~provides utilities that allow its clients to batch \maple
sessions and recover the output.}
\begin{exports}
\alexp{input}:
& \% $\to$ \altype{TextWriter} & Input stream of the maple session\\
\alexp{maple}: & () $\to$ \% & Create a maple session\\
\alexp{run}:
& \% $\to$ \altype{Partial} \altype{ExpressionTree} & Run a maple session\\
\end{exports}
#endif

Maple:with {
	input: % -> TextWriter;
#if ALDOC
\alpage{input}
\Usage{\name~m}
\Signature{\%}{\altype{TextWriter}}
\Params{m & \% & A maple session\\}
\Retval{Returns the input stream for the maple session.}
\Remarks{Use that stream to send sequences of valid maple commands, making
sure that all the commands are terminated with `:` in order to avoid any
printing from \maple. Do not use any of \maple's printing functions. Note
that the system maple is not started until 'run' is called.}
#endif
	maple: (debug?:Boolean == false) -> %;
#if ALDOC
\alpage{maple}
\Usage{\name()}
\Signature{()}{\%}
\Descr{Creates a maple session, by associating unique communications channels
to and from that session.}
\Remarks{The maple input and output files used for communication are located
in the /tmp directory and are deleted after the session is run, unless
the call {\tt maple(true)} is used, in which case they remain and can
be inspected. Note that the system maple is not started until 'run' is called.}
#endif
	run: % -> PTREE;
#if ALDOC
\alpage{run}
\Usage{\name~m}
\Signature{\%}{\altype{Partial} \altype{ExpressionTree}}
\Params{m & \% & A maple session\\}
\Descr{Launches the system maple and executes all the commands that were
sent to the input stream of the session. Returns the expression tree
corresponding to the value returned by the last maple command executed.}
\begin{alex}
This examples shows how to call \maple to compute the integral
of the $\sth{5}$ Legendre polynomial:
\begin{ttyout}
import from Integer, Maple, ExpressionTree, Partial ExpressionTree;

n := 5;
-- create a session (maple is not launched but a unique link is created)
mapl := maple();

-- send the maple code to compute the integral of the n-th legendre poly
-- note that all the maple commands are terminated with ":"
-- so that they do not generate any output
-- here again, nothing happens, the commands are only stored
input(mapl) << "with(orthopoly): p := P(" << n << ", x): int(p, x):";

-- now launch maple and recover the result of the last command ("int")
tree := run mapl;

failed? tree => error "Unable to parse Maple's output";
retract tree;
\end{ttyout}
Running the above code produces the following expression tree:
\begin{ttyout}
(+ (- (* (/ 21 16) (^ x 6)) (* (/ 35 16) (^ x 4))) (* (/ 15 16) (^ x 2)))
\end{ttyout}
\end{alex}
#endif
} == add {
	Rep == Record(nam:String, inp:File, del?:Boolean);
	import from Rep;

	local name(m:%):String		== rep(m).nam;
	local file(m:%):File		== rep(m).inp;
	local delete?(m:%):Boolean	== rep(m).del?;
	local outputName(m:%):String	== name(m) + ".out";
	input(m:%):TextWriter	== { import from File; file(m)::TextWriter; }

	maple(debug?:Boolean):% == {
		import from Character, File, MachineInteger;
		TRACE("maple::maple(): debug? = ", debug?);
		s := uniqueName "/tmp/sumitMaple";
		TRACE("maple::maple(): s = ", s);
		m := per [s, open(s,fileWrite + fileText), ~debug?];
		-- Maple hack: _sumitMapleVersion is 1 for V.4, 0 for V.5
		input(m) << "1:1:_sumitMapleVersion := length(_"_"):" <<newline;
		m;
	}

	run(m:%):PTREE == {
		import from Character, String, File, MachineInteger;
		import from TextReader, InfixExpressionParser;
		import { system: Pointer -> () } from Foreign C;
		TRACE("maple::run: ", name m);
		-- Maple hack: _sumitMapleVersion is 1 for V.4, 0 for V.5
		input(m) << "if _sumitMapleVersion = 0 then ";
		input(m) << "printf(`%a\n`, eval(`%`)) else ";
		input(m) << "printf(`%a\n`, eval(`_"`)) fi: done:" << newline;
		close! file m;
		sout := outputName m;
		cmd := "maple -q < " + name(m) + " > " + sout;
		TRACE("maple::run: cmd = ", cmd);
		system pointer cmd;
		out := open(sout, fileRead + fileText);
		ans := parse! parser(out::TextReader);
		if delete? m then {
			remove name m;
			remove sout;
		}
		ans;
	}
}
