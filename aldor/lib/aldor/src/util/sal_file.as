------------------------------- sal_file.as ----------------------------------
--
-- This file provides an interface to the C FILE* type
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel libaldor (c) INRIA 1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{File}
\History{Manuel Bronstein}{14/10/98}{created}
\History{Manuel Bronstein}{21/10/99}{added exceptions}
\History{Manuel Bronstein}{23/6/2003}{replaced mktemp by mkstemp in uniqueName}
\History{Manuel Bronstein}{2/12/2003}{added position and various setPosition!}
\Usage{import from \this}
\Descr{\this~is a type whose elements are operating system files.}
\begin{exports}
\alexp{close!}: & \% $\to$ \altype{MachineInteger} & close a file\\
\alexp{coerce}:
& \% $\to$ \altype{BinaryReader} & conversion to a binary input stream\\
\alexp{coerce}:
& \% $\to$ \altype{BinaryWriter} & conversion to a binary output stream\\
\alexp{coerce}:
& \% $\to$ \altype{TextReader} & conversion to a text input stream\\
\alexp{coerce}:
& \% $\to$ \altype{TextWriter} & conversion to a text output stream\\
\alexp{fileAppend}: & \altype{MachineInteger} & mode for \alexp{open}\\
\alexp{fileBinary}: & \altype{MachineInteger} & mode for \alexp{open}\\
\alexp{fileRead}: & \altype{MachineInteger} & mode for \alexp{open}\\
\alexp{fileText}: & \altype{MachineInteger} & mode for \alexp{open}\\
\alexp{fileWrite}: & \altype{MachineInteger} & mode for \alexp{open}\\
\alexp{open}: & (\altype{String}, Z) $\to$ \% & open a file\\
\alexp{position}: & \% $\to$ Z & current position\\
\alexp{remove}: & \altype{String} $\to$ () & removes a file\\
\alexp{setPosition!}: & (\%, Z) $\to$ Z & moves to a new position\\
\alexp{setPositionFromEnd!}: & (\%, Z) $\to$ Z & moves to a new position\\
\alexp{setRelativePosition!}: & (\%, Z) $\to$ Z & moves to a new position\\
\alexp{uniqueName}:
& \altype{String} $\to$ \altype{String} & get a unique filename\\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

macro {
	Ch == Character;
	Z  == MachineInteger;
}

File: with {
	close!: % -> Z;
#if ALDOC
\alpage{close!}
\Usage{\name~f}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em f} & \% & a file\\ }
\Descr{Closes the file \emph{f}, returning 0 if no error occured,
a nonzero error code (the value of {\tt errno} from C) otherwise.}
#endif
	coerce: % -> TextReader;
	coerce: % -> TextWriter;
	coerce: % -> BinaryReader;
	coerce: % -> BinaryWriter;
#if ALDOC
\alpage{coerce}
\Usage{f::BinaryReader\\ f::BinaryWriter\\ f::TextReader\\ f::TextWriter}
\Signatures{
\name: & \% $\to$ \altype{BinaryReader}\\
\name: & \% $\to$ \altype{BinaryWriter}\\
\name: & \% $\to$ \altype{TextReader}\\
\name: & \% $\to$ \altype{TextWriter}\\
}
\Params{ {\em f} & \% & a file\\ }
\Descr{Converts the file f to to a binary or text reader or writer.
This is necessary before reading from or writing to the file. The file must
have been opened in an appropriate mode for reading or writing respectively.}
\Remarks{Coercing a file to a reader or writer allocates memory, so it
is advisable to assign the resulting stream to a variable. Unlike the ones
for \altype{String}, those coercions do not reset the file to its beginning.}
#endif
	fileAppend: Z;
	fileBinary: Z;
	fileRead: Z;
	fileText: Z;
	fileWrite: Z;
#if ALDOC
\alpage{fileAppend,fileBinary,fileRead,fileText,fileWrite}
\altarget{fileAppend}
\altarget{fileBinary}
\altarget{fileRead}
\altarget{fileText}
\altarget{fileWrite}
\Usage{fileAppend\\ fileBinary\\ fileRead\\ fileText\\ fileWrite}
\alconstant{\altype{MachineInteger}}
\Descr{Those constants are for use in the mode parameter of the
\alexp{open} function.}
#endif
	open: (String, n:Z == fileRead) -> %;
#if ALDOC
\alpage{open}
\Usage{\name(s,m)}
\Signature{(\altype{String}, \altype{MachineInteger})}{\%}
\Params{
{\em s} & \altype{String} & a filename\\
{\em m} & \altype{MachineInteger} & a mode (optional)\\
}
\Descr{Opens the file with the name s in the mode m,
and returns the opened file. The mode is any combination of the constants
\alexp{fileAppend}, \alexp{fileRead},
and \alexp{fileWrite}, together with one of \alexp{fileBinary}
or \alexp{fileText}, grouped together
with \alfunc{AdditiveType}{+} or
\alalias{BooleanArithmeticType}{or}{$\backslash/$}.
The default is \alexp{fileRead} + \alexp{fileText}.}
\Remarks{\name~returns \alfunc{Pointer}{nil} and throws the
exception \altype{FileException} if the file cannot be opened for
any reason.}
#endif
	position: % -> Z;
#if ALDOC
\alpage{position}
\Usage{\name~f}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em f} & \% & a file\\ }
\Retval{Returns the file position indicator of \emph{f}
(same as {\tt ftell} in C).}
\alseealso{\alexp{setPosition!}}
#endif
	remove: String -> ();
#if ALDOC
\alpage{remove}
\Usage{\name~s}
\Signature{\altype{String}}{()}
\Params{ {\em s} & \altype{String} & A file name\\ }
\Descr{Removes the file with name s in the file system.}
#endif
	setPosition!: (%, Z) -> Z;
	setPositionFromEnd!: (%, Z) -> Z;
	setRelativePosition!: (%, Z) -> Z;
#if ALDOC
\alpage{setPosition!,setPositionFromEnd!,setRelativePosition!}
\altarget{setPosition!}
\altarget{setPositionFromEnd!}
\altarget{setRelativePosition!}
\Usage{\name(f,n)}
\Signature{(\%,Z)}{Z}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
\Params{
\emph{f} & \% & a file\\
\emph{n} & \altype{MachineInteger} & a signed offset\\
}
\Descr{setPosition!(f, n) sets the file position indicator of \emph{f}
to \emph{n}, while setPositionFromEnd!(f, n) sets it to \emph{n} from
the end of file and setRelativePosition!(f, n) adds \emph{n} to it.
All three return 0 if no error occured,
a nonzero error code (the value of {\tt errno} from C) otherwise.}
\alseealso{\alexp{position}}
#endif
	uniqueName: String -> String;
#if ALDOC
\alpage{uniqueName}
\Usage{\name~s}
\Signature{\altype{String}}{\altype{String}}
\Params{ {\em s} & \altype{String} & A root string\\ }
\Retval{Returns a unique name with prefix s in the file system.}
#endif
} == add {
	Rep == Pointer;

	import {
		fflush: Pointer -> ();
		fclose: Pointer -> Z;
		fgetc: Pointer -> Z;
		ftell: Pointer -> Z;
		cerrno: () -> Z;
		fseekset: (Pointer, Z) -> Z;
		fseekcur: (Pointer, Z) -> Z;
		fseekend: (Pointer, Z) -> Z;
		-- TEMPO: NEEDS A TYPE FOR C-int
		-- fputc: (CInteger, Pointer) -> CInteger;
		-- ungetc: (CInteger, Pointer) -> CInteger;
		lfputc: (Z, Pointer) -> Z;
		lungetc: (Z, Pointer) -> Z;
	} from Foreign C;

	-- bit mask for open-mode:
	--	b0    =  0 = noread,   1 = read
	--	b1    =  0 = nowrite,  1 = write
	--	b2    =  0 = noappend, 1 = append
	--	b3    =  0 = text,     1 = binary
	fileAppend:Z		== 4;
	fileBinary:Z		== 8;
	fileRead:Z		== 1;
	fileText:Z		== 0;
	fileWrite:Z		== 2;
	close!(file:%):Z	== { zero? fclose rep file => 0; cerrno(); }
	local ok?(file:%):Boolean	== { import from Rep; ~nil?(rep file) }
	position(file:%):Z			== ftell rep file;
	local putb!(s:Pointer)(b:Byte):()	== lfputc(b::Z, s);
	local putc!(s:Pointer)(c:Ch):()		== lfputc(ord c, s);
	local getb(s:Pointer)():Byte		== lowByte fgetc s;
	local getc(s:Pointer)():Ch		== char fgetc s;
	local push(s:Pointer)(c:Ch):()		== lungetc(ord c, s);
	local flush(s:Pointer)():()		== fflush s;

	setPosition!(file:%, offset:Z):Z == {
		zero? fseekset(rep file, offset) => 0;
		cerrno();
	}

	setPositionFromEnd!(file:%, offset:Z):Z == {
		zero? fseekend(rep file, offset) => 0;
		cerrno();
	}

	setRelativePosition!(file:%, offset:Z):Z == {
		zero? fseekcur(rep file, offset) => 0;
		cerrno();
	}

	uniqueName(s:String):String == {
		-- YC --> MB: mktemp obsolete in GCC, use mkstemp instead
		-- import { mktemp: Pointer -> Pointer } from Foreign C;
		-- string mktemp pointer(s + "XXXXXX");
		import { mkstemp: Pointer -> MachineInteger } from Foreign C;
		t := s + "XXXXXX";
		mkstemp pointer t;
		t;
	}

	remove(s:String):() == {
		import { unlink: Pointer -> Z } from Foreign C;
		unlink pointer s;
	}

	coerce(file:%):BinaryWriter == {
		assert(ok? file);
		binaryWriter(putb! rep file, flush rep file);
	}

	coerce(file:%):BinaryReader == {
		assert(ok? file);
		binaryReader getb rep file;
	}

	coerce(file:%):TextWriter == {
		assert(ok? file);
		textWriter(putc! rep file, flush rep file);
	}

	coerce(file:%):TextReader == {
		assert(ok? file);
		textReader(getc rep file, push rep file);
	}

	open(name:String, mode:Z):% == {
		import from Rep;
		import { fopen:(Pointer,Pointer) -> Pointer; } from Foreign C;
		read? := bit?(mode, 0);
		write? := bit?(mode, 1);
		s:String := {
			bit?(mode, 2) => "a";
			read? => "r";
			write? => "w";
			empty;
		}
		if read? and write? then s := s + "+";
		if bit?(mode, 3) then s := s + "b";
		empty? s or nil?(ptr := fopen(pointer name, pointer s)) =>
							throw FileException;
		per ptr;
	}
}

#if ALDOC
\thistype{FileException}
\History{Manuel Bronstein}{21/10/99}{created}
\Usage{
throw \this\\
try \dots catch E in \{ E has \altype{FileExceptionType} $=>$ \dots \}
}
\Descr{\this~is an exception type thrown by file operations.}
#endif
FileException: FileExceptionType == add;

#if ALDOC
\thistype{FileExceptionType}
\History{Manuel Bronstein}{21/10/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of exceptions thrown by file operations.}
#endif
define FileExceptionType:Category == with;
