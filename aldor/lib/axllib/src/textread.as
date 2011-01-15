-----------------------------------------------------------------------------
----
---- textread.as:  TextReader -- generic text input sources.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

import from SingleInteger;
import from InFile;

SI       ==> SingleInteger;
RdCharFn ==> (eof: Character == char 0) -> Character;
RdLineFn ==> (String, SI, SI) -> SI;

TextReader: with {
	reader:    FileName -> %;
	reader:    InFile -> %;

	reader:	   (RdCharFn, RdLineFn) -> %;

	readchar!: (%, eof: Character == char 0) -> Character;
		++ `readchar!(t, EOF)' returns the next character from the
		++ TextReader.  If the end has been reached, then the
		++ character EOF is returned.

	readline!: (%) -> String;
		++ `readline! t' reads a line from TextReader and returns
		++ a new string containing it.  The string will end with a
		++ newline character, unless the end is reached without one.

	readline!: (%, String, SI, SI) -> SI;
		++`readline!(t, s, start, limit)' reads characters from
		++ the TextReader `t' into the portion of the string `s'
		++ lying in the range `start..limit-1'.  The string `s' must
		++ have space for at least `limit' characters.  The function
		++ characters into the string until `limit' is reached,
		++ a newline is read, or the end of file is reached.
		++ The function returns the number of characters read,
		++ which will be `0' if the end of file has been reached.

	generator: % -> Generator Character;

	lines:     % -> Generator String;
}
== add {

	OpRets ==> (RdCharFn, RdLineFn);
	Rep    ==> () -> OpRets;

	local fGetc(inf:InFile): RdCharFn ==
		(eof: Character == char 0): Character +->
			readchar!(inf,eof);

	local fGetl(inf:InFile): RdLineFn ==
		(s: String, start: SI,limit: SI): SI +->
			readline!(inf, s, start, limit);

	-- Constructors
	reader(fn: FileName): % == reader open fn;

	reader(inf: InFile): % == reader(fGetc inf, fGetl inf);

	reader(fc: RdCharFn, fl: RdLineFn): % == per((): OpRets +-> (fc, fl));


	-- Reading operations
	readchar!(rdr: %, eof: Character == char 0): Character ==
		{ (fc, fl) := rep(rdr)(); fc(eof) }

	readline!(rdr: %, s: String, start: SI, limit: SI): SI ==
		{ (fc, fl) := rep(rdr)(); fl(s, start, limit) }

        readline!(rdr: %): String == {
                buf: Array Character := empty();
                repeat {
                        c := readchar! rdr;
                        c = char 0  => break;
                        extend!(buf, c);
			c = newline => break;
                }
                s := string buf;
                dispose! buf;
                s
        }

	-- Generators
	generator(rdr: %): Generator Character == generate {
		import from Character;
		(fc, fl) := rep(rdr)();
		repeat {
			c := fc char 0;
			if c = char 0 then break;
			yield c;
		}
	}

	lines(rdr: %): Generator String == generate {
		buf: Array Character := empty();
		import from String;
		for c in rdr repeat {
			extend!(buf, c);
			if c = newline then {
				yield string buf;
				empty! buf
			}
		}
		if #buf > 0 then
			yield string buf;
		dispose! buf
	}
}
