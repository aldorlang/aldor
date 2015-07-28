---------------------------- sal_string.as ----------------------------------
--
-- This file defines 0-indexed null-terminated strings.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{String}
\History{Manuel Bronstein}{4/10/98}{created}
\Usage{import from \this}
\Descr{\this~provides basic strings, null--terminated,
$0$-indexed and without bound checking.}
\begin{exports}
\category{\altype{ArrayType}(\altype{Character},
\altype{PackedPrimitiveArray} \altype{Character})}\\
\alexp{$+$}: & (\%, \%) $\to$ \% & concatenation\\
\alexp{char}: & \% $\to$ \altype{Character} & first character\\
\alexp{coerce}: & \altype{Character} $\to$ \% & conversion to a string\\
\alexp{coerce}:
& \% $\to$ \altype{TextReader} & conversion to a text input stream\\
\alexp{coerce}:
& \% $\to$ \altype{TextWriter} & conversion to a text output stream\\
\alexp{error}: & \% $\to$ \builtin{Exit} & error exit\\
\alexp{leftTrim}: & (\%, \altype{Character}) $\to$ \% & trim leading part\\
\alexp{rightTrim}: & (\%, \altype{Character}) $\to$ \% & trim trailing part\\
\alexp{rightTrim!}: & (\%, \altype{Character}) $\to$ \% & trim trailing part\\
\alexp{new}: & Z $\to$ \% & buffer allocation\\
\alexp{pointer}: & \% $\to$ \altype{Pointer} &
conversion to a null--terminated C--string\\
\alexp{string}: & \altype{Pointer} $\to$ \% &
conversion from a null--terminated C--string\\
\alexp{substring}: & (\%, Z)    $\to$ \% & substring\\
                   & (\%, Z, Z) $\to$ \% & \\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

String: ArrayType(Ch, PackedPrimitiveArray Ch) with {
	+: (%, %) -> %;
#if ALDOC
\alpage{$+$}
\Usage{s \name~t}
\Signature{(\%, \%)}{\%}
\Params{ {\em s,t} & \% & strings\\ }
\Retval{$s \name t$ returns the string $s t$. Copies all the characters
of $s$ and $t$, so $s$ is unchanged after the call, and $s$ and $t$ do
not share characters with $s + t$.}
\Remarks{$s + \alfunc{FiniteLinearStructureType}{empty}$
and $\alfunc{FiniteLinearStructureType}{empty} + s$ both return
a copy of $s$.}
\begin{asex}
If $s$ is the string ``abcde'',
then $s + (s + 2)$ is the string ``abcdecde''.
\end{asex}
#endif
	char: % -> Ch;
#if ALDOC
\alpage{char}
\Usage{\name~s}
\Signature{\%}{\altype{Character}}
\Params{ {\em s} & \% & a nonempty string\\ }
\Descr{Returns the first character of s.}
#endif
	coerce: Ch -> %;
	coerce: % -> TextReader;
	coerce: % -> TextWriter;
#if ALDOC
\alpage{coerce}
\Usage{c::\%\\ s::TextReader\\ s::TextWriter}
\Signatures{
\name: & \altype{Character} $\to$ \%\\
\name: & \% $\to$ \altype{TextReader}\\
\name: & \% $\to$ \altype{TextWriter}\\
}
\Params{
{\em c} & \altype{Character} & a character\\
{\em s} & \% & a string\\
}
\Descr{c::String converts the character c to the string ``c'', while
s::T where T is an I/O stream type
converts the string s to a text reader or writer, allowing
one to read data or write data to it.}
\Remarks{When writing to a string, you must ensure that the string is
large enough for all the data that will be written to it, since the
string will not be extended and this function does not protect you against
overwriting. When reading from or writing to a string, each coercion to
a reader or writer resets the stream to the beginning of the string,
and the string is not side--affected by the subsequent read or write
operations, while the stream is side--affected.
Thus, when reading several values from the same string, you must assign
the reader to a variable and read the values from that variable,
as in the example below.}
\begin{asex}
\begin{ttyout}
import from MachineInteger, String;
s := "  12  56";
a:MachineInteger := << s::TextReader;
b:MachineInteger := << s::TextReader;
\end{ttyout}
assigns the value 12 to both a and b, while
\begin{ttyout}
import from MachineInteger, String;
s := "  12  56";
p := s::TextReader;
a:MachineInteger := << p;
b:MachineInteger := << p;
\end{ttyout}
assigns 12 to a and 56 to b.
\end{asex}
\alseealso{PrimitiveMemoryBlock}{coerce}
#endif
	error: % -> Exit;
#if ALDOC
\alpage{error}
\Usage{\name~s}
\Signature{\%}{\builtin{Exit}}
\Params{ {\em s} & \% & a string\\ }
\Descr{Writes the message s to \alfunc{TextWriter}{stderr} and exits.}
#endif
	++ `leftTrim(s,c)' removes leading  occurrences of `c' from `s'.
	++ `rightTrim(s,c)' removes trailing occurrences of `c' from `s'.
	leftTrim: (%, Character) -> %;
        rightTrim: (%, Character) -> %;
        rightTrim!: (%, Character) -> %;
#if ALDOC
\alpage{leftTrim,rightTrim}
\altarget{leftTrim}
\altarget{rightTrim}
\altarget{rightTrim!}
\Usage{leftTrim(s, c)\\ rightTrim(s, c)\\ rightTrim!(s, c)}
\Signature{(\%, \altype{Character})}{\%}
\Params{
{\em s} & \% & a string\\
\emph{c} & \altype{Character} & a character\\
}
\Descr{leftTrim(s,c) (resp.~rightTrim(s,c)) returns a copy of \emph{s}
without its leading (resp.~trailing) occurences of \emph{c}.
No copy is made and \emph{s} is returned if the leading (resp.~trailing)
character of \emph{s} is not \emph{c}.
rightTrim!(s,c) removes the trailing occurences of \emph{c} from \emph{s}
without making a copy.}
#endif
	new: Z -> %;
#if ALDOC
\alpage{new}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\%}
\Params{ {\em n} & \altype{MachineInteger} & a nonnegative size\\ }
\Retval{Returns a string of $n+1$ null characters. This is useful when
creating a memory buffer of a specified number of bytes, or when creating
a string to be used as an \altype{TextWriter}.}
#endif
	pointer: % -> Pointer;
	string: Pointer -> %;
#if ALDOC 
\alpage{pointer,string}
\altarget{pointer}
\altarget{string}
\Usage{pointer~s\\ string~p}
\Signatures{
pointer: & \% $\to$ \altype{Pointer}\\
string: & \altype{Pointer} $\to$ \%\\
}
\Params{ 
{\em s} & \% & A \salli string\\
{\em p} & \altype{Pointer} & A null--terminated C--string\\
}
\Descr{Use those functions to safely convert between null--terminated
returned or needed by C--functions
and \salli strings. Those functions have no effect
in the release version of \salli, but they are
necessary when using the debug version, so it is recommended to use
them in all cases. The C--type {\tt char*} should be replaced
by \altype{Pointer} in the prototypes
when using C--functions in \salli clients.}
#endif
	string: Literal -> %;
	++ `substring(s, start, len)' returns the substring of `s'
	++ of length `len' beginning at position `start'.
        substring: (%, Z) -> %;
        substring: (%, Z, Z) -> %;
	literal: % -> Literal;
#if ALDOC
\alpage{substring}
\Usage{\name(s, n)\\ \name(s, n, m)}
\Signatures{
\name: & (\%, \altype{MachineInteger}) $\to$ \%\\
\name: & (\%, \altype{MachineInteger}, \altype{MachineInteger}) $\to$ \%\\
}
\Params{
{\em s} & \% & a string\\
\emph{n} & \altype{MachineInteger} & an index\\
\emph{m} & \altype{MachineInteger} & a length\\
}
\Descr{
\name(s,n) returns a copy of the substring starting at position \emph{n}
of \emph{s}, while \name(s,n,m) returns a copy of the substring of length
\emph{m} starting at position \emph{n} of \emph{s}.}
\Remarks{\name(s,0) returns a copy of \emph{s}.}
#endif
} == add {
	Rep == PackedPrimitiveArray Ch;

	pointer(s:%):Pointer		== { import from Rep; pointer rep s; }
	apply(x:%, n:Z):Ch		== { import from Rep; rep(x).n }
	set!(x:%, n:Z, y:Ch):Ch		== { import from Rep; rep(x).n := y; }
	coerce(s:%):TextReader		== textReader getc! s;
	coerce(s:%):TextWriter		== textWriter putc! s;
	new(n:Z):%			== { import from Ch; new(n, null); }
	coerce(c:Ch):%			== { import from Z; new(1, c); }
	empty:%				== { import from Z; new 0; }
	empty?(s:%):Boolean		== { import from Z; zero?(#s); }
	firstIndex:Z			== 0;
	local quote:Ch			== char "_"";
	string(l:Literal):%		== string(l pretend Pointer);
	literal(s: %): Literal          == s pretend Literal;
        substring(s: %, pos: Z): %	== substring(s, pos, #s - pos);
	data(s:%):PackedPrimitiveArray Ch	== rep s;

        substring(s1: %, pos: Z, len: Z): % == {
		assert(len >= 0);
		zero? len => empty;
		n := #s1;
		assert(pos >= 0); assert(pos < n);
		if len > n - pos then len := n - pos;
                s2:% := new len;
                for i in 0..len-1 repeat s2.i := s1(i + pos);
                s2;
        }

	rightTrim(s: %, c: Character): % == {
		empty? s or s(prev(#s)) ~= c => s;	-- no copy if no trim
		rightTrim!(copy s, c);
	}

	rightTrim!(s: %, c: Character): % == {
		n := prev(#s);
		while n >= 0 and s.n = c repeat { s.n := null; n := prev n };
		s;
	}

	leftTrim(s: %, c: Character): % == {
		n := prev(#s);
		n < 0 or s.0 ~= c => s;		-- no copy if no trim
		for i in 1..n repeat { s.i ~= c => return substring(s, i) }
		empty;
	}

	free!(x:%):() == {
		import from Rep;
		if ~empty? x then free! rep x;
	}

	array(a:PackedPrimitiveArray Ch, n:Z):%	== {
		import from Ch;
		a.n := null;
		per a;
	}

	resize!(s:%, n:Z):% == {
		import from Rep;
		assert(n >= 0);
		if zero?(m := #s) then {
			-- do not call resize!$Rep, that would free empty
			if n > 0 then {
				s := per new next n;
				s.0 := null;
			}
			s;
		}
		else if n > m then s := per resize!(rep s, next m, next n);
		s.n := null;
		s;
	}

	char(x:%):Ch == {
		import from Z, Boolean;
		assert(~empty? x);
		x.0;
	}

	bracket(g:Generator Ch):% == {
		import from Z, List Ch;
		l:List Ch := [g];
		zero?(n := #l) => empty;
		s:% := new n;
		i:Z := 0;
		for c in l repeat {
			s.i := c;
			i := next i;
		}
		s;
	}

	map!(f:Ch -> Ch)(s:%):% == {
		import from Z;
		n := prev(#s);
		for i in 0..n repeat s.i := f(s.i);
		s;
	}

	map(f:Ch -> Ch)(s:%):% == {
		import from Z;
		zero?(n := #s) => empty;
		t:% := new n;
		for i in 0..prev n repeat t.i := f(s.i);
		t;
	}

	copy!(t:%, s:%):% == {
		import from Boolean, Z;
		tooSmall? := (n := #s) > #t;
		if tooSmall? then t := new n;
		for i in 0..prev n repeat t.i := s.i;
		if ~tooSmall? then t.n := null;
		t;
	}

	copy(s:%):% == {
		import from Z;
		zero?(n := #s) => empty;
		t:% := new n;
		for i in 0..prev n repeat t.i := s.i;
		t;
	}

	error(a:%):Exit == {
		import from TextWriter, Ch, Z, Machine;
		stderr << a << newline;
		halt 0;
	}

	<< (p:BinaryReader):% == {
		import from Z, Ch;
		n:Z := << p;				-- read size first
		s:% := new n;
		for m in 0..prev n repeat s.m := << p;
		s;
	}

	bracket(t:Tuple Ch):% == {
		import from Z;
		zero?(n := length t) => empty;
		s := new n;
		for i in 0..prev n repeat s.i := element(t, next i);
		s;
	}

	(s:%) + (t:%):% == {
		import from Z;
		a := new(#s + #t);
		n:Z := 0;
		for c in s repeat { a.n := c; n := next n; }
		for c in t repeat { a.n := c; n := next n; }
		a;
	}

	new(n:Z, x:Ch):% == {
		import from Rep;
		assert(n >= 0);
		s := per new(next n, x);
		s.n := null;
		s;
	}

#if DEBUG
	string(p:Pointer):% == {
		import from Rep, Boolean, Ch, Z, Machine;
		import { ArrElt: (Arr, SInt) -> Char } from Builtin;
		n:Z := 0;
		s := p pretend Arr;
		while ~zero?(ord(ArrElt(s, n::SInt)::Ch)) repeat n := next n;
		per array(p, next n);
	}
#else
	-- actual size is irrelevant
	string(p:Pointer):% == { import from Rep; per array(p, 0); }
#endif

	generator(a:%):Generator Ch == generate {
		import from Boolean, Ch, Z;
		for n in 0.. while ~zero?(ord(c := a.n)) repeat yield c;
	}

	local putc!(s:%):Ch -> () == {
		i:Z := 0;
		(c:Ch):() +-> { s.i := c; free i := next i; }
	}

	-- Text-mode scanning, send eof on a null-character
	local getc!(s:%):(() -> Ch, Ch -> ()) == {
		import from Boolean, Z;
		i:Z := 0;
		(():Ch +-> {
			zero? ord(c := s.i) => eof;
			free i := next i;
			c;
		}, (c:Ch):() +-> {
			if ~(zero? i or c = eof) then {
				free i := prev i;
				s.i := c;
			}
		})
	}

	#(s:%):Z == {
		import from Boolean, Ch;
		n:Z := 0;
		while ~zero?(ord(s.n)) repeat n := next n;
		n;
	}

	(a:%) = (b:%):Boolean == {
		import from Z, Ch;
		(na := #a) ~= #b => false;
		for n in 0..prev na repeat {
			a.n ~= b.n => return false;
		}
		true;
	}

	(p:TextWriter) << (s:%):TextWriter == {
		import from Ch;
		for c in s repeat p << c;
		p;
	}

	-- strings must be either quoted, in which case they contain any char,
	-- or chains of letters and digits (no punctuation or special chars)
	<< (p:TextReader):% == {
		import from Z, Ch, List Ch;
		local c:Ch;
		while space?(c := << p) or c = newline repeat {};
		l := { c = quote => quotedString p; unquotedString(p, c) }
		s:% := new(n := #l);
		for cc in l for i in prev(n).. by -1 repeat s.i := cc;
		free! l;
		s;
	}

	-- opening quote has been read
	local quotedString(p:TextReader):List Ch == {
		import from Boolean, Ch;
		local c:Ch;
		l:List Ch := empty;
		c := << p;
		while c ~= quote repeat {
			l := cons(c, l);
			c := << p;
		}
		l;
	}

	-- first char has been read (in c), string is not quoted
	local unquotedString(p:TextReader, c:Ch):List Ch == {
		l:List Ch := empty;
		while letter? c or digit? c repeat {
			l := cons(c, l);
			c := << p;
		}
		c = newline => cons(c, l);
		push!(c, p);
		l;
	}
}

extend Enumeration(T: Tuple Type): Join(PrimitiveType, OutputType) == add {
	-- this is due to the compiler's internal handling of Enumeration
	local TT(): Tuple Pointer == T pretend Tuple Pointer;
	Rep == MachineInteger;
	import from Rep;

	(a: %) = (b: %): Boolean == rep a = rep b;

	(outp: TextWriter) << (a: %): TextWriter == {
		import from String, Tuple Pointer;
		outp << string element(TT(), next rep a);
	}
}

#if ALDOC
\thistype{StringBuffer}
\History{Manuel Bronstein}{14/11/2003}{created}
\Usage{import from \this}
\Descr{\this~provides extensible character buffers that can be used as
input or output, either for reading an undetermined number of characters
from a \altype{TextReader} into them,
or for writing an undetermined number of characters into them.}
\begin{exports}
\category{\altype{DataStructureType}}\\
\category{\altype{OutputType}}\\
\category{\altype{PrimitiveType}}\\
\alexp{coerce}:
& \% $\to$ \altype{TextReader} & conversion to a text input stream\\
\alexp{coerce}:
& \% $\to$ \altype{TextWriter} & conversion to a text output stream\\
\alfunc{FiniteLinearStructureType}{empty}: & \% & create an empty string\\
\alexp{string}: & \% $\to$ \altype{String} & conversion to a regular string\\
\end{exports}
#endif

StringBuffer: Join(DataStructureType, PrimitiveType, OutputType) with {
	coerce: % -> TextReader;
	coerce: % -> TextWriter;
#if ALDOC
\alpage{coerce}
\Usage{s::TextReader\\ s::TextWriter}
\Signatures{
\name: & \% $\to$ \altype{TextReader}\\
\name: & \% $\to$ \altype{TextWriter}\\
}
\Params{ {\em s} & \% & a string\\ }
\Descr{s::T where T is an I/O stream type
converts the string s to a text reader or writer, allowing
one to read data or write data to it.}
\Remarks{When writing to an extensible string, the string grows to
accomodate all the data that you write to it.}
#endif
	new: () -> %;
#if ALDOC
\alpage{new}
\Signature{()}{\%}
\Retval{Returns a new empty buffer.}
#endif
	string: % -> String;
#if ALDOC 
\alpage{string}
\Usage{\name~s}
\Signature{\%}{\altype{String}}
\Params{ {\em s} & \% & a string\\ }
\Descr{Returns the underlying string.}
\Remarks{This function does not make a copy of the string, so if
\emph{s} is modified after calling {\tt str := \name s},
then \emph{str} is affected. Use \alfunc{CopyableType}{copy} on
\emph{str} if you need to prevent this.}
#endif
} == add {
	Rep == Record(sz:Z, str:String);
	import from String;

	new():%			== { import from Rep, Z; per [0, empty]; }
	empty?(s:%):Boolean	== { import from Z; zero? size s; }
	string(s:%):String	== { import from Rep; rep(s).str; }
	local size(s:%):Z	== { import from Rep; rep(s).sz; }
	free!(s:%):()		== free! string s;
	coerce(s:%):TextReader	== string(s)::TextReader;
	coerce(s:%):TextWriter	== textWriter putc! s;
	(s1:%) = (s2:%):Boolean	== string(s1) = string(s2);
	(p:TextWriter) << (s:%):TextWriter	== p << string s; 

	local update!(s:%, n:Z, strg:String):() == {
		import from Rep;
		rep(s).sz := n;
		rep(s).str := strg;
	}

	local putc!(s:%):Ch -> () == {
		n := size s;
		str := string s;
		i:Z := 0;
		(c:Ch):() +-> {
			if i >= n then {
				free n := { zero? n => 1; n + n };
				free str := resize!(str, n);
				update!(s, n, str);
			}
			str.i := c;
			free i := next i;
		}
	}
}

-- readUntil! and readWhile! are documented in sal_tstream.as
extend TextReader: with {
	readUntil!: (StringBuffer, %, Ch) -> Ch;
	readWhile!: (StringBuffer, %, Ch -> Boolean) -> Ch;
} == add {
	readUntil!(s:StringBuffer, port:%, stop:Ch):Ch ==
		readWhile!(s, port, (c:Ch):Boolean +-> c ~= stop);

	readWhile!(s:StringBuffer, port:%, read?:Ch -> Boolean):Ch == {
		wr := s::TextWriter;
		c:Ch := << port;
		while c ~= eof and read? c repeat { wr << c; c := << port }
		wr << null;
		c;
	}
}

extend OutputTypeFunctions(T: OutputType): with {
        toString: T -> String
}
== add {
        import from TextWriter;
	toString(t: T): String == {
	        sb: StringBuffer := new();
		(sb::TextWriter) << t;
		string sb
	}
}

#if ALDORTEST
---------------------- test sal_string.as --------------------------
#include "aldor"
#include "aldortest"

import from Assert String;
import from Assert Integer;
import from Assert MachineInteger;
import from Assert Character;

testIterate(): () == {
   import from String;
   import from MachineInteger;
   i := 0;
   lc := "x".0;
   for c in "hello" repeat {
       i := i+1;
       lc := c;
   }
   assertEquals(i, 5);
   assertEquals(lc, "o".0);
}

testBasics(): () == {
   testConcat(a: String, b: String, r: String): () ==
      assertEquals(r, a+b);
   import from String;
   import from Integer;
   testConcat("", "", "");
   testConcat("a", "", "a");
   testConcat("", "b", "b");
   testConcat("a", "b", "ab");
}

testToString(): () == {
    import from String;
    import from Integer;
    assertEquals("1234", toString 1234);
}

testBasics();
testIterate();
testToString();


#endif
