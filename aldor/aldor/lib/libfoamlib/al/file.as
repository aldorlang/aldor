-----------------------------------------------------------------------------
----
---- file.as:  Basic input and output text files.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib"

CStream ==> Pointer;
CString ==> String;
CChar   ==> SingleInteger;
CInt    ==> SingleInteger;
SI      ==> SingleInteger;

import from SingleInteger;

+++ `OutFile' is the type of output text files.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: file, stream, text, output, write, print

OutFile: with {
	open:	FileName -> %;
	open?:  % -> Boolean;
	close:  % -> ();
	
	write!: (%, Character) -> ();
		++ Write a character to a file.

	write!:	(%, String, start: SI == 1, limit: SI == 0) -> SI;
		++ `write(f,s,start,limit)' writes the portion of the `s'
		++ lying in the range `start..limit-1' to the file `f'.  
		++ A limit of `0' means to write everything up to the end `s'.
		++ The number of characters written is returned.
}
== add {
	Rep ==> CStream;
	import {
		fopen: 	(CString, CString) 	 -> CStream;
		fclose:	(CStream) 		 -> CInt;
		fputc:	(CChar, CStream)         -> CChar;
		fputs:	(CString, CStream)       -> CInt;
		fputss: (CString, CInt, CInt, CStream) -> CInt;
	} from Foreign C;

	import from Rep, String;

  	open(fn: FileName): % == per fopen(unparse fn, "w");

	open?(ouf: %): Boolean == not nil? rep ouf;

	close(ouf: %): () == fclose rep ouf;

	write!(ouf: %, c: Character): () ==
		if open? ouf then fputc(ord c, rep ouf);

	write!(ouf: %, s: String, start: SI == 1, limit: SI == 0): SI == 
		if open? ouf then
			fputss(s, start-1, limit-1, rep ouf)
		else {
			if limit = 0 then #s + 1 - start else limit - start
		}
}


+++ `InFile' is the type of input text files.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: file, stream, text, intput, read 

InFile: with {
	open:	   FileName -> %;
	open?:     % -> Boolean;
	close:	   % -> ();

	readchar!: (%, eof: Character == char 0) -> Character;
		++ `readchar!(f, EOF)' returns the next character from the
		++ file `f'.  If the end of file has been reached, then the
		++ character EOF is returned.

	readline!: (%) -> String;
		++ `readline! f' reads a line from the given file and returns
		++ a new string containing it.  The string will end with a
		++ newline character, unless the end of the file is reached
		++ without one.

	readline!: (%, String, SI, SI) -> SI;
		++`readline!(f, s, start, limit)' reads characters from
		++ the file `f' into the portion of the string `s' lying in
		++ the range `start..limit-1'.  The string `s' must have
		++ space for at least `limit' characters.  The function
		++ characters into the string until `limit' is reached,
		++ a newline is read, or the end of file is reached.
		++ The function returns the number of characters read,
		++ which will be `0' if the end of file has been reached.
}
== add {
	Rep ==> CStream;

	import {
		fopen: 	(CString, CString) 	 -> CStream;
		fclose:	(CStream) 		 -> CInt;
		fgetc:	(CStream) 		 -> CChar;
		fgets:	(CString, CInt, CStream) -> CString;
		fgetss:	(CString, CInt, CInt, CStream) -> CInt;
	} from Foreign C;

	import from Rep, SingleInteger, String, Character;


  	open(fn: FileName): % == per fopen(unparse fn, "r");

	open?(inf: %): Boolean == not nil? rep inf;

	close(inf: %): () == fclose rep inf;


	readchar!(inf: %, eof: Character == char 0): Character == {
		n := fgetc rep inf;
		if n = -1 then eof else char n
	}

	readline!(inf: %, s: String, start: SI, limit: SI): SI ==
		fgetss(s, start-1, limit-1, rep inf);

	readline!(inf: %): String == {
		buf: Array Character := empty();
		repeat {
			c := readchar! inf;
			c = char 0 => break;
			extend!(buf, c);
			c = newline => break;
		}
		s := string buf;
		dispose! buf;
		s
	}
}
