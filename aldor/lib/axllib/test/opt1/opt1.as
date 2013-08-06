-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint -Q3

#include "axllib"

import from SingleInteger;

myfile : String == "0000.001";

blocks (p: TextReader) : Generator List String == generate {
	l := nil;
	for s in lines p repeat {
		if #s > 0 then
			l := cons(s, l);
		else if l then {
			yield reverse! l;
			l := nil;
		}
	}
	if l then yield reverse! l;
}

write():() == {
	import from FileName, TextWriter, OutFile;

	fout := open filename myfile;
	wr := writer fout;
	
	for x in 1..10 repeat wr << "Natascia" << newline;

	close fout
}

read() : () == {
	import from StandardIO, TextReader, List String, FileName, InFile;
	fin := open filename myfile;

	if ~ open? fin then error "Cannot open myfile...";
	for i in 1.. for lines in blocks reader fin repeat {
		print << "block " << i << ":" << newline;
		print << lines << newline;
	}
	print << "done" << newline;

	close fin
}

remove():() == {
	import from OperatingSystemInterface;

	fileRemove(myfile)
}

write();
read();
remove();
