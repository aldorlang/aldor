-------------------------------- aldor.as ----------------------------------
--
-- Header file for libaldor clients
--
-- Used compile-time symbols:
--    AGAT:      agat on when asserted     AGAT(stream, value);
--    TIME:      profile on when asserted  TIMESTART; ...; TIME(message);
--    TRACE:     trace on when asserted    TRACE(message, value);
--
-- Copyright (c) Manuel Bronstein 1998-2001
-- Copyright (c) INRIA 2000-2001, Version 0.1.13
-- Logiciel LibAldor (c) INRIA 2000-2001, dans sa version 0.1.13
-----------------------------------------------------------------------------

-- This allows code to check whether it is built on top of libaldor
#assert LibraryAldorLib

#unassert ALDOC
#unassert ALDORTEST

macro {
	rep x == ((x)@%) pretend Rep;
	per r == ((r)@Rep) pretend %;
}

-- Selection of the appropriate portable object library
#if BuildAldorLib
#else
#if DEBUG
#library AldorLib "libaldord.al"
#else
#library AldorLib "libaldor.al"
#endif
#endif

import from AldorLib;
inline from AldorLib;

-- Selection of the GNU bignum and bigfloat packages via the -dGMP option
#if GMP
macro Integer	== GMPInteger;
macro Float	== GMPFloat;
#else
macro Integer	== AldorInteger;
#endif

-- Automatic import of Boolean is needed to compensate for a compiler weakness,
-- this allows to turn it off (necessary in some files when building libaldor)
#if DoNotImportBoolean
#else
import from Boolean;
#endif

-- Macros for simple profiling
#if TIME
macro {
	TIMESTART	== { import from Character, String, MachineInteger, _
				Timer, TextWriter; _
				start!(AlDoRcLoCk := timer()); }
	TIME(msg)	== { stderr<<msg<<space<<read AlDoRcLoCk<<newline; }
}
#else
macro {
	TIMESTART	== {};
	TIME(msg)	== {};
}
#endif

-- Macros for simple tracing
#if TRACE
macro TRACE(str, val) == { import from WriterManipulator, TextWriter, String;_
				stderr << str << val << endnl; }
#else
macro TRACE(str, val) == {};
#endif

-- Macros for Agat animation
#if AGAT
macro AGAT(str, val) == { import from Agat; agat(str, val); }
#else
macro AGAT(str, val) == {};
#endif

