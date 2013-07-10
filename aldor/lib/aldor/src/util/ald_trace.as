-------------------------------- ald_trace.as ------------------------------
--
-- This file provides trace activation routines needed to support
-- the -Wdebug option of the compiler
-- It is derived from the debug.as file, part of axllib, belonging to 
-- Aldor.org.
--
-- Compile any .as file with -Wdebug, then use traceActivate()$Trace and
-- traceStop()$Trace to get traces of functions from that file (PB 8/1995)
--
-- The traces look first for a function
--	dbgPrint: (TextWriter,%)->TextWriter 
-- from a type T, and fall back on << otherwise,
-- which lets you define a dbgPrint more appropriate for debugging than <<
-- You can also trace types that are not OutputType that way
--
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) Manuel Bronstein 2003
-- Copyright (c) INRIA 2003
-- Logiciel libaldor (c) INRIA 2003
-----------------------------------------------------------------------------

#include "aldor"

-- NB: Do not add-inherit from this package (dixit PB 8/1995)
#if ALDOC
\thistype{Trace}
\History{Peter Broadbery}{8/1995}{created for axllib}
\History{Manuel Bronstein}{31/7/2003}{adapted for libaldor and renamed Trace}
\Usage{import from \this}
\Descr{\this{} provides support for the {\tt -Wdebug} option of the
\aldor{} compiler.}
\begin{exports}
\alexp{name}: & \albuiltin{Type} $\to$ \altype{String} & Type name\\
\alexp{shortName}: & \albuiltin{Type} $\to$ \altype{String} & Type name\\
\alexp{traceActivate}: & \altype{Boolean} $\to$ () & start traces\\
\alexp{traceStop}: & () $\to$ () & stop traces\\
\end{exports}
#endif

Trace: with {
	name: Type -> String;
	shortName: Type -> String;
#if ALDOC
\alpage{name,shortName}
\altarget{name}
\altarget{shortName}
\Usage{name~T\\ shortName~T}
\Signature{\albuiltin{Type}}{\altype{String}}
\Params{ \emph{T} & \albuiltin{Type} & A type\\ }
\Retval{Returns the name of the type \emph{T}. In case \emph{T}
is the result of a function call $F(T_1,\dots,T_n)$, then name(T)
returns the full expanded name of the type, while shortName(T) returns
$F(\ast,\dots,\ast)$.}
#endif
	traceActivate: (abbr?:Boolean == false) -> ();
	traceStop:     () -> ();
#if ALDOC
\alpage{traceActivate,traceStop}
\altarget{traceActivate}
\altarget{traceStop}
\Usage{traceActivate()\\ traceActivate(abbr?)\\ traceStop()}
\Signatures{
traceActivate: & \altype{Boolean} $\to$ ()\\
traceStop: & () $\to$ ()\\
}
\Params{
\emph{abbr?} & \altype{Boolean} & A flag to control type names (optional)\\ }
\Descr{Tracing of functions from source files compiled with the {\tt -Wdebug}
option is enabled between calls to traceActivate() and traceStop().
If \emph{abbr?} is missing or \false,
then full type names are printed, otherwise
the arguments to types are not printed, see \alexp{name} for details.}
\begin{asex}
Suppose that {\tt foo.as} contains the following code:
\begin{ttyout}
#include "aldor"
#include "aldorio"

local fact(n:Integer):Integer == {
    assert(n >= 0);
    zero? n or one? n => 1;
    n * fact(n-1);
}

main():() == {
    import from Boolean, Integer, Trace;
    traceActivate();
    stdout << "5! = " << fact 5 << newline;
    traceStop();
    stdout << "20! = " << fact 20 << newline;
}

main();
\end{ttyout}
Compiling and running it with {\tt aldor -grun -Wdebug -laldor foo.as} yields
the following output:
\begin{ttyout}
{Line: 7 Entering: fact
{Line: 7 Entering: fact
{Line: 7 Entering: fact
{Line: 7 Entering: fact
{Line: 7 Entering: fact
Line: 11 Return: AldorInteger is: 1}
Line: 11 Return: AldorInteger is: 2}
Line: 11 Return: AldorInteger is: 6}
Line: 11 Return: AldorInteger is: 24}
Line: 11 Return: AldorInteger is: 120}
5! = 120
20! = 2432902008176640000
\end{ttyout}
Note that the call to {\tt fact 20} is not traced because it is done
after the call to {\tt traceStop()}.
\end{asex}
#endif
} == add {
	-- from gf_rtime.
	macro DbgTypes == 'assign,entry,exit,dominit';
	macro I == MachineInteger;
	
	-- local state
	local doItAll   := true;
	local activated := false;
	local inDebug   := false;
	local debugAbbr := true;

	name(T:Type):String	== { import from Boolean; name(T, false); }
	shortName(T:Type):String== { import from Boolean; name(T, true); }

	-- horrible hack to get the type name,
	-- at least this avoids linking with langx.as from axllib!
	macro DN == Record(tag:I, p:Pointer);
	local name(T:Type, abbr?:Boolean):String == {
		macro DV == Record(tag:I, namer:Type -> DN);
		macro Dom == Record(dispatcher:DV, domainRep:Type);
		import from DV, Dom;
		dom := T pretend Dom;
		dn := (dom.dispatcher.namer) (dom.domainRep);
		name(dn, abbr?);
	}

	local name(dn:DN, abbr?:Boolean):String == {
		import from List DN;
		dn.tag = 0 => string(dn.p);
		dn.tag = 1 => {
			ldn := (dn.p) pretend List DN;
			assert(~empty? ldn);
			s := string(first(ldn).p) + "(";
			empty?(ldn := rest ldn) => s + ")";
			t := { abbr? => "*"; name(first ldn, abbr?) };
			s := s + t;
			while ~empty?(ldn := rest ldn) repeat {
				s := s + ",";
				t := { abbr? => "*"; name(first ldn, abbr?) };
				s := s + t;
			}
			s + ")";
		}
		empty;
	}

	--
	-- activation
	--
	traceActivate(abbr?:Boolean): () == {
		free doItAll, activated, debugAbbr;
		import { fiSetDebugVar: Pointer -> () } from Foreign;
		-- only do this once.
		if (not activated) then fiSetDebugVar(doDebug pretend Pointer);
		activated := true;
		doItAll   := true;
		debugAbbr := abbr?;
	}
	traceStop(): () == { free doItAll := false; }

	-- 
	-- The real thing
	--
	-- name is a C-string, which is not the same as a libaldor debug String
	local doDebug(kind: DbgTypes, name: Pointer, line: I, 
			T: Type, v: Pointer, v2: Pointer): () == {
		free  inDebug;
		inDebug		 => return;
		not doItAll      => return;
		inDebug := true;
		if kind = assign then doAssign(name, line, T, v pretend T, 
					       v2 pretend Boolean);
		if kind = entry  then
			 doEntry(name, line, T, v pretend Boolean);
		if kind = exit then 
			doExit(name, line, T, v pretend T, v2 pretend Boolean);
		inDebug := false;
		return;
	}

	-- f is true for parameter initialisations, 
	-- false for ordinary assignments.
	-- nme is a C-string, which is not the same as a libaldor debug String
	local doAssign(nme:Pointer, line:I, T:Type, v:T, f:Boolean): () == {
		import from TextWriter, Character, String;
		stderr << "Line " << line << ": " << string nme << " : ";
		stderr << name(T, debugAbbr) << " := ";
		printValue(T, v);
		stderr << newline;
	}

	-- v indicates if the exporter was valid
	-- T is the exporter, if v is true. 
	-- nme is a C-string, which is not the same as a libaldor debug String
	local doEntry(nme:Pointer, line: I, T: Type, v: Boolean):() == {
		import from TextWriter, Character, String;
		stderr << "{Line: " << line << " Entering: " << string nme;
		if v then stderr << " from: " << name(T, debugAbbr);
		stderr << newline;
	}

	-- f false indicates that T and v are not bone fide objects
	-- doExit1 is necessary as we don't know if T is a type at this point.
	-- nme is a C-string, which is not the same as a libaldor debug String
	local doExit(nme:Pointer,line: I,T:Type,v:T, f:Boolean): () == {
		import from TextWriter, Character, String;
		not f => stderr << "Line: " << line << "}" << newline;
		doExit1(nme, line, T, v);
	}

	-- nme is a C-string, which is not the same as a libaldor debug String
	local doExit1(nme:Pointer, line:I, T: Type, v: T): () == {
		import from TextWriter, Character, String;
		stderr << "Line: " << line << " Return: " ;
		stderr << name(T, debugAbbr) << " is: ";
		printValue(T, v);
		stderr << "}" << newline;
		
	}

	-- finding an appropriate print routine
	local printValue(T: Type, v: T): () == {
		import from TextWriter, String;
		T has (with { dbgPrint: ((TextWriter, %) ->TextWriter) }) =>
							dbgPrint(stderr, v);
		T has OutputType => stderr << v;
		stderr << "--unprintable--";
	}
		
	--
	-- type-matching predicates
	-- 
	local debugType?(T: Type): Boolean == { return true }
}

