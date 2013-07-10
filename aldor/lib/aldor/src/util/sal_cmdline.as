---------------------------- sal_cmdline.as ----------------------------------
--
-- This file provides utilities for command-line processing
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	PA == PrimitiveArray;
	A  == Array;
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{CommandLine}
\History{Manuel Bronstein}{7/10/98}{created}
\Usage{import from \this}
\Descr{\this~provides utilities for command-line processing.}
\begin{exports}
\alexp{arguments}: & \altype{Array} \altype{String} & command line arguments\\
\alexp{command}: & \altype{String} & command line command\\
\alexp{flag}: & \altype{Character} $\to$ \altype{List} \altype{String}
& value of a command line flag\\
\alexp{flag?}: & \altype{Character} $\to$ \altype{Boolean} &
test for a command line flag\\
\end{exports}
#endif

CommandLine: with {
	arguments: A String;
#if ALDOC
\alpage{arguments}
\Usage{\name}
\alconstant{\altype{Array} \altype{String}}
\Retval{Returns an array containing all the arguments of the command line,
the command itself is not included.}
#endif
	command: String;
#if ALDOC
\alpage{command}
\alconstant{\altype{String}}
\Retval{Returns the command, \ie the first word of the command line.}
#endif
	flag: (Ch, s:String == empty) -> List String;
	flag?: (Ch, s:String == empty) -> Boolean;
#if ALDOC
\alpage{flag}
\altarget{\name?}
\Usage{ \name~c\\ \name(c, s)\\ \name?~c\\ \name?(c, s) }
\Signatures{
\name:& \altype{Character} $\to$ \altype{List} \altype{String}\\
\name:& (\altype{Character},\altype{String})$\to$\altype{List} \altype{String}\\
\name?: & \altype{Character} $\to$ \altype{Boolean}\\
\name?: & (\altype{Character}, \altype{String}) $\to$ \altype{Boolean}\\
}
\Params{
{\em c} & \altype{Character} & flag code to look for\\
{\em s} & \altype{String} & special flag codes (optional)\\
}
\Retval{
\name(c) returns all the values of the flag c each time it is present
in the command line, an empty list otherwise.\\
\name?(c) returns \true~if the flag c is present in the command line,
\false~otherwise.\\
In both functions, the optional argument s contains a list of
flag codes which cause the rest of the argument to be skipped.
}
\begin{asex}
If the command line to the program was {\tt myprog -lsalli -l gmp -v}, then
{\tt \name?(char "a")} and {\tt \name?(char "v")} both return \true, while
{\tt \name?(char "b")} and {\tt \name?(char "a", "l")} both return \false.
In addition, {\tt \name(char "l")} returns the list {\tt [``salli'', ``gmp'']}.
\end{asex}
#endif
} == add {
	import {
		mainArgc:  Z;
		mainArgv:  Pointer;
	} from Foreign C;

	import from Z, A String;

	-- The following is for safe conversion in the debug version
	local argv:PA Pointer	== array(mainArgv, mainArgc);

	command:String		== { import from PA Pointer; string argv 0; }
	local flagsign:Ch	== { import from String; char "-"; }

	arguments:A String == {
		import from PA Pointer;
		local locala:A String := new(prev mainArgc, "");
		for locali in 0..prev prev mainArgc repeat
			locala.locali := string argv(next locali);
		locala;
	}

	flag?(c:Ch, s:String):Boolean == {
		import from List String;
		not empty? flag(c, s);
	}

	local nextarg(i:Z, n:Z):String == {
		(i := next i) = n => empty;
		arguments i;
	}

	flag(c:Ch, s:String):List String == {
		assert(c ~= flagsign);
		import from A String, Partial String, Z;
		n := #arguments;
		l:List String := empty;
		for i in 0..prev n repeat {
			if char(arguments.i) = flagsign then {
				u := flag(arguments.i, nextarg(i, n), s, c);
				if not failed? u then l := cons(retract u, l);
			}
		}
		reverse! l;
	}

	-- TEMPORARY: SHOULD HAVE SUBSTRINGS SO NO COPY IS MADE
	local (s:String) + (n:Z):String == {
		zero? n => s;
		[s.i for i in n..#s];
	}

	local flag(arg:String, t:String, s:String, c:Ch):Partial String == {
		import from Z;
		assert(char arg = flagsign);
		n := prev(#arg);
		for i in 1..prev n repeat {
			arg.i = c => return [arg + next i];
			member?(arg.i, s) => return failed;
		}
		arg.n = c => [t];
		failed;
	}
}

#if ALDORTEST
---------------------- test sal_command.as --------------------------
#include "aldor"
#include "aldortest"

-- command line must have "-v -b -Lfoo -abar"
local flags():Boolean == {
	import from String, List String, CommandLine;
	prefix := "aleoBCDFILMPQRSUW";
	boolean := "bcdfghijkmnpqrstuvwxyzAEGHJKNOTWXYZ";
	boolfound := "";
	for c in boolean | flag?(c, prefix) repeat
		boolfound := boolfound + c::String;
	preffound := "";
	for c in prefix repeat {
		l := flag(c, prefix);
		if ~empty?(l) then {
			preffound := preffound + c::String;
			for s in l repeat preffound := preffound + s;
		}
	}
	boolfound = "bv" and preffound = "abarLfoo";
}

stderr << "Testing sal__command..." << newline;
aldorTest("flags", flags);
stderr << newline;
#endif

