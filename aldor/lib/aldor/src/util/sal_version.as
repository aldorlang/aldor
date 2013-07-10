---------------------------- sal_version.as ----------------------------------
--
-- This file provides utilities for storing version info in libraries
--
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Z == MachineInteger;
	CPR == "Aldor Library - Copyright (c) 1998-2005, INRIA and M.Bronstein";
	VER == "Aldor Library Version 1.1.0 - April 19, 2005";
}

#if ALDOC
\thistype{AldorLibraryInformation}
\History{Manuel Bronstein}{24/9/2001}{created}
\Usage{import from \this}
\Descr{\this~provides \libaldor{} version information.}
\begin{exports}
\category{\altype{VersionInformationType}}\\
\end{exports}
#endif

AldorLibraryInformation: VersionInformationType == add {
	name:String		== "libaldor";
	version:String		== VER;
	credits:List String	== [CPR];
	major:Z			== 1;
	minor:Z			== 2;
	patch:Z			== 0;
}


#if ALDOC
\thistype{VersionInformationType}
\History{Manuel Bronstein}{24/9/2001}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types providing version information.}
\begin{exports}
\alexp{credits}: & \altype{List} \altype{String} & various credits\\
\alexp{major}: & \altype{MachineInteger} & major version number\\
\alexp{minor}: & \altype{MachineInteger} & minor version number\\
\alexp{name}: & \altype{String} & library name\\
\alexp{patch}: & \altype{MachineInteger} & patch version number\\
\alexp{version}: & \altype{String} & version information\\
\end{exports}
#endif

define VersionInformationType:Category == with {
	credits: List String;
#if ALDOC
\alpage{credits}
\alconstant{\altype{List} \altype{String}}
\Retval{Returns a list of lines, crediting the various authors of a library.}
#endif
	major: Z;
	minor: Z;
	patch: Z;
#if ALDOC
\alpage{major,minor,patch}
\altarget{major}
\altarget{minor}
\altarget{patch}
\Usage{\name}
\alconstant{\altype{MachineInteger}}
\Retval{Returns the major and minor version of a library as well as
its patchlevel.}
#endif
	name: String;
#if ALDOC
\alpage{name}
\Usage{\name}
\alconstant{\altype{String}}
\Retval{Returns the name of the library.}
#endif
	version: String;
#if ALDOC
\alpage{version}
\Usage{\name}
\alconstant{\altype{String}}
\Retval{Returns a string describing the current version of a library.}
#endif
}

