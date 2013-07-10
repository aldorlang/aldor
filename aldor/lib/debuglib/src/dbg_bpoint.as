
#include "dbg_defs"

------------------------------------------------------------------------
-- Representation of breakpoints.

+++ BreakPoint is a local domain used to represent traps in the path
+++ of execution. If the debugger is enabled and reaches a line which
+++ holds a breakpoint then special actions may be taken.
BreakPoint:SetCategory with
{
	new: (String, SingleInteger, SingleInteger) -> %;
		++ new(fil, lin, id) creates a breakpoint for line
		++ lin in the source file fil. The ID number is id.

	nil: () -> %;
		++ The invisible, impossible-to-hit breakpoint. It is
		++ illegal to apply any function other than nil? to
		++ this value.

	nil?: % -> Boolean;
		++ Returns true if given the special breakpoint nil.

	file: % -> String;
		++ file(b) returns the filename associated with b.

	line: % -> SingleInteger;
		++ line(b) returns the line number associated with b.

	number: % -> SingleInteger;
		++ number(b) returns the ID number of a breakpoint.

	apply: (%, 'hits') -> SingleInteger;
		++ `brk.hits' yields the hit count of the breakpoint.

	apply: (%, 'active?') -> Boolean;
		++ `brk.active?' yields true if the breakpoint is active.

	set!: (%, 'hits', SingleInteger) -> SingleInteger;
		++ `brk.hits := N' sets the hit count of the breakpoint.

	set!: (%, 'active?', Boolean) -> Boolean;
		++ `brk.active? := T' is used to make a breakpoint active
		++ or inactive.

	dispose!: % -> ();
		++ Discard the storage associated with this breakpoint.

	export from 'active?', 'hits';
}
== add
{
	Rep == Record
	(
		-- Immutable fields.
		fname:String, ------------------ File name
		fline:SingleInteger, ----------- Line number
		bnum:SingleInteger, ------------ ID number


		-- Mutable fields.
		hcount:SingleInteger, ---------- Number of hits
		isactive:Boolean --------------- Enabled?
	);
	import from Rep, 'hits', 'active?';

        local address(s:%):Pointer              == s pretend Pointer;
                                                                                                                              
        hash(s:%): MachineInteger  == { import from String; hash ((rep s).fname) * (rep s).fline}

	nil():% == (0@SingleInteger) pretend %;
	nil?(x:%):Boolean == zero?(x pretend SingleInteger);

	new(f:String, l:SingleInteger, id:SingleInteger):% ==
		per [f, l, id, 0, true];

	file(b:%):String == (rep b).fname;
	line(b:%):SingleInteger == (rep b).fline;
	number(b:%):SingleInteger == (rep b).bnum;

	apply(b:%, x:'hits'):SingleInteger == (rep b).hcount;
	set!(b:%, x:'hits', i:SingleInteger):SingleInteger ==
	{
		(rep b).hcount := i;
	}

	apply(b:%, x:'active?'):Boolean == (rep b).isactive;
	set!(b:%, x:'active?', f:Boolean):Boolean ==
	{
		(rep b).isactive := f;
	}

	(a:%) = (b:%):Boolean ==
	{
		-- Quick check on the line numbers.
		(not(line a = line b)) => false;


		-- All depends on the filename.
		file(a) = file(b);
	}

	(tw:TextWriter) << (b:%):TextWriter ==
	{
		tw << (b.active? => "[*]"; "[ ]");
		tw << " _"" << (file b);
		tw << "_" at line " << (line b);
		tw << " (";
		tw << "hit " << (b.hits) << " times).";
		tw;
	}
	
	sample:% == nil();

	dispose!(x:%):() ==
	{
		(nil? x) => return;
		dispose! rep x;
	}
}
