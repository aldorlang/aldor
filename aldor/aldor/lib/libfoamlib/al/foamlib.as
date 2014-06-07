-----------------------------------------------------------------------------
----
---- foamlib.as: Top-level for standard configuration.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
----
---- Standard configurations.
----
-----------------------------------------------------------------------------

#if AssertConfig
	-- Configuration by Assertions --
#else
	-- Configuration for Typical AxlLib Clients --
#	assert AllMacros
#	assert AllImports
#	assert LibraryAxlLib
#endif

-----------------------------------------------------------------------------
----
---- Configuration customizations.
----
-----------------------------------------------------------------------------

#if AllMacros
#	assert RepMacros
#	assert MachineMacros
#endif

#if AllImports
#	assert ImportBoolean
#	assert ImportString
#	assert ImportCharacter
#	assert ImportTextWriter
#	assert ImportFormattedOutput
#endif

#if NRepMacros
#	unassert RepMacros
#endif

#if NMachineMacros
#	unassert MachineMacros
#endif

#if NImportBoolean
#	unassert ImportBoolean
#endif

#if NImportString
#	unassert ImportString
#endif

#if NImportCharacter
#	unassert ImportCharacter
#endif

#if NImportTextWriter
#	unassert ImportTextWriter
#endif

#if NImportFormattedOutput
#	unassert ImportFormattedOutput
#endif

-----------------------------------------------------------------------------
----
---- Assertion-selectable configurations
----
-----------------------------------------------------------------------------

#if RepMacros
macro {
	rep x == x @ % pretend Rep;
	per r == r @ Rep pretend %;
}
#endif

#if MachineMacros
macro {
	BBool == Bool$Machine;
	BChar == Char$Machine;
	BArr  == Arr$Machine;
	BPtr  == Ptr$Machine;
	BByte == XByte$Machine;
	BHInt == HInt$Machine;
	BSInt == SInt$Machine;
	BBInt == BInt$Machine;
	BSFlo == SFlo$Machine;
	BDFlo == DFlo$Machine;
}
#endif

#if LibraryLanguage
#	library LangLib "lang.ao"
{
	import from LangLib;
	inline from LangLib;
}
#endif

#if LibraryMachine
#	library LangLib "lang.ao"
#	library MachLib "machine.ao"
{
	import from LangLib, MachLib;
	import from Machine;
	inline from LangLib, MachLib;
	inline from Machine;
}
#endif

#if LibraryBasic
#	library LangLib  "lang.ao"
#	library MachLib  "machine.ao"
#	library BasicLib "basic.ao"
{
	import from LangLib, MachLib, BasicLib;
	import from Machine;
	inline from LangLib, MachLib, BasicLib;
	inline from Machine;
}
#endif

#if LibraryAsCat
#	library LangLib  "lang.ao"
#	library MachLib  "machine.ao"
#	library BasicLib "basic.ao"
#	library AsCatLib "axlcat.ao"
{
	import from LangLib, MachLib, BasicLib, AsCatLib;
	inline from LangLib, MachLib, BasicLib, AsCatLib;
	import from Machine;
	inline from Machine;
}
#endif


SingleInteger ==> MachineInteger;

#if LibraryAxlLib
#if BuildAxlLib
#else
#	library AxlLib "foamlib"
#endif
{
	import from AxlLib;
	inline from AxlLib;
}
#endif

#if ImportBoolean
{
	import from Boolean;
	inline from Boolean;
}
#endif

#if ImportString
{
	import {
		string:	Literal -> %;
		<<:	(TextWriter, %) -> TextWriter;
		<<:	% -> TextWriter -> TextWriter;
	} from String;
}
#endif

#if ImportCharacter
{
	import {
		newline: %;
		<<:	(TextWriter, %) -> TextWriter;
		<<:	% -> TextWriter -> TextWriter;
	} from Character;
}
#endif

#if ImportTextWriter
{
	import {
		print:	%;
		error:	%;
	} from TextWriter;
}
#endif

#if ImportFormattedOutput
{
	import from FormattedOutput;
}
#endif

