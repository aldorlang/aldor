
#include "dbg_defs"

HelpPackage(dbOut:TextWriter): with
{
	help: List(String) -> ();
} == add
{
	import from TextWriter, String, WriterManipulator;

	-- Shorthand for large text messages.
	X(s) ==> dbOut << s << endnl;


	local generalHelp():() ==
	{
		X "Available commands:";
		X "  break: display or insert breakpoints.";
		X "   cont: continue execution of program.";
		X " delete: delete breakpoints.";
		X "disable: disable breakpoints.";
		X " enable: enable breakpoints.";
		X "   halt: abort the program.";
		X "   help: access this help system.";
		X "  hints: hints and tips for using the debugger.";
		X "    int: interactive or non-interactive debugging.";
		X "   next: move to next statement (stepping over calls).";
		X "    off: turn off the debugging system and continue.";
		X "   quit: abort the program.";
		X "   step: move to next statement (stepping into calls).";
		X "   tips: hints and tips for using the debugger.";
		X "verbose: display of event details.";
		X "  where: display a stack trace.";
		X "  print: print out the value of a variable.";
		X " update: update the value of a varibale.";
		X "  shell: execute a shell command.";
		X "";
		X "Use _"help <command>_" for more help on <command>.";
	}


	local breakHelp():() ==
	{
		X("Syntax: break [[<file>] <line>]");
		X("");
		X("With no arguments, this command lists the current set of");
		X("breakpoints. If one argument is specified then it must be");
		X("a valid line number in the current file. If two arguments");
		X("are given then the first is a filename and the second a");
		X("line number within that file.");
		X("");
		X("See also: delete, disable, enable");
	}


	local contHelp():() ==
	{
		X("This command takes no arguments and causes the debugging");
		X("system to continue executing until a breakpoint is hit or");
		X("an exception is thrown.");
	}


	local deleteHelp():() ==
	{
		X("Syntax: delete [<breakpoint list>]");
		X("");
		X("This command can be used to remove a breakpoint from the");
		X("set of active and inactive breakpoints.");
		X("");
		X("If no arguments are given, ALL breakpoints are deleted.");
		X("");
		X("See also: break, disable, enable");
	}


	local disableHelp():() ==
	{
		X("Syntax: disable [<breakpoint list>");
		X("");
		X("This command can be used to disable breakpoints. They will");
		X("remain in place but will not trigger the debugger if hit.");
		X("");
		X("If no arguments are given, ALL breakpoints are disabled.");
		X("");
		X("See also: break, delete, enable");
	}


	local enableHelp():() ==
	{
		X("Syntax: enable [<breakpoint list>]");
		X("");
		X("This command can be used to enable breakpoints that were");
		X("previously disabled.");
		X("");
		X("If no arguments are given, ALL breakpoints are enabled.");
		X("");
		X("See also: break, delete, enable");
	}


	local haltHelp():() ==
	{
		X("This command takes no arguments and causes the debugging");
		X("system and program to cease execution immediately.");
		X("");
		X("Alias: quit (or press Ctrl-D)");
	}


	local helpHelp():() ==
	{
		X("With no arguments, this command displays the commands that");
		X("are available under the debugging system. If an argument");
		X("is provided then help specific to that command is given.");
		X("");
		X("Suggestions on how to make the most of the debugger are");
		X("given in the hints and tips section. Use _"help hints_" or");
		X("_"help tips_" for details.");
	}


	local helpHint():() ==
	{
		X("Hints and Tips");
		X("");
		X("This section aims to provide helpful hints and tips for");
		X("getting the most out of the debugger.");
		X("");
		X("Make sure that your programs are compiled with no mactext");
		X("(-Mno-mactext) otherwise you may become very confused!");
		X("");
		X("Pressing ENTER on its own causes the debugger to repeat");
		X("the previous step, next or cont command.");
		X("");
		X("Obtain an execution trace of your program before you use");
		X("the interactive session. Either insert a trace() call or");
		X("use _"int off_" and _"cont_". For a more detailed trace");
		X("type _"verbose high_" before typing _"cont_".");
	}


	local intHelp():() ==
	{
		X("Syntax: int [on|off]");
		X("");
		X("With no arguments, this command displays the current");
		X("interaction status (on or off). The single argument _"on_"");
		X("or _"off_" can be used to change the interaction status.");
		X("");
		X("When the debugger is running in non-interactive mode it");
		X("will never enter an interactive session unless start()");
		X("is invoked. An indented trace is written to debug output.");
		X("");
		X("When the debugger is running interactively it will always");
		X("start an interactive session whenever a debug event has");
		X("occurred. Note that the verbose (qv) level is set to off");
		X("when this mode is entered.");
	}


	local nextHelp():() ==
	{
		X("Causes execution to continue until the next statement in");
		X("the current function is reached or until a breakpoint is");
		X("triggered. Function calls are not entered by this command");
		X("and will be stepped over.");
		X("");
		X("See also: break, cont, step");
	}


	local quitHelp():() ==
	{
		X("This command takes no arguments and causes the debugging");
		X("system and program to cease execution immediately.");
		X("");
		X("Alias: halt (or press Ctrl-D)");
	}


	local stepHelp():() ==
	{
		X("Causes execution to continue until the next statement is");
		X("reached. If the current statement is a function call then");
		X("it will be entered (if compiled under -Wdebugger) and the");
		X("execution will pause there. Compare this with the _"next_"");
		X("command which never steps into function calls.");
		X("");
		X("See also: cont, next");
	}


	local offHelp():() ==
	{
		X("This command is used to disable the debugging system. The");
		X("program will continue executing until it has been turned");
		X("on again by a start() function call.");
	}


	local verboseHelp():() ==
	{
		X("Syntax: verbose [on|off|high|low]");
		X("");
		X("With no arguments, this command displays the current");
		X("verbose setting. The arguments have the following effect:");
		X("");
		X("    on: debug events will be displayed when they occur.");
		X("   off: debug events will not be displayed.");
		X("  high: step events will be printed if in verbose mode.");
		X("   low: step events will not be printed.");
		X("");
		X("The high verbose setting tends to generate too much output");
		X("in non-interactive mode and is disabled on entering it.");
	}

	local printHelp():() ==
	{
		X("Syntax: print x");
		X("");
		X("where x is a variable or operation whose return value is printable.");
	}

	local updateHelp():() ==
	{
		X("Syntax: update x:=y");
		X("");
		X("where x is a variable, y is a variable or operation whose ");
		X("return value will be assigned to x.");
	}
		
	local shellHelp():() ==
	{
		X("Syntax: shell x");
		X("");
		X("where x is a shell command.For example: shell ls. ");
	}
	
	local whereHelp():() ==
	{
		X("Display a trace of the function calls which are active and");
		X("being traced. If a call is not being traced then it will");
		X("not appear in the listing even if it is active!");
	}


	-- Display help on the interactive features of the debugger.
	help(cmds:List String):() ==
	{
		local syntax():() ==
		{
			X("*** Syntax error: help [<command>]");
			X("*** Type _"help_" for more information.");
		}

		import from SingleInteger;
		local option:String;


		-- No arguments means show top-level help.
		(#cmds = 0) => { generalHelp(); return; }


		-- Help can take at most one argument.
		(#cmds > 1) => { syntax(); return; }


		-- Find out the command option.
		option := first cmds;


		-- Command specific help.
		--select option in
		{			
			if 	option = "break" 	then breakHelp();
			else if option = "cont"		then contHelp();
			else if option = "delete"	then deleteHelp();
			else if option = "disable"	then disableHelp();
			else if option = "enable"	then enableHelp();
			else if option = "halt"		then haltHelp();
			else if option = "help"		then helpHelp();
			else if option = "hints"	then helpHint();
			else if option = "int"		then intHelp();
			else if option = "next"		then nextHelp();
			else if option = "off"		then offHelp();
			else if option = "quit"		then quitHelp();
			else if option = "step"		then stepHelp();
			else if option = "tips"		then helpHint();
			else if option = "verbose"	then verboseHelp();
			else if option = "where"	then whereHelp();
			else if option = "print"        then printHelp();
			else if option = "update"	then updateHelp();
			else if option = "shell"	then shellHelp();
			else
			-- Didn't recognise that command.
			X "*** Sorry: I can't provide any help on that topic.";
		}
	}
}
