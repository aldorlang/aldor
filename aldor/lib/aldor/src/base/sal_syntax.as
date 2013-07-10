---------------------------- sal_syntax.as ---------------------------------
--
-- This file defines the syntax exception to be thrown by << on a TextReader
--
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 2001
-- Logiciel Salli ©INRIA 2001
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{SyntaxException}
\History{Manuel Bronstein}{23/4/2001}{created}
\Usage{
throw \this\\
try \dots catch E in \{ E has \astype{SyntaxExceptionType} $=>$ \dots \}
}
\Descr{\this~is an exception type thrown by read operations from
a \astype{TextReader}.}
#endif
SyntaxException: SyntaxExceptionType == add;

#if ALDOC
\thistype{SyntaxExceptionType}
\History{Manuel Bronstein}{23/4/2001}{created}
\Usage{\this: Category}
\Descr{\this~is the category of exceptions thrown by read operations
from a \astype{TextReader}.}
#endif
define SyntaxExceptionType:Category == with;

