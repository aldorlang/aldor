
#include "dbg_defs"

------------------------------------------------------------------------
-- Abbreviations to keep signatures on one line.

SI  ==> SingleInteger;
PTR ==> Pointer;
STR ==> String;
TYP ==> Type;
CON ==> CallContext;

------------------------------------------------------------------------

-- Callback funtion in the FOAM interpreter

import
{
	fintDebugQuery: (PTR, SI, SI, STR) -> SI;
} from Foreign;

------------------------------------------------------------------------

UserInterface: with
{
	interact: (DebuggerState, String, CON) -> ();
		++ Enter the user-interaction loop if necessary.

	showDetails: (DebuggerState, String, CON, Boolean) -> ();
		++ showDetails(state, msg, con, indent?) writes the
		++ details of the event described by `msg' occurring
		++ in the context `con' to the debug output. If the
		++ indent? argument is true then indentation is used
		++ to highlight the call depth.
} == add
{
	import from SingleInteger;
	import from TextWriter, String, Character;


	-- Simple banner.
	local showBanner(state:DebuggerState):() ==
	{
		import from String;
		local dbout:TextWriter := state.dbOut;
		local dbnewline:Character := state.dbnewline;

		-- Shorthand for large text messages.
		X(s) ==> dbout << s << dbnewline;


		-- Stop if we have already shown the banner.
		state.bannerShown? => return;


		-- Simple title banner.
		X "";
		X "--------------------------------------------------------";
		X "Aldor Runtime Debugger";
                X "v0.60 (22-May-2000), Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).";
		X "v0.70 (05-Dec-2003), ORCCA @ UWO.";
		X "";
		X "Type _"help_" for more information.";
		X "--------------------------------------------------------";
		X "";


		-- Note that the banner has been shown.
		state.bannerShown? := true;
	}


	-- Display help on the interactive features of the debugger.
	local doHelp(state:DebuggerState, cmds:List String):() ==
	{
		import from HelpPackage(state.dbOut);


		-- Use the stand-alone debug help system.
		help(cmds);
	}


	-- Display a simple stack trace.
	local doWhere(state:DebuggerState):() ==
	{
		--import from JustifyPackage(SingleInteger);
		import from CallContext, String;

		local depth:SingleInteger;
		local dbout:TextWriter := state.dbOut;
		local dbnewline: Character := state.dbnewline;

		-- How deep are we?
		depth := #(state.callStack);


		-- Display the stack.
		for con in reverse(state.callStack) repeat
		{
			--dbout << justify(depth, 3, right);
			dbout << depth;
			dbout << ": ";
			--dbout << format(con, 78 - 3 - 2) << dbnewline;
			dbout << con << dbnewline;
			depth := depth - 1;
		}
	}


	-- Handle display or change of interaction mode.
	local doInteractive(state:DebuggerState, cmds:List String):() ==
	{
		local option:String;
		local dbout:TextWriter := state.dbOut;
		local dbnewline: Character := state.dbnewline;

		local syntax():() ==
		{
			-- Syntax error.
			dbout << "*** Syntax error: ";
			dbout << "int [on|off]";
			dbout << dbnewline;
		}


		-- Decide what to do.
		if (#cmds < 2) then
		{
			-- No arguments means show interaction.
			if (#cmds = 1) then

			{
				-- Pull off the command option.
				option := first cmds;


				-- Process the option: changing to
				-- interactive mode turns off verbose
				-- mode. Changing to non-interactive
				-- mode turns off high-verbosity.
				if (option = "on") then
				{
					state.interactive? := true;
					state.verbose?     := false;
				}
				else if (option = "off") then
				{
					state.interactive? := false;
					state.verbose?     := true;
					state.veryVerbose? := false;
				}
				else
				{
					-- Syntax message and return.
					syntax();
					return;
				}
			}


			-- Show the interactive mode setting.
			dbout << "Interactive mode is ";
			dbout << (state.interactive? => "on"; "off");
			dbout << "." << dbnewline;
		}
		else
			syntax();
	}


	-- Handle display or change of verbosity.
	local doVerbose(state:DebuggerState, cmds:List String):() ==
	{
		local option:String;
		local dbout:TextWriter := state.dbOut;
		local dbnewline: Character := state.dbnewline;

		local syntax():() ==
		{
			-- Syntax error.
			dbout << "*** Syntax error: ";
			dbout << "verbose [on|off|high|low]";
			dbout << dbnewline;
		}


		-- Decide what to do.
		if (#cmds < 2) then
		{
			-- No arguments means show verbosity.
			if (#cmds = 1) then
			{
				-- Pull off the command option.
				option := first cmds;


				-- Process the option.
				if (option = "on") then
					state.verbose? := true;
				else if (option = "off") then
					state.verbose? := false;
				else if (option = "high") then
					state.veryVerbose? := true;
				else if (option = "low") then
					state.veryVerbose? := false;
				else
				{
					-- Syntax message and return.
					syntax();
					return;
				}

			}


			-- Show the verbosity setting.
			dbout << "Verbose mode is ";
			dbout << (state.verbose? => "on"; "off");
			dbout << " (";
			dbout << (state.veryVerbose? => "high"; "low");
			dbout << ")." << dbnewline;
		}
		else
			syntax();
	}


	-- Deleting of breakpoints.
	local doDelete(state:DebuggerState, cmds:List String):() ==
	{
		import from BreakPointPackage;
		delete(state, cmds);
	}


	-- Disabling of breakpoints.
	local doDisable(state:DebuggerState, cmds:List String):() ==
	{
		import from BreakPointPackage;
		disable(state, cmds);
	}


	-- Enabling of breakpoints.
	local doEnable(state:DebuggerState, cmds:List String):() ==
	{
		import from BreakPointPackage;
		enable(state, cmds);
	}

	-- Addition and display of breakpoints.
	local doBreakpoint(state:DebuggerState, cmds:List String, con:CON):() ==
	{
		import from BreakPointPackage, BreakPoint;
		import from String;

		local dbout:TextWriter := state.dbOut;		
		local dbnewline:Character := state.dbnewline;
		
		-- No arguments is easy: display breakpoints.


		(#cmds = 0) => show(state);
		
		-- Creation of breakpoints.
		create(state, cmds, con);
	}


	-- Handle interactions with the user (called internally).
	local gLoop(state:DebuggerState, prompt:String, con:CON):() ==
	{
		-- We assume that the caller has checked to see if
		-- we are already in a debug handler and whether or
		-- not they want interaction with the user. We read
		-- and evaluate simple commands until the user tells
		-- us that they have had enough.
		import from DebugUtilities, List String;

		local cmd:String;
		local cmds:List String;
		local inStream:TextReader;
		local dbout:TextWriter := state.dbOut;
		local dbnewline: Character := state.dbnewline;

		-- Read from standard input.
		inStream := stdin;

		-- Loop until we are told to stop.
		repeat
		{
			-- Emit a prompt and read the command.
			dbout << prompt;
			input := readline! inStream;

			-- If there isn't a newline then the user
			-- type the EOF character. We use that as
			-- a signal to terminate the session.
			(#input = 0) => error("<Quit>");

			-- Split the command into words
			cmds := words(input);

			(fst:String, rst:String) := split(input);

			--dbout << fst << "; " << rst << dbnewline;

#if NoCommandWaits
			-- If no commands, try again.
			if (empty? cmds) then iterate;
#else
			-- If no commands then repeat last step/cont/next.
			(empty? cmds) => return;
#endif

			-- Decide what to do based on the first
			-- four letters of the first word of the
			-- command line entered by the user.
			cmd := substring(first cmds, 0, 4);


			-- Do they want to continue execution?
			(cmd = "cont") =>
			{
#if DEBUG__SET__ENUMERATE
				state.debugAction := ContAction;
#else
				state.conting? := true;
#endif
				return;
			}


			-- Move to the next statement stepping over calls?
			(cmd = "next") =>
			{
#if DEBUG__SET__ENUMERATE
				state.debugAction := NextAction;
#else
				state.nexting? := true;
#endif
				state.nextContext := con;
				return;
			}


			-- Move to the next statement entering calls?
			(cmd = "step") =>
			{
#if DEBUG__SET__ENUMERATE
				state.debugAction := StepAction;
#else
				state.stepping? := true;
#endif
				return;
			}


			-- Perhaps terminate the program?
			(cmd = "halt") => error("<Quit>");
			(cmd = "quit") => error("<Quit>");


			-- They could disable the debugger.
			(cmd = "off") =>
			{
				-- Hack because we can't call stop() yet.
				state.enabled? := false;
				return;
			}


			-- How about some help?
			if (cmd = "help") then
			{
				doHelp(state, rest cmds);
				iterate;
			}


			-- They can even see where they are.
			if (cmd = "wher") then
			{
				doWhere(state);
				iterate;
			}


			-- Are they asking or changing the verbosity?
			if (cmd = "verb") then
			{
				doVerbose(state, rest cmds);
				iterate;
			}


			-- Breakpoint handling.
			if (cmd = "brea") then
			{
				doBreakpoint(state, rest cmds, con);
				iterate;
			}


			-- Breakpoint removal.
			if ((cmd = "del") or (cmd = "dele")) then
			{
				doDelete(state, rest cmds);
				iterate;
			}


			-- Breakpoint enabling.
			if ((cmd = "ena") or (cmd = "enab")) then
			{
				doEnable(state, rest cmds);
				iterate;
			}


			-- Breakpoint disabling.
			if ((cmd = "dis") or (cmd = "disa")) then
			{
				doDisable(state, rest cmds);
				iterate;
			}


			-- Are they asking or changing the interactive mode?
			if ((cmd = "int") or (cmd = "inte")) then
			{
				doInteractive(state, rest cmds);
				iterate;
			}

			-- Print the value of a variable
			--if (((cmd = "pri") or (cmd = "prin")) and (#cmds > 1)) then
			if ((fst = "print") and (#rst > 0)) then
			{
				--local isks:List IntStack := state.intStackChain;
				local isk: IntStack := state.intStack;
				import from Timer;
				local t: Timer;
				local x: MachineInteger;
				x := read(t);
				start!(t);
				fintDebugQuery(isk pretend PTR, 0, con.line, rst);
				stop!(t);
				stdout << "Running time of the query command is " << (read(t) - x) << newline;
				iterate;
			}


			-- Update the value of a variable
			if ((fst = "update") and (#rst > 0)) then
			{
				local isku:IntStack := state.intStack;
				--local isku: IntStack := first state.intStackChain;			

				fintDebugQuery(isku pretend PTR, 1, con.line, rst);

				iterate;
			}

			-- Shell command
			if ((fst = "shell") and (#rst > 0)) then
			{
				--local isku:IntStack := state.intStack;
				--local isku: IntStack := first state.intStackChain;			

				fintDebugQuery(0 pretend PTR, 2, con.line, rst);

				iterate;
			}

			-- Unrecognised command.
			dbout << "Command _"";
			dbout << cmd << "_" not recognised.";
			dbout << dbnewline;


			-- Debugging: display the commands.
			dbout << "[ ";
			for word in cmds repeat
				dbout << "_"" << word << "_" ";
			dbout << "]" << dbnewline;
		}
	}


	-- Display the text for a specific event.
	showDetails(state:DebuggerState, event:String, context:CON, indent?:Boolean):() ==
	{
		--import from JustifyPackage(String);

		local dbout:TextWriter := state.dbOut;


		-- Add some indentation if required.
		if (indent?) then
		{
			-- We use the value of stackDepth because the 
			-- context may be empty.
			for i in 1..(state.stackDepth - 1) repeat
				dbout << "   ";
		}


		-- Right justify the event tag.
		--dbout << justify("[" + event + "]", 8, right);
		dbout << "[" << event << "]";


		-- Display the calling context.
		dbout << " " << context;
	}


	-- Display the source code associated with a specific event.
	local showSource(state:DebuggerState, con:CON):() ==
	{
		--import from JustifyPackage(String), TextWriter;
		--import from JustifyPackage(SingleInteger);
		import from TextWriter;

		local ftext:String;
		local fidx:TextFileIndex;
		local fname:String       := con.file;
		local fline:SI           := con.line;
		local dbout:TextWriter   := state.dbOut;
		local dbnewline: Character := state.dbnewline;
		local ftab:TextFileTable := state.fileTable;
		local fil:String         := fname;


		-- Search for the file name.
		fidx := ftab.fname;


		-- Did we find anything?
		if (nil? fidx) then
		{
			-- No: create a new index.
			fidx := new(fil);


			-- Store this index in the file table.
			ftab.fname := fidx;
		}


		-- Get the source code associated with this file/line
		-- using the index we just found or created. This may
		-- update the index with this line for future access.
		ftext := readline(fil, fidx, fline);

		dbout << fline << " " << ftext;

		-- Right justify the line number
		--dbout << justify(fline, 6, left);

		-- Display the source line.
		--dbout << " " << justify(ftext, 78, left);
	}


	-- Decide whether or not to enter the user-interaction loop.
	interact(state:DebuggerState, prompt:String, con:CON):() ==
	{
		import from BreakPointPackage;

		local dbout:TextWriter := state.dbOut;
		local dbnewline: Character := state.dbnewline;
		local bpoint:BreakPoint;
		local brk?, stop?:Boolean;


		-- If we are running non-interactively then we never
		-- enter the interactive loop.
		(not state.interactive?) => return;


		-- See if we can find a breakpoint here.
		bpoint := breakpoint(state, con);


		-- Have we hit a breakpoint?
		brk? := not nil? bpoint;


		-- If we hit a breakpoint, increment its count and
		-- display some details about it. At the moment we
		-- assume that there is only one breakpoint per line.
		if (brk?) then
		{
			-- Increment the hit count.
			hit! bpoint;


			-- Display its details.
			dbout << "Hit breakpoint #" << (number bpoint);
			dbout << " " << con << dbnewline;
		}


#if DEBUG__SET__ENUMERATE
		-- Do we need to stop for some reason?
		if (not state.bannerShown?) then
		{
			-- This is our first event and we want to
			-- greet the user and let them configure
			-- their settings/set breakpoints etc.
			stop?  := true;
		}
		else if (brk?) then
		{
			-- Stop because we hit a breakpoint.
			stop?  := true;
		}
		else if (state.debugAction = NextAction) then
		{
			-- Only stop if context same as stored one.
			stop?  := (con = state.nextContext);
		}
		else
		{
			-- Otherwise only stop if stepping.
			stop?  := (state.debugAction = StepAction);
		}
#else
		-- Do we need to stop for some reason?
		if (not state.bannerShown?) then
		{
			-- This is our first event and we want to
			-- greet the user and let them configure
			-- their settings/set breakpoints etc.
			stop?  := true;
		}
		else if (brk?) then
		{
			-- Stop because we hit a breakpoint.
			stop?  := true;
		}
		else if (state.nexting?) then
		{
			-- Only stop if context same as stored one.
			stop?  := (con = state.nextContext);
		}
		else
		{
			-- Otherwise only stop if stepping.
			stop?  := state.stepping?;
		}
#endif


		-- Do we need to stop?
		if (stop?) then
		{
			-- Make sure that the banner has been shown.
			showBanner(state);


			-- Display the context if we have just entered it
			-- (or just left one and returned to another).
			dbout << con << dbnewline;


			-- Display the source code.
			showSource(state, con);
			dbout << dbnewline;


			-- Enter the interactive loop.
			gLoop(state, prompt, con);
		}
	}
}

