#include "axllib.as"
-- #include "aldor"
-- #include "aldorio"

-- number the lines of an Axiomxl program, producing latex

numberLines(): () == {
	import from TextReader, InFile, StandardIO;
	import from FormattedOutput, String, Character;
	input := reader stdin;
	n: SingleInteger := 1;
	for line in lines input repeat {
		myline := substring(line, 1, #line-1);
		myline := rightTrim(myline, space);
		myline := detab(myline);
		(print "{\codeline{~a}\verb&~a&}\\~n")(<< n, << myline);
--		(stdout "{\codeline{~a}\verb&~a&}\\~n")(<< n, << myline);
		n := n + 1;
	}
}

detab(s: String): String == {
	import from SingleInteger, String, Character;
--	import from MachineInteger;
	tabs := 0;
	i    := 0;
	for free i in 1..#s repeat {
		s.i = char "	" => tabs := tabs + 1;
		s.i ~= space => break;
	}
	concat(new(tabs*7+i), substring(s, i, #s));
}

numberLines();
