-----------------------------------------------------------------------------
----
---- fmtout.as: Formatted output
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

Format	==> String;
SI 	==> SingleInteger;
OUT 	==> TextWriter;

+++`FormattedOutput' provides functions which format their arguments
+++ according to a control string.  The control characters are:
+++
+++ ~t           Put a tab.
+++ ~n           Put a newline.
+++ ~~           Put a `~' character.
+++ ~{i}a        Put the i-th argument.
+++ ~a           Put the next argument.
+++ ~1,...,~9    Shorthand for `~{1}a',...,`~{9}a'

extend FormattedOutput: with {
	print: (Format, on: OUT == print)  -> Tuple(OUT->OUT) -> OUT;
	count: (Format, on: OUT == sink()) -> Tuple(OUT->OUT) -> SI;
	string:(Format)                    -> Tuple(OUT->OUT) -> String;
}
== add {

	import from SI, Character, TextWriter;

	default fmt:     Format;
	default i0,narg: SI;
	default on:      TextWriter;
	default items:   Tuple(OUT->OUT);

	CTL  ==> char "~";
	DONT ==> -1;
	CANT ==> -2;
	
	fake: TextWriter := error;

	print(fmt, on: TextWriter == print): Tuple(OUT->OUT) -> OUT == {
		i0 := putSpan(fmt, 1, on);
		(items: Tuple(OUT->OUT)): OUT  +->
			{ putFormat(fmt, i0, on, items); on }
	}
	count(fmt, on: TextWriter == sink()): Tuple(OUT->OUT) -> SI == {
		n0 := #on;
		i0 := putSpan(fmt, 1, on);
		(items: Tuple(OUT->OUT)): SI  +-> {
			putFormat(fmt, i0, on, items);
			#on - n0
		}
	}
	string(fmt: Format): Tuple(OUT->OUT) -> String == {
		a: Array Character := empty();
		on := writer a;

		i0 := putSpan(fmt, 1, on);
		(items: Tuple(OUT->OUT)): String  +-> {
			putFormat(fmt, i0, on, items);
			s := string a;
			dispose! a;
			s
		}
	}

	--`putSpan' puts characters from the format string onto the TextWriter.
	-- The first character put is `fmt.i0', and output proceeds until a
	-- format control requiring an argument item, or the end of the format
	-- string is encountered.  The result is the index of the first
	-- character not output, and the modified writer.

	local putSpan(fmt, i0, on): SI == {
		if i0 < 1 then return 1;

		while not end?(fmt, i0) repeat {
			-- Handle those formats which do not require an item.
			if fmt.i0 = CTL then {
				(i0,narg) := doControl(fmt,i0, on, DONT, ());
				if narg = CANT then break;
				if end?(fmt, i0) then break;
			}
			-- Find end of substring.
			i := i0;
			while not end?(fmt,i+1) and fmt.i ~= CTL repeat i:=i+1;

			-- Back up if necessary.
			if fmt.i = CTL then i:=i-1;

			-- Put the substring.
			write!(on, fmt, i0, i+1);
			i0 := i+1
		}
	        return i0;
	}

	--`putFormat' puts the characters from the format string onto
	-- the TextWriter, with the formatted items interspersed as desired.

	local putFormat(fmt, i0, on, items): () == {
		narg   := 1;
		nitems := length items;

		repeat {
			i0 := putSpan(fmt, i0, on);
			if end?(fmt,i0) then break;
			if fmt.i0 ~= CTL then error "Bad case in putFormat.";
			(i0, narg) := doControl(fmt,i0,on,narg,items);
		}
	}

	--`doControl' handles the control sequences in the format.
	-- An input  value of DONT for narg means not to handle items.
	-- An output value of CANT for narg means the format couldn't be done.

	BADFMT ==> "Bad format string.";

	local doControl(fmt, i0, on, narg, items): (SI, SI) == {
		i00 := i0;

		if i0 < 1 or fmt.i0 ~= CTL then error BADFMT;
		i0  := i0 + 1;

		if end?(fmt, i0) then error BADFMT;
		c  := lower fmt.i0;
		i0 := i0 + 1;

		-- Handle easy cases without parameter.
		if c = CTL       then { write!(on, c);       return (i0,narg) }
		if c = char "n"  then { write!(on, newline); return (i0,narg) }
		if c = char "t"  then { write!(on, tab);     return (i0,narg) }

		if digit? c then {
			if narg = DONT then return (i00, CANT);
			return doControlA(i0, on, ord c - ord char "0", items);
		}

		-- Collect the numeric parameter, if there is one.
		parm  := 0;
		parm? := false;
		if c = char "{" then {
			parm? := true;
			while not end?(fmt, i0) and digit? fmt.i0 repeat {
				parm  := 10 * parm + ord fmt.i0 - ord char "0";
				i0    := i0 + 1;
			}
			if end?(fmt,i0) or fmt.i0 ~= char "}" then error BADFMT;
			i0 := i0 + 1;
			if end?(fmt, i0) then error BADFMT;
			c  := lower fmt.i0;
			i0 := i0 + 1;
		}

		-- Handle cases with parameter.
		if c = char "a" then {
			if narg = DONT then return (i00, CANT);
			if parm? then narg := parm;
			return doControlA(i0, on, narg, items);
		}
		error BADFMT;
		(i0, narg)
	}

	local doControlA(i0, on, narg, items): (SI, SI) == {
		if narg < 1 or narg > length items then error BADFMT;
		element(items, narg)(on);
		(i0, narg+1)
	}
}
