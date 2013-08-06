-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint -Q3
--> testrun  -l axllib

#include "axllib"

-- test for opsys0.as

nl ==> newline;

--#library libop "opsys0.ao";

-- import from libop;

import from Character;
import from String;
import from SingleInteger;
import from Pointer;
import from OperatingSystemInterface;

foo():() == {
#if TestOsSpecific
	run("echo Natascia > oooo");
	result := run("cat oooo");  -- display this file
	if ~fileExists? "oooo" then error "fileExist? error";
	fileRemove("oooo");
	if fileExists? "oooo" then error "fileRemove error";

	print << nl << "The result is :" << result << nl;
#endif

	putenv("FOO=natascia");
 	s := getenv("FOO");	

	print << "FOO is: " << s << nl;

	if (s ~= "natascia") then error("Wrong put/getenv");

#if TestOsSpecific
	print << "exec files: " << execFileType << " obj: " << objectFileType << nl;
#endif

	s1 := cpuTime();

	print << "io: " << ioRdMode << space << ioAubMode << space << ioRbMode << nl;

	print << "Current dir: " << curDirName() << nl;

	s2 := cpuTime();

	if (s1 > s2) then 
		print << "Never read this !!!!!" << newline;
	else
		print << "Timer seems Ok..." << newline;

	-- print << "Include path: " << includePath() << nl;
	-- print << "Library path: " << libraryPath() << nl;
	-- print << "Execute path: " << executePath() << nl;
	print << "exist? dir '/tmp'" << dirExists?("/tmp") << nl;
	dt := date();

	StringArr ==> PrimitiveArray String; 	-- In C: String *

	fnparts : StringArr := new(fnameNParts, "");
	fnparts.1 := "home";
	fnparts.2 := "axiomxl";
	fnparts.3 := "foo";

	flen := fnameUnparseSize(fnparts, true);

	s := fnameUnparse(empty flen, fnparts, true);

	print << s << newline;

}

foo()
