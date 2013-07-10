
#include "dbg_defs"

------------------------------------------------------------------------
---------- IMPORTANT: NEVER COMPILE THIS FILE WITH -Wdebugger ----------
------------------------------------------------------------------------

--+
-- Copyright:
--    (c) Aldor.org 1990-2007
--
-- Authors:
--    Martin N Dunstan (martin@nag.co.uk)
--
-- Notes:
--    The idea of this package is to provide a one-stop-shop for all our
--    debugging needs. This package intends to keep track of all the data
--    about code compiled with -Wdebugger: this information can be shared
--    with other debugging packages or profilers if required. To use these
--    facilities programs must use start() when they are ready to begin
--    the debugging phase. This may occur as the first statement of the
--    program or inside a single function known to be functioning badly.
--
-- Conventions:
--    Some of the domain interfaces may seem somewhat strange at first
--    with a mix of functional and record accessors. There is reason in
--    this apparent madness though:
--     * constant properties for domain values are normally only accessed
--       via a suitably-named function. For example, the name of the
--       function for a given CallContext can be obtained via name().
--     * mutable properties are normally accessed and updated via record
--       style methods. For example, the current line number associated
--       with a CallContext can be read via `context.line' and can be
--       changed via `context.line := newvalue'.
--
--    Avoid using domains such as HashTable(BreakPoint, BreakPoint) as we
--    do in dbg_state.as. Instead we ought to define a new domain called
--    BreakPointTable whose rep is HashTable(). Not only is this good ADT
--    practice but it helps to push library-specific details down to the
--    lowest possible level.
--
-- Porting:
--    Using other base libraries may be tricky. I have tried hard to make
--    the library compile under both Axllib and Basicmath with as few #if
--    statements as possible. Please try to ensure that any library-specific
--    issues are kept in dbg_specifics rather than polluting the rest of
--    the library. This file also shows you how to deal with differences
--    between libraries: sometimes it may be necessary to define a new
--    domain whose implementation is completely library-dependent. This is
--    okay and shields the rest of the library from the details. See the
--    TextFileTable domain for an example of this.
--
-- Future work:
--   * Try building under Basiclib instead of Axllib.
--   * The debuglib.as header file ought to be split into two: one version
--     will be used to build the library (with macros) while the other will
--     be installed in $ALDORROOT/include, $BMROOT/include or wherever.
--   * The type passed to the "enter" event ought to be the return type
--     of the function. The compiler generates (0 pretend Type) currently.
--   * Protect important code from exceptions
--   * Provide hooks for notifying other clients about entry/exit events
--     (eg Marc's debugger/trace library
--   * option to display the store status via stoShow()
--   * allow users to display variables. We display all the variables with
--     the specified name in the current scope. The user could use var@T
--     but we filter the results by computing typeName as a text string and
--     performing string equality on T.
--   * support for assignment of other identifiers not just parameters. This
--     needs compiler support first.
--   * Get the compiler to define a special symbol (__DEBUGGING__) when
--     programs are compiled with -Wdebugger. Then we can use #error to
--     drop out if they try -Wdebugger on us.
--   * allow user to execute shell commands
--   * function exit event is associated with the closing } for the
--     function or with a fake line if a single-line function.
--   * allow users to control the verbosity/interactiveness via exports
--     this enables them to start the debugger running silently and only
--     dropping into interactive mode after a certain amount of computation
--     has taken place.
--   * when running under the interpreter we ought to be able to access
--     functions to execute Aldor code from within the debugger and thus
--     find out more about the current program state.
--   * live dangerously: use exceptions instead of nil()/nil?().
--   * define BreakPointTable to replace HashTable(BreakPoint, BreakPoint)
--     in dbg_state.as. Repeat this with the 'callStack' and 'breakList'
--     components of the internal state.
--
-- History:
--   09-May-2000: (mnd)
--     Created.
--   22-May-2000: (mnd)
--     Frozen at version 0.60
--   {add_changes_here}
--
-- Bugs:
--   {list_bugs_here}
---


------------------------------------------------------------------------
-- Abbreviations to keep signatures on one line.

SI  ==> SingleInteger;
PTR ==> Pointer;
STR ==> String;
TYP ==> Type;
CON ==> CallContext;
ISK ==> IntStack;

------------------------------------------------------------------------
-- The parameter types are not needed outside this file.

DbgDepthLocal	==> (-1@SI);	-- (Loc ...)
DbgDepthParam	==> (-2@SI);	-- (Par ...)
DbgDepthFake	==> (-3@SI);	-- Argument to next call
DbgDepthGlobal	==> (-4@SI);	-- (Glo ...)
DbgDepthFluid	==> (-5@SI);	-- Fluid variable

------------------------------------------------------------------------
-- The debug event enumeration type is not needed outside this file.
-- Debug event types (see runtime.as or foam_c.h for details).

local FiDbgTag;

FiDbgTag ==
	'
		DbgInit,
		DbgEnter,
		DbgInside,
		DbgReturn,
		DbgExit,
		DbgAssign,
		DbgThrow,
		DbgCatch,
		DbgStep,
		DbgCall,
		DbgIntEnter,
		DbgIntStep,
		DbgIntExit,
		DbgIntQuery
	';

------------------------------------------------------------------------
-- Signatures of the runtime debugger hooks (see runtime.as). They are
-- listed here purely for reference.

DbgEnterSIG  ==> ((STR, SI, TYP, STR, TYP, SI) -> CON);
DbgInitSIG   ==> ((STR, SI, TYP, STR, TYP, SI) -> CON);
DbgInsideSIG ==> (CON -> ());
DbgReturnSIG ==> ((STR, SI, CON, T:TYP, T) -> ());
DbgExitSIG   ==> ((STR, SI, CON) -> ());
DbgAssignSIG ==> ((STR, SI, CON, STR, T:TYP, T, SI, SI) -> ());
DbgThrowSIG  ==> ((STR, SI, CON, TYP) -> ());
DbgCatchSIG  ==> ((STR, SI, CON, TYP) -> ());
DbgCallSIG   ==> ((STR, SI, CON, STR, TYP, TYP, SI) -> ());
DbgStepSIG   ==> ((STR, SI, CON) -> ());


------------------------------------------------------------------------
-- Interface to the runtime debugger hooks.

import
{
	fiGetDebugger: FiDbgTag -> PTR;
	fiSetDebugger: (FiDbgTag, PTR) -> ();

	-- Callback funtion in the FOAM interpreter
	fintDebugQuery: (PTR, SI, SI, STR) -> SI;

} from Foreign;


------------------------------------------------------------------------
-- We want to have state shared across all domains created by the
-- NewDebugPackage constructor.

local state:DebuggerState := new();

------------------------------------------------------------------------
-- Best not add-inherit from this package. Also don't try parameterising
-- the package as a compiler bug will cause us to lose track of the
-- free state variable defined above.

+++ NewDebugPackage provides a primitive low-level debugging system
+++ for Aldor programs. It can be used with stand-alone executables
+++ or interpreted FOAM applications; it may be interactive or not
+++ as the user desires. It may be used to single-step through an
+++ entire application from the beginning, down through every library
+++ function, until the end. Alternatively it may be enabled for
+++ individual functions which need to be examined more closely.
NewDebugPackage: with
{
	stopped?: () -> Boolean;
		++ Returns true if debugging is disabled.

	paused?: () -> Boolean;
		++ Returns true if debugging is paused.

	pause!: () -> ();
		++ Pauses the debugger so that it no longer processes
		++ any events. Use unpause! to request the debugger to
		++ restart event processing. The processing will only
		++ resume if the number of unpause! calls executed is
		++ equal to, or greater than, the number of pause!
		++ calls and not stopped?().

	unpause!: () -> ();
		++ Undoes the action of the previous pause! call. If
		++ the number of unpause! calls is equal to, or greater
		++ than the number of pause! calls then debugging events
		++ will continue to be processed (unless stopped?()).

	start!: () -> ();
		++ Enable and start interactive debugging.

	end!: () -> ();
		++ Call uninitialising function in Interpreter.

	trace!: (steps?:Boolean == false) -> ();
		++ Enable and start non-interactive debugging
		++ (indented trace listing of procedure calls).
		++ If steps? is true then each program line
		++ executed will also be shown (step events).
		++ This defaults to false if omitted.

	stop!:  () -> ();
		++ Disable debugging.

	setOutput!: TextWriter -> TextWriter;
		++ Set the debug output stream.
	

} == add
{
	import from SingleInteger, Boolean;
	import from Machine, String,WriterManipulator;

	local querying?:Boolean := false;

	-- Ensure that the call stack is sane.
	local reconcileStack(state:DebuggerState):() ==
	{
		local got, pop:SingleInteger;


		-- If the stack depth is K then there ought to be exactly
		-- K elements on the stack. The times when this may not
		-- hold are when we hit an exception without a context or
		-- returned from a function without a context.
		got := #(state.callStack);
		pop := got - state.stackDepth;


		-- Anything to do?
		(pop < 1) => return;


		-- Pop call records to restore call stack to its
		-- correct state.
		for i in 1..pop repeat
			state.callStack := rest state.callStack;
	}


	-- Update a function parameter.
	local doParam(con:CON, n:String, T:Type, v:T, c:SI):() ==
	{
		local par:Parameter;
		local fnparams:Array(Parameter);


		c:= c-1; -- Array index from 0 in aldorlib rather than from 1 in axllib;

		-- Extract the parameter array.
		fnparams := con.parameters;

		--import from TextWriter, String, Character;

		-- Check that we have a valid parameter number.
		(not((0 <= c) and (c <= #fnparams-1))) => return;

		-- Get the parameter value.
		par := fnparams.c;


		-- Create a new parameter if not initialised.
		if (nil? par) then
			par := new(n, T, c);

		-- Set the value of the parameter.
		fnparams.c := update(par, T, v);
	}


	-- Is it safe to continue processing a debugging event?
	local unsafe?():Boolean ==
	{
		-- Control flags and other information.
		free state:DebuggerState;


		-- Unsafe if already processing an event.
		state.debugging? => true;


		-- Unsafe if not registered yet?
		not state.registered? => true;


		-- If stopped or paused then we are unsafe.
		(stopped?() or paused?()) => true;


		-- Looks okay.
		false;
	}


	-- Handler for function-entry events.
	local dEnter(fil:STR, lin:SI, xp:TYP, nam:STR, T:TYP, argc:SI):CON ==
	{
		-- Return value.
		local con:CON := nothing();


		-- Control flags and other information.
		free state:DebuggerState;


		-- If unsafe then we do nothing.
		unsafe?() => con;


		-- Prevent re-entry.
		state.debugging? := true;


		-- Note that we now have an extra function call active.
		state.stackDepth := state.stackDepth + 1;


		-- Create a context for this call.
		con := context(nam, T, xp, state.stackDepth, argc);


		-- Set the filename and line number.
		con.file := fil;
		con.line := lin;


		-- Add this call to the stack.
		state.callStack := cons(con, state.callStack);


		-- Note that we just entered a function.
		state.entered? := true;


		-- Safe for re-entry.
		state.debugging? := false;
		con;
	}


	-- Handler for function-inside events.
	local dInside(con:CON):() ==
	{
		import from UserInterface;


		-- Control flags and other information.
		free state:DebuggerState;


		-- If unsafe then we do nothing.
		unsafe?() => return;


		-- No point in continuing if no context.
		nothing? con => return;


		-- Prevent re-entry.
		state.debugging? := true;


		-- Display the event details: the function entry event
		-- has been processed (which gave us this context) and
		-- all the parameters have been passed to us via the
		-- assign event. Now we can display the entry event in
		-- its full glory.
		if (state.verbose?) then
		{
			showDetails(state, "enter", con, ~state.interactive?);
			(state.dbOut) << endnl;
		}


		-- Safe for re-entry.
		state.debugging? := false;
	}


	-- Handler for function-return events.
	local dReturn(fil:STR, lin:SI, con:CON, T:TYP, val:T):() ==
	{
		import from DomainName, TypePackage;
		import from UserInterface;


		-- Control flags and other information.
		free state:DebuggerState;
		free querying?;

		local dbout:TextWriter := state.dbOut;


		-- If unsafe then we do nothing.
		unsafe?() => return;


		-- No point in continuing if no context.
		nothing? con => return;


		-- Prevent re-entry.
		state.debugging? := true;


		-- Update the filename and line number.
		con.file := fil;
		con.line := lin;


		-- Get the stack depth of this call from the context.
		state.stackDepth := depth(con);


		-- Ensure the call stack is sane.
		reconcileStack(state);


		-- Do we need to show the user this event?
		if (state.verbose?) then
		{
			-- Display the event text.
			showDetails(state, "return", con, ~state.interactive?);


			-- Primitive details
			dbout << " = ";


			-- Is the result printable?
			if (zero?(T pretend SI)) then
				dbout << "??? : ???";
			else if (T has BasicType) then
				dbout << val << ":" << typeName(T);
			else
				dbout << "???" << ":" << typeName(T);


			-- Finish the line.
			dbout << endnl;
		}


		-- Interactive command loop: allow the user to
		-- find out about the return value if they want.
		querying? := true;
		interact(state, "(debug) ", con);
		querying? := false;

		-- Reduce the stack depth to the level of the caller
		-- if we haven't done so already.
		state.stackDepth := state.stackDepth - 1;


		-- Ensure the call stack is sane.
		reconcileStack(state);


		-- Note that we just entered the calling function.
		state.entered? := true;


		-- Safe for re-entry.
		state.debugging? := false;
	}


	-- Handler for function-exit events.
	local dExit(fil:STR, lin:SI, con:CON):() ==
	{
		import from UserInterface;


		-- Control flags and other information.
		free state:DebuggerState;
		free querying?;

		-- If unsafe then we do nothing.
		unsafe?() => return;


		-- No point in continuing if no context.
		nothing? con => return;


		-- Prevent re-entry.
		state.debugging? := true;


		-- Update the filename and line number.
		con.file := fil;
		con.line := lin;


		-- Get the stack depth of this call from the context.
		state.stackDepth := depth(con);


		-- Ensure the call stack is sane.
		reconcileStack(state);


		-- Display the event text.
		if (state.verbose?) then
		{
			showDetails(state, "exit", con, ~state.interactive?);
			(state.dbOut) << endnl;
		}


		-- Interactive command loop.
		querying? := true;
		interact(state, "(debug) ", con);
		querying? := false;

		-- Reduce the stack depth to the level of the caller
		-- if we haven't done so already.
		state.stackDepth := state.stackDepth - 1;


		-- Ensure the call stack is sane.
		reconcileStack(state);


		-- Note that we just entered the calling function.
		state.entered? := true;


		-- Safe for re-entry.
		state.debugging? := false;
	}


	local dAssign(fil:STR, lin:SI, con:CON, n:STR, T:TYP, v:T, d:SI, c:SI):() ==
	{
		import from DomainName;
		import from UserInterface;


		-- Control flags and other information.
		free state:DebuggerState;

		local dbout:TextWriter := state.dbOut;


		-- If unsafe then we do nothing.
		unsafe?() => return;


		-- No point in continuing if no context.
		nothing? con => return;


		-- Prevent re-entry.
		state.debugging? := true;


		-- Update the filename and line number.
		con.file := fil;
		con.line := lin;


		-- Get the stack depth for this call.
		state.stackDepth := depth(con);


		-- Ensure the call stack is sane.
		reconcileStack(state);


		-- Update the local copy of this identifier.
		if (d = DbgDepthParam) then
			doParam(con, n, T, v, c);


		-- Safe for re-entry.
		state.debugging? := false;
	}


	-- Handler for single-stepping events
	local dStep(fil:STR, lin:SI, con:CON):() ==
	{
		import from UserInterface;
		free querying?;

		-- Control flags and other information.
		free state:DebuggerState;


		-- If unsafe then we do nothing.
		unsafe?() => return;


		-- No point in continuing if no context.
		nothing? con => return;


		-- Prevent re-entry.
		state.debugging? := true;


                --if (nothing? con = false) then {

		-- Update the filename and line number.
		con.file := fil;
		con.line := lin;


		-- Retrieve the stack depth for this call.
		state.stackDepth := depth(con);


		-- Ensure the call stack is sane.
		reconcileStack(state);


		-- Display the event text if in very verbose mode.
		if (state.verbose? and state.veryVerbose?) then
		{
			showDetails(state, "step", con, ~state.interactive?);
			(state.dbOut) << endnl;
		}
		--}

		-- Interactive command loop.
		querying? := true;
		interact(state, "(debug) ", con);
		querying? := false;

		-- Safe for re-entry.
		state.debugging? := false;
	}


	--Events tied with FOAM Interpreter, Jinlong Cai
	local dIntEnter(con:CON): ISK ==
	{
		local isk: ISK := nothing();

		import from UserInterface;	
		import from List IntStack;	
		import from Array Pointer;

		-- Control flags and other information.
		free state:DebuggerState;

		-- If unsafe then we do nothing.
		unsafe?() => return nothing();

		--local con:CallContext := first state.callStack;

		isk := stack(con.file,con.name,new(0,nil$PTR), new(0,nil$PTR),nil$Pointer);

		if (#state.intStackChain > 0) then
		{
			local current:ISK := first state.intStackChain;
			isk.depth := current.depth + 1;
		}

		state.intStack := isk;

		-- Add this call to the stack.
		state.intStackChain := cons(isk, state.intStackChain);

		-- Safe for re-entry.
		state.debugging? := false;
		isk;
	}


	local dIntStep(typ:SI,val:PTR,con:CON):() ==
	{
		import from ISK;
		import from UserInterface;

		local dbout:TextWriter := state.dbOut;

		local isk: IntStack := state.intStack;

		typ = 0 => setLocals!(isk, val);
		typ = 1 => setParams!(isk, val);
		typ = 2 => isk.env := val;
	
		dbout << "Error in Debug library: Undefined type in stack!" << endnl;  
	} 

	local dIntExit(con:CON): () ==
	{
		if (#state.intStackChain > 0) then
		{
			local isks: List IntStack := rest state.intStackChain;
			state.intStackChain := isks;	
			if (#isks > 0) then
		        	state.intStack := first isks;
		}

	}


	-- Flag to disable debugging system
	local dQuery(): Boolean == 
	{		
		querying?;
	}

	-- Dummy handlers: proper ones not implemented yet.
	local dInit(f:STR, l:SI, e:TYP, n:STR, t:TYP, c:SI):CON == nothing();
	local dThrow(f:STR, l:SI, x:CON, e:TYP):() == {}
	local dCatch(f:STR, l:SI, x:CON, e:TYP):() == {}
	local dCall(f:STR, l:SI, x:CON, n:STR, e:TYP, t:TYP, c:SI):() == {}


	-- Register the debugging system.
	local register():() ==
	{
		import from FiDbgTag;

		free state:DebuggerState;


		-- We only want to register once.
		state.registered? => return;
	
		--state.dbOut << "Registering debug hooks ... " << endnl;

		-- Install our handlers.
		fiSetDebugger(DbgInit, dInit pretend PTR);
		fiSetDebugger(DbgEnter, dEnter pretend PTR);
		fiSetDebugger(DbgInside, dInside pretend PTR);
		fiSetDebugger(DbgReturn, dReturn pretend PTR);
		fiSetDebugger(DbgExit, dExit pretend PTR);
		fiSetDebugger(DbgAssign, dAssign pretend PTR);
		fiSetDebugger(DbgThrow, dThrow pretend PTR);
		fiSetDebugger(DbgCatch, dCatch pretend PTR);
		fiSetDebugger(DbgCall, dCall pretend PTR);
		fiSetDebugger(DbgStep, dStep pretend PTR);

                -- Tied with FOAM Interpreter, Jinlong Cai
                fiSetDebugger(DbgIntEnter, dIntEnter pretend PTR);
		fiSetDebugger(DbgIntStep, dIntStep pretend PTR);
		fiSetDebugger(DbgIntExit, dIntExit pretend PTR);
                fiSetDebugger(DbgIntQuery, dQuery pretend PTR);

		-- We are now activated.
		state.registered? := true;
	}


	-- Have we stopped?
	stopped?():Boolean ==
	{
		free state:DebuggerState;

		not state.enabled?;
	}


	-- Have we paused?
	paused?():Boolean ==
	{
		import from SingleInteger;
		free state:DebuggerState;

		(state.pauseLevel) > 0;
	}


	-- Pause the debugger.
	pause!():() ==
	{
		import from SingleInteger;
		free state:DebuggerState;


		-- Increment the pause count.
		state.pauseLevel := state.pauseLevel + 1;
	}


	-- Start the interactive system, registering it if necessary.
	start!():() ==
	{
		free state:DebuggerState;


		-- Do nothing if already enabled.
		state.enabled? => return;


		-- Ensure that we are activated.
		register();


		-- We need non-verbose, interactive mode.
		state.verbose?     := false;
		state.interactive? := true;
		state.stepping?    := true;


		-- Make sure that we aren't paused.
		state.pauseLevel := 0;


		-- We are now enabled and ready to go.
		state.enabled? := true;
	}


	--Call fintFini in Interpreter
	--Has not used below function
	end!():() == 
	{
		import from PTR, STR;
		fintDebugQuery(nil$PTR, 3, 0, new(0));	
	}


	-- Start the non-interactive system, registering it if necessary.
	trace!(steps?:Boolean):() ==
	{
		free state:DebuggerState;


		-- Do nothing if already enabled.
		state.enabled? => return;


		-- Ensure that we are activated.
		register();


		-- We need verbose, non-interactive mode. We may
		-- be asked to show step events.
		state.verbose?     := true;
		state.veryVerbose? := steps?;
		state.interactive? := false;


		-- Make sure that we aren't paused.
		state.pauseLevel := 0;


		-- We are now enabled and ready to go.
		state.enabled? := true;
	}


	-- Stop the debugger.
	stop!():() ==
	{
		free state:DebuggerState;


		-- Do nothing if not enabled.
		not state.enabled? => return;


		-- We are now disabled (but still receiving events)
		state.enabled? := false;
	}


	-- Un-pause the debugger.
	unpause!():() ==
	{
		free state:DebuggerState;
		import from SingleInteger;

		-- Do nothing if not paused.
		(not paused?()) => return;


		-- Decrement the pause count.
		state.pauseLevel := state.pauseLevel - 1;
	}


	-- Change the output stream.
	setOutput!(tw:TextWriter):TextWriter ==
	{
		free state:DebuggerState;


		-- Save the old textwriter.
		local result:TextWriter := state.dbOut;


		-- Change to the new one.
		state.dbOut := tw;


		-- Return the previous output stream.
		result;
	}
}

