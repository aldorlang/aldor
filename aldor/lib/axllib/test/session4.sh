aldor -gloop -Mno-release <<ThatsAll

#int timing off
#include "axllib"

import from TextReader, FileName, SingleInteger;

myfile : String == "natascia"
mylove : String  == "Natascia 4ever"

write():() ==
	import from OutFile, TextWriter

	fout := open filename myfile
	~open? fout => error "cannot open myfile..."
	tout := writer fout
	for i in 1..10 repeat tout << mylove
	close fout
--

write()

rd := reader filename myfile;

import from List String;

ls := [l for l in lines rd];

foo():() == (
	import from Character
	for l in ls repeat
		for c in l repeat
			print << upper c
)

print << "foo()" << newline
foo()

lt := copy ls

import from List List Character;

print << "ln := [[upper c for c in s] for s in ls]" << newline
ln := [[upper c for c in s] for s in ls]

import from OperatingSystemInterface

fileRemove myfile

#quit

ThatsAll
