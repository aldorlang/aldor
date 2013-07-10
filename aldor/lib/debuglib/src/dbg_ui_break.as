
#include "dbg_defs"

------------------------------------------------------------------------
-- Abbreviations to keep signatures on one line.

SI  ==> SingleInteger;
PTR ==> Pointer;
STR ==> String;
TYP ==> Type;
CON ==> CallContext;
DBG ==> DebuggerState;
------------------------------------------------------------------------

BreakPointPackage: with
{
	breakpoint?: (DebuggerState, CON) -> Boolean;
		++ Have we hit a breakpoint?

	breakpoint: (DebuggerState, CON) -> BreakPoint;
		++ Return the breakpoint associated with the
		++ debugger state and calling context. The
		++ breakpoint may be in any state. If none
		++ were found then nil is returned.

	hit!: BreakPoint -> SI;
		++ Increment the record of the number of times that
		++ this breakpoint has been hit. The return value is
		++ the new hit count.

	create: (DebuggerState, List String, CON) -> ();
		++ Add a breakpoint at the specified position.

	delete: (DebuggerState, List String) -> ();
		++ Deletion of breakpoints.

	disable: (DebuggerState, List String) -> ();
		++ Disable of breakpoints.

	enable: (DebuggerState, List String) -> ();
		++ Enable of breakpoints.

	show: DebuggerState -> ();
		++ Display the breakpoints.
} == add
{
	import from SingleInteger;
	import from BreakPoint;
	import from String, TextWriter, Character;

	-- Create a new breakpoint and add to the tables.
	local newBreak(state:DebuggerState, fil:String, lin:SI):BreakPoint ==
	{
		local bpoint:BreakPoint;
		local slot:SingleInteger := -1;
		local blist:Array(BreakPoint) := state.breakList;


		-- First search for a spare slot.
		for i in 0.. prev #blist repeat
		{
			if (nil?(blist.i)) then
			{
				slot := i;
				break;
			}
		}


		-- Did we find a spare slot?
		if (slot < 0) then
		{
			-- No: extend the array.
			bpoint := new(fil, lin, #blist + 1);
			state.breakList := extend!(state.breakList, bpoint);
		}
		else
		{
			-- Yes: re-use it.
			bpoint := new(fil, lin, slot);
			blist.slot := bpoint;
		}


		-- Add the breakpoint to the hash table.
		(state.breakTable).bpoint := bpoint;


		-- Return the new breakpoint.
		bpoint;
	}


	-- Delete a single breakpoint.
	local deleteBreakpoint(state:DebuggerState, i:SingleInteger):() ==
	{
		local blist:Array(BreakPoint) := state.breakList;


		-- Safety checks.
		assert(not(nil?(blist.i)));
		assert((0 <= i) and (i <= prev #blist));


		-- Release the store associated with this breakpoint.
		dispose!(blist.i);


		-- Clobber it.
		blist.i := nil();
	}


	-- Disable a single breakpoint.
	local disableBreakpoint(state:DebuggerState, i:SingleInteger):() ==
	{
		local blist:Array(BreakPoint) := state.breakList;


		-- Safety checks.
		assert(not(nil?(blist.i)));
		assert((0 <= i) and (i <= prev #blist));


		-- Mark this breakpoint as disabled.
		(blist.i).active? := false;
	}


	-- Enable a single breakpoint.
	local enableBreakpoint(state:DebuggerState, i:SingleInteger):() ==
	{
		local blist:Array(BreakPoint) := state.breakList;


		-- Safety checks.
		assert(not(nil?(blist.i)));
		assert((0 <= i) and (i <= prev #blist));


		-- Mark this breakpoint as enabled.
		(blist.i).active? := true;
	}


	-- Operations over breakpoints: return the number processed;
	local map(state:DBG, cmds:List String, op:(DBG, SI) -> ()):SI ==
	{
		--import from SingleIntegerScanner;
		--import from NumberScanPackage SingleInteger;

		local bnum, count:SingleInteger;
		local dbout:TextWriter := state.dbOut;
		local blist:Array(BreakPoint) := state.breakList;


		-- Count the number processed.
		count := 0;


		-- Operate on all or individual ones by number?
		(#cmds = 0) =>
		{
			-- Apply to all: ought to check with the user
			-- if this is the delete operation.
			for i in 0..prev #blist repeat
			{
				-- Only process non-empty slots.
				if (not(nil?(blist.i))) then
				{
					op(state, i);
					count := count + 1;
				}
			}


			-- That's all folks.
			count;
		}


		-- Process each breakpoint listed.
		for cmd in cmds repeat
		{
			-- Convert the text into a number.
			--bnum := scanNumber(cmd);
			bnum := stringToInteger(cmd);


			-- Check that we have a valid number.
			if ((bnum < 0) or (bnum > prev #blist)) then
			{
				-- Invalid breakpoint.
				dbout << "*** Error: " << cmd;
				dbout << " is not a valid breakpoint.";
				dbout << newline;
				iterate;
			}


			-- Process this breakpoint.
			if (nil?(blist.bnum)) then
			{
				-- Ooops.
				dbout << "*** Error: breakpoint ";
				dbout << cmd << " doesn't exist!";
				dbout << newline;
			}
			else
			{
				op(state, bnum);
				count := count + 1;
			}
		}


		-- Return the count of the number processed.
		count;
	}


	-- Deletion of breakpoints.
	delete(state:DebuggerState, cmds:List String):() ==
	{
		local dbout:TextWriter := state.dbOut;
		local bnum:SI := map(state, cmds, deleteBreakpoint);


		-- Tell them what happened.
		if (bnum = 0) then
			dbout << "No breakpoints deleted!";
		else if (bnum = 1) then
			dbout << "Deleted 1 breakpoint.";
		else
			dbout << "Deleted " << bnum << " breakpoints.";
		dbout << newline;
	}


	-- Disabling of breakpoints.
	disable(state:DebuggerState, cmds:List String):() ==
	{
		local dbout:TextWriter := state.dbOut;
		local bnum:SI := map(state, cmds, disableBreakpoint);


		-- Tell them what happened.
		if (bnum = 0) then
			dbout << "No breakpoints disabled!";
		else if (bnum = 1) then
			dbout << "Disabled 1 breakpoint.";
		else
			dbout << "Disabled " << bnum << " breakpoints.";
		dbout << newline;
	}


	-- Enabling of breakpoints.
	enable(state:DebuggerState, cmds:List String):() ==
	{
		local dbout:TextWriter := state.dbOut;
		local bnum:SI := map(state, cmds, enableBreakpoint);


		-- Tell them what happened.
		if (bnum = 0) then
			dbout << "No breakpoints enabled!";
		else if (bnum = 1) then
			dbout << "Enabled 1 breakpoint.";
		else
			dbout << "Enabled " << bnum << " breakpoints.";
		dbout << newline;
	}

	
	-- Display of breakpoints.
	show(state:DebuggerState):() ==
	{

		--import from JustifyPackage(SingleInteger);
		local bpoint:BreakPoint;
		local found:SingleInteger;

		local dbout:TextWriter := state.dbOut;

		-- How many breakpoints defined?
		found := 0;


		-- Print all the breakpoints.
		for i in 0..prev #(state.breakList) repeat
		{
			-- Get the breakpoint.
			bpoint := (state.breakList).i;


			-- Is is a real one?
			if ((bpoint pretend SI) = 0) then
				iterate;


			-- Display the details.
			--dbout << justify(i, 3, right);
			dbout << ") " << bpoint;
			dbout << newline;


			-- Add to our total.
			found := found + 1;
		}


		-- Did we find any?
		if (found = 0) then
		{
			dbout << "No breakpoints defined.";
			dbout << newline;
		}
	}


	-- Addition of breakpoints.
	create(state:DebuggerState, cmds:List String, con:CON):() ==
	{
		--import from NumberScanPackage SingleInteger;
		--import from SingleIntegerScanner;
		--import from JustifyPackage(SingleInteger);

		local fname:String;
		local bpoint:BreakPoint;
		local dbout:TextWriter := state.dbOut;
		local fline:SingleInteger;

		-- Safety check.
		assert(#cmds > 0);

		local syntax():() ==
		{
			-- Syntax error.
			dbout << "*** Syntax error: ";
			dbout << "break [[<file>] <line>]";
			dbout << newline;
		}


		-- Stop now if too many arguments.
		(#cmds > 2) => { syntax(); return; }


		-- One or two arguments?
		if (#cmds = 1) then
		{
			-- Guess the filename.
			(nothing? con) =>
			{
				dbout << "*** No context: specify filename.";
				dbout << newline;
				return;
			}


			-- Filename from the current context.
			fname := con.file;


			-- Line number from the arguments.
			--fline := scanNumber(cmds.1);
			fline := stringToInteger(cmds.1);
		}
		else
		{
			-- Filename and line number from the arguments.
			fname := first cmds;
			--fline := scanNumber(cmds.2);
			fline := stringToInteger(cmds.2);
			     --dbout << fname << fline << cmds.1 << cmds.2<< newline;
		}

		-- Create a new breakpoint.
		bpoint := newBreak(state, fname, fline);

		-- Tell the user what happened.
		dbout << "Breakpoint set: " << bpoint << newline;
	}


	-- Return the breakpoint associated with the
	-- current context and debugger state. If none
	-- are found then we return nil()$BreakPoint.
	breakpoint(state:DebuggerState, con:CON):BreakPoint ==
	{
		import from String;
		import from Partial BreakPoint;
		import from HashTable(BreakPoint, BreakPoint);

		local key, result:BreakPoint;


		-- If no context then return the nil breakpoint.
		(nothing? con) => nil();


		-- Search for this breakpoint: breakpoint equality
		-- is based on line number and filename.
		key := new(con.file, con.line, -1);

		found: Partial BreakPoint := find(key, state.breakTable);
		--(found, result) := search(state.breakTable, key, nil());
		if failed? found then		
			result := nil();
		else
			-- Return whatever we found (nil if ~found).
			result := retract found;

		result
	}


	-- Have we hit a breakpoint?
	breakpoint?(state:DebuggerState, con:CON):Boolean ==
	{
		import from String;

		local bpoint:BreakPoint;


		-- Search for this breakpoint.
		bpoint := breakpoint(state, con);


		-- Stop if nothing found.
		(nil? bpoint) => false;


		-- We've hit a breakpoint: is it active?
		bpoint.active?;
	}


	-- Increment the hit count of a breakpoint.
	hit!(bpoint:BreakPoint):SingleInteger ==
	{
		bpoint.hits := bpoint.hits + 1;
	}


	stringToInteger(buf: String): SI == {
		import from TextWriter, String, Character, SI;

		bufpos := 0;
		ans : SI := 0;

		while ((buf.bufpos >= char "0") and (buf.bufpos <= char "9") and (bufpos < #buf)) repeat {
			ans := ans*10 + (ord buf.bufpos - ord char "0")::SI;
			bufpos := bufpos + 1;
		}
		ans;
	}

}

