#include "testall.h"
#include "testlib.h"
#include "fname.h"

local String fnameAbsolute(String dir, String fname);
local String fnameToRoot(String cwd, String file);

void fnameTest()
{
	String f;

	f = fnameAbsolute("/opt/home/pab", "Work/aldor/opt/bin/aldor");
	testStringEqual("Test1", "/opt/home/pab/Work/aldor/opt/bin/aldor", f);

	f = fnameAbsolute("/opt/home/pab", "./Work/aldor/opt/bin/aldor");
	testStringEqual("Test2", "/opt/home/pab/./Work/aldor/opt/bin/aldor", f);

	f = fnameAbsolute("/opt/home/pab/foo", "../Work/aldor/opt/bin/aldor");
	testStringEqual("Test3", "/opt/home/pab/foo/../Work/aldor/opt/bin/aldor", f);

	f = fnameAbsolute("/opt/home/pab", "/usr/lib/aldor/bin/aldor");
	testStringEqual("Test4", "/usr/lib/aldor/bin/aldor", f);

	f = fnameAbsolute("/opt/home/pab", "/usr/lib/aldor/bin/aldor");
	testStringEqual("Test5", "/usr/lib/aldor/bin/aldor", f);

	f = fnameAbsolute(".", "../bin/aldor");
	testStringEqual("Test6", "./../bin/aldor", f);
	
	f = fnameToRoot(".", "../../opt/bin/aldor");
	testStringEqual("Test7", "./../../opt/bin/..", f);

	f = fnameToRoot(".", "/usr/bin/aldor");
	testStringEqual("Test8", "/usr/bin/..", f);
	
}

local String
fnameToRoot(String cwd, String file)
{
	FileName fname = fnameParseStaticWithin(file, cwd);
	String binDir = fnameDir(fname);
	FileName rootDir = fnameNew(binDir, "..", "");

	String root = fnameUnparse(rootDir);

	return root;
}


local String
fnameAbsolute(String dir, String fname)
{
	FileName f = fnameParseStaticWithin(fname, dir);
	return fnameUnparse(f);
}
