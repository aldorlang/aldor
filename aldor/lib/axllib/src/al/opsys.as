-----------------------------------------------------------------------------
----
---- opsys.as: High-level interface to operating system functions.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

CommandLine: with {
	command:   	String;
	arguments: 	Array String;
}
== add {
	import {
		mainArgc:  SingleInteger;
		mainArgv:  PrimitiveArray String;
	} from Foreign;

	import from SingleInteger, PrimitiveArray String;

	command:   String       == mainArgv 1;
	arguments: Array String == [mainArgv i for i in 2..mainArgc];
}


StandardIO: with {
	stdin:   InFile;

	stdout:  OutFile;
	stderr:	 OutFile;
	stdsink: OutFile;
}
== add {
	CStream ==> Pointer;
	import from Pointer;

	import {
		stdinFile:  () -> CStream;
		stdoutFile: () -> CStream;
		stderrFile: () -> CStream;
	} from Foreign;

	stdin:   InFile  == stdinFile()  pretend InFile;
	stdout:  OutFile == stdoutFile() pretend OutFile;
	stderr:  OutFile == stderrFile() pretend OutFile;
	stdsink: OutFile == nil          pretend OutFile;
}
