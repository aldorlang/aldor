
#include "dbg_defs"


SI  ==> SingleInteger;
CON ==> CallContext;
BP  ==> BreakPoint;


#if DEBUG__SET__ENUMERATE
-- This is commented out because although we can define the export:
--    set!: (%, 'debugAction', DebuggerAction) -> DebuggerAction;
-- when we try to apply (state.StepAction := ...) we get a runtime
-- error looking for set! with code 897888711.

+++ Possible actions used by the debugger to control when it
+++ enters the interactive loop.
DebuggerAction == 'StepAction,ContAction,NextAction';
#endif



+++ DebuggerState is a domain used to represent the internal state of
+++ the debugger. It is safe to use a non-local domain because the
+++ actual state of the debugger will never be released to clients.
DebuggerState:with
{
	new: () -> %;
		++ Returns a new state with sensible defaults.

	apply: (%, 'enabled?') -> Boolean;
		++ true if debugging has been enabled by a client.

	apply: (%, 'registered?') -> Boolean;
		++ true if the debugger has registered with runtime.

	apply: (%, 'debugging?') -> Boolean;
		++ true if we are currently processing a debug event.
		++ Used to prevent the functions here from being traced.

	apply: (%, 'bannerShown?') -> Boolean;
		++ true if we have shown the debugger banner.

#if DEBUG__SET__ENUMERATE
	apply: (%, 'debugAction') -> DebuggerAction;
		++ Return the type of action requested by the last
		++ debugging command. StepAction causes the debugger
		++ to halt at the next step event; ContAction causes
		++ it to continue until a breakpoint is reached and
		++ NextAction causes it to continue until the next
		++ step event occuring in the same context.
#else
	-- Use three boolean flags instead of a ternary value. Setting
	-- one of these to try flips the others to false.
	apply: (%, 'stepping?') -> Boolean;
		++ true if the user used "step".

	apply: (%, 'nexting?') -> Boolean;
		++ true if the user used "next".

	apply: (%, 'conting?') -> Boolean;
		++ true if the user used "cont".
#endif

	apply: (%, 'entered?') -> Boolean;
		++ true if we have entered a function but not
		++ told the user about it.

	apply: (%, 'nextContext') -> CallContext;
		++ Return the context associated with NextAction.

	apply: (%, 'intStack') -> IntStack;
                ++ Return the Interpreter Stack 

	apply: (%, 'verbose?') -> Boolean;
		++ If true then details of each event is written to
		++ the debug output stream. This output is indented
		++ according to the stack depth to assist the reader.

	apply: (%, 'veryVerbose?') -> Boolean;
		++ If true then step-events are written to the debug
		++ stream.

	apply: (%, 'interactive?') -> Boolean;
		++ If false then we never drop into the debug loop and
		++ we display an indented trace.

	apply: (%, 'stackDepth') -> SingleInteger;
		++ number of function calls yet to finish executing.

	apply: (%, 'callStack') -> List CallContext;
		++ current list of active function calls.

	apply: (%, 'intStackChain') -> List IntStack;
		++ current list of active program calls.

	apply: (%, 'breakTable') -> HashTable(BreakPoint, BreakPoint);
		++ table of breakpoints used to check whether a given
		++ source code position has a breakpoint set.

	apply: (%, 'breakList') -> Array(BreakPoint);
		++ breakpoints ordered by ID number.

	apply: (%, 'dbOut') -> TextWriter;
		++ debug output stream

	apply: (%, 'dbnewline') -> Character;
		++ newline character

	apply: (%, 'fileTable') -> TextFileTable;
		++ Line number table.

	apply: (%, 'pauseLevel') -> SingleInteger;
		++ Pause level: not paused when level is zero.


	-- Update functions: note the ommission of set! for breakTable
	-- and fileTable. This is because the tables are created once
	-- and never changed: we always update-in-place.
	set!: (%, 'enabled?', Boolean) -> Boolean;
	set!: (%, 'registered?', Boolean) -> Boolean;
	set!: (%, 'debugging?', Boolean) -> Boolean;
	set!: (%, 'bannerShown?', Boolean) -> Boolean;
#if DEBUG__SET__ENUMERATE
	set!: (%, 'debugAction', DebuggerAction) -> DebuggerAction;
#else
	-- Use three boolean flags instead of a ternary value. Setting
	-- one of these to try flips the others to false.
	set!: (%, 'stepping?', Boolean) -> Boolean;
	set!: (%, 'nexting?', Boolean) -> Boolean;
	set!: (%, 'conting?', Boolean) -> Boolean;
#endif
	set!: (%, 'entered?', Boolean) -> Boolean;
	set!: (%, 'nextContext', CallContext) -> CallContext;
        set!: (%, 'intStack', IntStack) -> IntStack;
	set!: (%, 'verbose?', Boolean) -> Boolean;
	set!: (%, 'veryVerbose?', Boolean) -> Boolean;
	set!: (%, 'interactive?', Boolean) -> Boolean;
	set!: (%, 'stackDepth', SingleInteger) -> SingleInteger;
	set!: (%, 'callStack', List CON) -> List CON;
	set!: (%, 'intStackChain', List IntStack) -> List IntStack;
	set!: (%, 'breakList', Array(BP)) -> Array(BP);
	set!: (%, 'dbOut', TextWriter) -> TextWriter;
	set!: (%, 'pauseLevel', SingleInteger) -> SingleInteger;

	export from List(CON);
#if DEBUG__SET__ENUMERATE
	export from DebuggerAction;
#endif
	export from Array(BreakPoint);
	export from HashTable(BreakPoint, BreakPoint);

	export from 'enabled?';
	export from 'registered?';
	export from 'debugging?';
	export from 'bannerShown?';
#if DEBUG__SET__ENUMERATE
	export from 'debugAction';
#else
	export from 'stepping?', 'nexting?', 'conting?';
#endif
	export from 'entered?';
	export from 'nextContext';
        export from 'intStack';
	export from 'verbose?';
	export from 'veryVerbose?';
	export from 'interactive?';
	export from 'stackDepth';
	export from 'callStack';
	export from 'intStackChain';
	export from 'breakList';
	export from 'breakTable';
	export from 'fileTable';
	export from 'pauseLevel';
	export from 'dbOut';
	export from 'dbnewline';
}
== add
{
	Rep == Record
	(
		ena?:Boolean,
		reg?:Boolean,
		deb?:Boolean,
		ban?:Boolean,

		ver?:Boolean,
		vve?:Boolean,
		int?:Boolean,

#if DEBUG__SET__ENUMERATE
		act:DebuggerAction,
#else
		stp?:Boolean,
		nxt?:Boolean,
		cnt?:Boolean,
#endif
		ent?:Boolean,
		ncon:CallContext,
                intStack:IntStack,

		dept:SingleInteger,
		iskchain:List IntStack,
		call:List CallContext,
		btab:HashTable(BreakPoint, BreakPoint),
		blis:Array(BreakPoint),

		dbtw:TextWriter,
		dbnl: Character,		
		ftab:TextFileTable,
		paws:SingleInteger
	);
	import from Rep;


	-- Macros to reduce the chance of error when wanting to
	-- perform functional-style updates of a single field.
	WRITE(x, f, v)  ==> (rep x).f := v;
	READ(x, f)      ==> (rep x).f;
	MODIFY(x, f, v) ==> {
		local res:% := per record explode rep x;
		WRITE(res, f, v);
		res;
	}


	-- Constructor.
	new():% == per
	[
		false, false, false, false,
		false, false, true,
#if DEBUG__SET__ENUMERATE
		ContAction,
#else
		false, false, true,
#endif
		false, nothing(), nothing(),
		0, empty, empty, table(), empty,
		stdout, newline, new(), 0
	];


	apply(x:%, ignored:'enabled?'):Boolean ==
		READ(x, ena?);

	apply(x:%, ignored:'registered?'):Boolean ==
		READ(x, reg?);

	apply(x:%, ignored:'debugging?'):Boolean ==
		READ(x, deb?);

	apply(x:%, ignored:'bannerShown?'):Boolean ==
		READ(x, ban?);

#if DEBUG__SET__ENUMERATE
	apply(x:%, ignored:'debugAction'):DebuggerAction ==
		READ(x, act);
#else
	apply(x:%, ignored:'stepping?'):Boolean ==
		READ(x, stp?);

	apply(x:%, ignored:'nexting?'):Boolean ==
		READ(x, nxt?);

	apply(x:%, ignored:'conting?'):Boolean ==
		READ(x, cnt?);
#endif

	apply(x:%, ignored:'entered?'):Boolean ==
		READ(x, ent?);

	apply(x:%, ignored:'nextContext'):CallContext ==
		READ(x, ncon);

	apply(x:%, ignored:'intStack'):IntStack ==
		READ(x, intStack);

	apply(x:%, ignored:'verbose?'):Boolean ==
		READ(x, ver?);

	apply(x:%, ignored:'veryVerbose?'):Boolean ==
		READ(x, vve?);

	apply(x:%, ignored:'interactive?'):Boolean ==
		READ(x, int?);

	apply(x:%, ignored:'stackDepth'):SingleInteger ==
		READ(x, dept);

	apply(x:%, ignored:'callStack'):List CallContext ==
		READ(x, call);

	apply(x:%, ignored:'intStackChain'):List IntStack ==
		READ(x, iskchain);

	apply(x:%, ignored:'breakTable'):HashTable(BreakPoint, BreakPoint) ==
		READ(x, btab);

	apply(x:%, ignored:'breakList'):Array(BreakPoint) ==
		READ(x, blis);

	apply(x:%, ignored:'dbOut'):TextWriter ==
		READ(x, dbtw);

	apply(x:%, ignored:'dbnewline'):Character ==
		READ(x, dbnl);

	apply(x:%, ignored:'fileTable'):TextFileTable ==
		READ(x, ftab);

	apply(x:%, ignored:'pauseLevel'):SingleInteger ==
		READ(x, paws);


	set!(x:%, ignored:'enabled?', v:Boolean):Boolean ==
		WRITE(x, ena?, v);

	set!(x:%, ignored:'registered?', v:Boolean):Boolean ==
		WRITE(x, reg?, v);

	set!(x:%, ignored:'debugging?', v:Boolean):Boolean ==
		WRITE(x, deb?, v);

	set!(x:%, ignored:'bannerShown?', v:Boolean):Boolean ==
		WRITE(x, ban?, v);

#if DEBUG__SET__ENUMERATE
	set!(x:%, ignored:'debugAction', v:DebuggerAction):DebuggerAction ==
		WRITE(x, act, v);
#else
	set!(x:%, ignored:'stepping?', v:Boolean):Boolean ==
	{
		WRITE(x, stp?,  v);
		WRITE(x, nxt?, ~v);
		WRITE(x, cnt?, ~v);
	}

	set!(x:%, ignored:'nexting?', v:Boolean):Boolean ==
	{
		WRITE(x, stp?, ~v);
		WRITE(x, nxt?,  v);
		WRITE(x, cnt?, ~v);
	}

	set!(x:%, ignored:'conting?', v:Boolean):Boolean ==
	{
		WRITE(x, stp?, ~v);
		WRITE(x, nxt?, ~v);
		WRITE(x, cnt?,  v);
	}
#endif

	set!(x:%, ignored:'entered?', v:Boolean):Boolean ==
		WRITE(x, ent?, v);

	set!(x:%, ignored:'nextContext', v:CallContext):CallContext ==
		WRITE(x, ncon, v);

	set!(x:%, ignored:'intStack', v:IntStack):IntStack ==
		WRITE(x, intStack, v);

	set!(x:%, ignored:'verbose?', v:Boolean):Boolean ==
		WRITE(x, ver?, v);

	set!(x:%, ignored:'veryVerbose?', v:Boolean):Boolean ==
		WRITE(x, vve?, v);

	set!(x:%, ignored:'interactive?', v:Boolean):Boolean ==
		WRITE(x, int?, v);

	set!(x:%, ignored:'stackDepth', v:SingleInteger):SingleInteger ==
		WRITE(x, dept, v);

	set!(x:%, ignored:'callStack', v:List CON):List CON ==
		WRITE(x, call, v);

	set!(x:%, ignored:'intStackChain', v:List IntStack):List IntStack ==
		WRITE(x, iskchain, v);

	set!(x:%, ignored:'breakList', v:Array(BP)):Array(BP) ==
		WRITE(x, blis, v);

	set!(x:%, ignored:'dbOut', v:TextWriter):TextWriter ==
		WRITE(x, dbtw, v);

	set!(x:%, ignored:'pauseLevel', v:SingleInteger):SingleInteger ==
		WRITE(x, paws, v);
}

