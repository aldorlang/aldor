-----------------------------------------------------------------------------
----
---- textwrit.as: TextWriters -- generic text output.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ `TextWriter' is a type of writable text streams.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: file, stream, text, output, write, print

extend TextWriter: with {
	writer:  FileName -> %;
	writer:  OutFile  -> %;
	writer:  Array Character -> %;

  	print:   %;
  	error:   %;
  	sink:    () -> %;
}
== add {
	import from StandardIO;

	Char ==> Character;
	SI   ==> SingleInteger;

	----  functions from which to make writers ----
	local aPutc(oarr: Array Char)(c: Char): () ==
		extend!(oarr, c);

	-- Note, this handles the case where 0 => do upto EOS.
	local aPuts(oarr: Array Char)(s: String, start: SI, limit: SI): SI == {
		start >= limit and limit ~= 0 => 0;
		i := start;
		while i ~= limit and not end?(s, i) repeat {
			extend!(oarr, s.i);
			i := i + 1
		}
		i - start
	}

	local fPutc(ofile: OutFile)(c: Char):() ==
		write!(ofile, c);

	local fPuts(ofile: OutFile)(s: String, start: SI, limit: SI): SI ==
		write!(ofile, s, start, limit);

	---- Interesting exported operations ----
	writer(fname: FileName): %   == writer(open(fname)$OutFile);
  	writer(ofile: OutFile): %    == writer(fPutc ofile, fPuts ofile);
  	writer(oarr:  Array Char): % == writer(aPutc oarr, aPuts oarr);

	-- Create sinks freshly to allow character counting.
  	sink(): % == writer(fPutc stdsink,fPuts stdsink);
  	print:  % == writer(fPutc stdout, fPuts stdout);
  	error:  % == writer(fPutc stderr, fPuts stderr);
}
