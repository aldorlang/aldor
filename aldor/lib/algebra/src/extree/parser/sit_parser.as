----------------------- sit_parser.as ---------------------------------
--
-- Parser Category
--
-- Copyright (c) Niklaus Mannhart 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Parser}
\History{Niklaus Mannhart}{25/03/96}{created}
\History{Manuel Bronstein}{25/09/96}{turned into a category}
\Usage{\this: Category}
\Descr{\this~is the category for parser objects.}
\begin{exports}
\asexp{eof?}: & \% $\to$ \astype{Boolean} & Check for end of input\\
\asexp{lastError}:
& \% $\to$ \astype{MachineInteger} & Code for last parsing error\\
\asexp{parse!}:
& \% $\to$ \astype{Partial} \astype{ExpressionTree} & Parse one expression\\
\end{exports}
#endif

define Parser: Category == with {
	eof?:	      % -> Boolean;
#if ALDOC
\aspage{eof?}
\Usage{\name~p}
\Signature{\%}{\astype{Boolean}}
\Params{ {\em p} & \% & A parser \\ }
\Retval{Returns \true~if the input is finished, \false~otherwise.}
#endif
	lastError:    % -> MachineInteger;
#if ALDOC
\aspage{lastError}
\Usage{\name~p}
\Signature{\%}{\astype{MachineInteger}}
\Params{ {\em p} & \% & A parser \\ }
\Retval{Returns the code for the last parsing error.}
#endif
	parse!:       % -> Partial ExpressionTree;
#if ALDOC
\aspage{parse!}
\Usage{\name~p}
\Signature{\%}{\astype{Partial} \astype{ExpressionTree}}
\Params{ {\em p} & \% & A parser \\ }
\Retval{Returns either an expression tree for the next parsed expression, or
\failed in case of error or end of input.}
\alseealso{lastError(\%)}
#endif
};

#if ALDOC
\thistype{ParserReader}
\History{Niklaus Mannhart}{25/03/96}{created}
\History{Manuel Bronstein}{25/09/96}{turned into a category}
\Usage{\this: Category}
\Descr{\this~is the category for parser objects that parse text readers.}
\begin{exports}
\asexp{parser}: & \astype{TextReader} $\to$ \% & Create a parser\\
\end{exports}
#endif

define ParserReader: Category == Parser with {
	parser:	TextReader -> %;
#if ALDOC
\aspage{parser}
\Usage{\name~r}
\Signature{\astype{TextReader}}{\%}
\Params{ {\em r} & \astype{TextReader} & The input stream to parse\\ }
\Retval{Returns a parser that takes its input on r.}
#endif
}
