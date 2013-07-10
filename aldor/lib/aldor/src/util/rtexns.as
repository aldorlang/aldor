-------------------------------- rtexns.as ---------------------------------
--
-- This file provides aldorRuntimeException and aldorUnhandledException
-- which are currently required by libfoam, but not provided by it.
-- It is derived from the rtexns.as file belonging to Aldor.org.
--
-- The way libfoam is compiled, it must have the name rtexns.as
--
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

export {
	aldorRuntimeException: Pointer -> ();
	aldorUnhandledException: Pointer -> ();
} to Foreign Builtin;

-- should print and raise exception...
-- error is a C-string, which is not the same as a salli debug String
aldorRuntimeException(error:Pointer):() == runtimeError(error)$ExceptionPackage;

aldorUnhandledException(p:Pointer):() == unhandledHandler(p)$ExceptionPackage;

define RuntimeException: Category == with {
	name: () -> String;
	printError: TextWriter -> ();
}

-- s is a C-string, which is not the same as a salli debug String
RuntimeError(s: Pointer): RuntimeException == add {
	name(): String == string s;
	printError(o: TextWriter): () == { import from String; o << name(); }
}

ExceptionPackage: with {
	unhandledHandler: Pointer -> ();
	installHandler: (Pointer -> ()) -> ();
	defaultHandler: Pointer -> ();
	runtimeError: Pointer -> ();
} == add {
	defaultHandler(p: Pointer): () == {
		import from Trace, String, TextWriter, WriterManipulator;
		stderr << "Unhandled Exception: ";
		stderr << name(p pretend Type) << endnl;
		-- should have a way of adding extra handlers
		printRTInfoIfAny(p pretend with);
	}

	printRTInfoIfAny(t: with): () == {
		import from TextWriter, WriterManipulator;
		if t has RuntimeException then {
			printError(stderr)$t;
			stderr << endnl;
		}
	}

	theHandler: Pointer -> () := defaultHandler;

	unhandledHandler(p: Pointer): () == theHandler(p);

	installHandler(f: Pointer -> ()): () == {
		free theHandler := f;
	}
	
	-- s is a C-string, which is not the same as a salli debug String
	runtimeError(s: Pointer): () == throw RuntimeError(s);
}
